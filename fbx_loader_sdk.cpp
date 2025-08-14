#include "fbx_loader_sdk.h"

// Do no include fbxsdk headers in codeanalysis
#include <codeanalysis\warnings.h>
#pragma warning( push )
#pragma warning ( disable : ALL_CODE_ANALYSIS_WARNINGS )

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include <fbxsdk/core/fbxsystemunit.h>
#include <fbxsdk/utils/fbxgeometryconverter.h>

#pragma warning( pop )

// Copied (with minor changes) from fbxsdk samples/Common/Common.cxx
bool FbxUtils::LoadScene(fbxsdk::FbxManager* pManager, fbxsdk::FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int lAnimStackCount;
    bool lStatus;

	fbxsdk::FbxIOSettings* ios = pManager->GetIOSettings();

    // Get the file version number generate by the FBX SDK.
    fbxsdk::FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        FBXSDK_printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        FBXSDK_printf("\n");

        for(int i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            FBXSDK_printf("    Animation Stack %d\n", i);
            FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            FBXSDK_printf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        ios->SetBoolProp(IMP_FBX_MATERIAL,        true);
        ios->SetBoolProp(IMP_FBX_TEXTURE,         true);
        ios->SetBoolProp(IMP_FBX_LINK,            true);
        ios->SetBoolProp(IMP_FBX_SHAPE,           true);
        ios->SetBoolProp(IMP_FBX_GOBO,            false);
        ios->SetBoolProp(IMP_FBX_ANIMATION,       false);
        ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);
	if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
	{
#if 0	// We're not supporting password files right now
		FBXSDK_printf("Please enter password: ");

		char lPassword[1024];
		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		ios->SetStringProp(IMP_FBX_PASSWORD, lString);
		ios->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
#endif
	}

	if (!lStatus || (lImporter->GetStatus() != FbxStatus::eSuccess))
	{
		FBXSDK_printf("********************************************************************************\n");
		if (lStatus)
		{
			FBXSDK_printf("WARNING:\n");
			FBXSDK_printf("   The importer was able to read the file but with errors.\n");
			FBXSDK_printf("   Loaded scene may be incomplete.\n\n");
		}
		else
		{
			FBXSDK_printf("Importer failed to load the file!\n\n");
		}

		if (lImporter->GetStatus() != FbxStatus::eSuccess)
			FBXSDK_printf("   Last error message: %s\n", lImporter->GetStatus().GetErrorString());

		FbxArray<FbxString*> history;
		lImporter->GetStatus().GetErrorStringHistory(history);
		if (history.GetCount() > 1)
		{
			FBXSDK_printf("   Error history stack:\n");
			for (int i = 0; i < history.GetCount(); i++)
			{
				FBXSDK_printf("      %s\n", history[i]->Buffer());
			}
		}
		FbxArrayDelete<FbxString*>(history);
		FBXSDK_printf("********************************************************************************\n");
	}

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

FbxSceneLoader::~FbxSceneLoader()
{
	clearScene();
	if ( sdkManager )
		sdkManager->Destroy();
}

void FbxSceneLoader::clearScene()
{
	for ( irr::u32 i=0; i<IrrMeshes.size(); ++i )
	{
		IrrMeshes[i].irrMesh->drop();
	}
	IrrMeshes.clear();
	MapFbxMeshIndices.clear();
	NodeInfos.clear();
}

bool FbxSceneLoader::loadScene(const char* filename)
{
	if ( !filename )
		return false;

	// We use lazy generation for sdkManager as we want to delay dll loading as long as possible when working with delay loaded DLL's
	if ( !sdkManager )	
	   sdkManager = fbxsdk::FbxManager::Create();
	if ( !sdkManager )
		return false;

    // Create an IOSettings object.
    fbxsdk::FbxIOSettings * ios = fbxsdk::FbxIOSettings::Create(sdkManager, IOSROOT );
    sdkManager->SetIOSettings(ios);
	
	// Create scene
	const char* sceneName = "fbx_imported_scene";
	fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(sdkManager, sceneName);

	bool loaded = FbxUtils::LoadScene(sdkManager, fbxScene, filename);
	if ( !loaded )
	{
		return false;
	}

	irr::core::splitFilename(irr::io::path(filename), &fbxFilePath);

	// Ignore original axes and convert to the system Irrlicht uses (left handed with Y up)
	fbxsdk::FbxAxisSystem loadAxis = fbxScene->GetGlobalSettings().GetAxisSystem();
	fbxsdk::FbxAxisSystem axisSystem(fbxsdk::FbxAxisSystem::eDirectX);
	axisSystem.DeepConvertScene(fbxScene);

	// T1 works with 1 unit = 1 inch. TODO: check if this is correct for input 
	// TODO: this should probably be a parameter of the loader
	fbxsdk::FbxSystemUnit importUnits(2.54); // fbxsdk::FbxSystemUnit::Inch (can't use static variables with delay-loaded dll)
	if ( importUnits != fbxScene->GetGlobalSettings().GetOriginalSystemUnit() )
	{
		fbxsdk::FbxSystemUnit::ConversionOptions convOpt;	// have to set manually as 'DefaultConversionOptions' is another static variable I can't use with delay-loaded dll
		convOpt.mConvertRrsNodes = true;
		convOpt.mConvertLimits = true;
		convOpt.mConvertClusters = true;
		convOpt.mConvertLightIntensity = true;
		convOpt.mConvertPhotometricLProperties = true;
		convOpt.mConvertCameraClipPlanes = true;
		importUnits.ConvertScene(fbxScene, convOpt);
	}

	const int countGeometry = fbxScene->GetGeometryCount();
	if (countGeometry < 1 )
	{
		return false;
	}

	// Replace all meshes in the scene with triangulated versions
	fbxsdk::FbxGeometryConverter geomConv(sdkManager);
	fbxsdk::FbxArray<fbxsdk::FbxNode*> nodesWithBadPolygonsRemoved;
	geomConv.RemoveBadPolygonsFromMeshes(fbxScene, &nodesWithBadPolygonsRemoved);
	bool triangulated = geomConv.Triangulate(fbxScene, true);

	for ( int g=0; g < countGeometry; ++ g )
	{
		fbxsdk::FbxGeometry* geometry = fbxScene->GetGeometry(g);
		if ( !geometry )
			continue;
		if ( geometry->GetAttributeType() != fbxsdk::FbxNodeAttribute::eMesh )
			continue; // we only support meshes for now
		fbxsdk::FbxMesh *fbxMesh = static_cast<fbxsdk::FbxMesh*>(geometry);
		SIrrMeshWithFbxMaterialIndices meshWithFbxMats = createIrrMesh(fbxMesh);
		if (!meshWithFbxMats.irrMesh)
		{
			continue;
		}
		IrrMeshes.push_back(meshWithFbxMats);
		MapFbxMeshIndices[fbxMesh] = IrrMeshes.size()-1;
	}

	const int countNodes = fbxScene->GetNodeCount();
	if ( countNodes == 0 )
	{
		// we create one dummy node for first mesh
		NodeInfo ni;
		ni.mesh = IrrMeshes[0].irrMesh;
		for ( irr::u32 i=0; i< ni.mesh->getMeshBufferCount(); ++i )
			ni.materials.push_back(irr::video::SMaterial());	// we create same amount, but without node we don't really have any material info
		NodeInfos.push_back(ni);
	}
	else
	{
		for ( int i=0; i < countNodes; ++i )
		{
			addNodeWithChildren(fbxScene->GetNode(i));
		}
	}

	fbxScene->Destroy(true);

	return loaded;
}

irr::video::SColor FbxSceneLoader::toIrrColor(const fbxsdk::FbxColor& fbxColor) const
{
	return irr::video::SColor((irr::u32)(fbxColor.mAlpha*255.0), (irr::u32)(fbxColor.mRed*255.0), (irr::u32)(fbxColor.mGreen*255.0), (irr::u32)(fbxColor.mBlue*255.0));
}

irr::core::matrix4 FbxSceneLoader::toIrrMatrix(const fbxsdk::FbxAMatrix& fbxMatrix) const
{
	const double* m = fbxMatrix;
	return irr::core::matrix4( 
		(irr::f32)m[0], (irr::f32)m[1], (irr::f32)m[2], (irr::f32)m[3],
		(irr::f32)m[4], (irr::f32)m[5], (irr::f32)m[6], (irr::f32)m[7],
		(irr::f32)m[8], (irr::f32)m[9], (irr::f32)m[10], (irr::f32)m[11],
		(irr::f32)m[12], (irr::f32)m[13], (irr::f32)m[14], (irr::f32)m[15]);
}

irr::video::ITexture* FbxSceneLoader::getIrrTexture(const char* relativeFileName)
{
	// Note: could be a bit optimized with non-irrlicht functions
	// For example fbx files can return absolute paths even when requesting relative names (guess depends on the tool which created them)
	// So no need trying to merge those (just had no function in Irrlicht to check if it _is_ an absolute name)
	// Also could check if files exist before getTexture calls to avoid several log-warnings for this stuff if a file is not found)
	irr::io::path relativePath, filename, extension;
	irr::core::splitFilename(irr::io::path(relativeFileName), &relativePath, &filename, &extension);
	irr::io::path texturePath = irr::core::mergeFilename(fbxFilePath+relativePath, filename, extension);
	irr::video::ITexture* texture = videoDriver->getTexture(texturePath);
	if ( !texture )
	{
		texturePath = irr::core::mergeFilename(relativePath, filename, extension);
		texture = videoDriver->getTexture(texturePath);
		if ( !texture )
		{
			texturePath = irr::core::mergeFilename(fbxFilePath, filename, extension);
			texture = videoDriver->getTexture(texturePath);
			if ( !texture )
			{
				texturePath = irr::core::mergeFilename(irr::io::path(), filename, extension);
				texture = videoDriver->getTexture(texturePath);
			}
		}
	}

	return texture;
}


void FbxSceneLoader::fbxToIrrMaterial(const fbxsdk::FbxSurfaceMaterial& fbxMaterial, irr::video::SMaterial& irrMaterial)
{
	auto getIrrColor = [this, &fbxMaterial](const char* propName, irr::video::SColor defaultColor ) -> irr::video::SColor
	{
		fbxsdk::FbxProperty prop = fbxMaterial.FindProperty(propName);
		if (prop.IsValid()) 
		{
            return toIrrColor((FbxColor)prop.Get<FbxColor>());
        }
        return defaultColor;
	};

	irrMaterial.DiffuseColor = getIrrColor("DiffuseColor", irrMaterial.DiffuseColor);	// fbxsdk::FbxSurfaceMaterial::sDiffuse (can't use static variables with delay-loaded dll)
	irrMaterial.AmbientColor = getIrrColor("AmbientColor", irrMaterial.AmbientColor);	// fbxsdk::FbxSurfaceMaterial::sAmbient
	irrMaterial.EmissiveColor = getIrrColor("EmissiveColor", irrMaterial.EmissiveColor);	// fbxsdk::FbxSurfaceMaterial::sEmissive
	irrMaterial.SpecularColor = getIrrColor("SpecularColor", irrMaterial.SpecularColor);	// fbxsdk::FbxSurfaceMaterial::sSpecular

    FbxProperty shininessProp = fbxMaterial.FindProperty("Shininess"); // fbxsdk::FbxSurfaceMaterial::sShininess
    irrMaterial.Shininess = shininessProp.IsValid() ? shininessProp.Get<float>() : 0.0f;

	// Transparency... bit difficult to say how to handle this best
	// We add it to diffuse color for now. But really depends a bit on the materials I think.
	FbxProperty transProp = fbxMaterial.FindProperty("TransparencyFactor");	// fbxsdk::FbxSurfaceMaterial::sTransparencyFactor
	if ( transProp.IsValid() )
	{
		float alpha = 1.f - transProp.Get<float>();
		irrMaterial.DiffuseColor.setAlpha( (irr::u32)((irr::f32)irrMaterial.DiffuseColor.getAlpha()*alpha));
	}

	auto getFbxFileTexture = [this, &fbxMaterial](const char* propName) -> fbxsdk::FbxFileTexture*
	{
		fbxsdk::FbxProperty prop = fbxMaterial.FindProperty(propName);
		if (prop.IsValid()) 
		{
			// Note: can't work with simpler prop.GetSrcObjectCount<fbxsdk::FbxTexture>() because of using delay loaded dll and fbxsdk casts use internally a static ClassId
			// So have to get class strings and do slow name comparisons
			int count = prop.GetSrcObjectCount(); 
			for ( int i=0; i<count; ++i )
			{
				fbxsdk::FbxObject* obj = prop.GetSrcObject(i);
				const char* typeName = obj->GetClassId().GetName();
				if (strcmp(typeName, "FbxFileTexture") == 0)
				{
					return static_cast<fbxsdk::FbxFileTexture*>(obj);
				}
			}
        }
        return nullptr;
	};

	fbxsdk::FbxFileTexture* fbxDiffuseTex = getFbxFileTexture("DiffuseColor");
	if ( fbxDiffuseTex )
	{
		irr::video::ITexture* irrTex = getIrrTexture(fbxDiffuseTex->GetRelativeFileName());
		irrMaterial.setTexture(ETL_DIFFUSE, irrTex);
	}

	fbxsdk::FbxFileTexture* fbxNormalTex = getFbxFileTexture("NormalMap");
	if ( fbxNormalTex )
	{
		irr::video::ITexture* irrTex = getIrrTexture(fbxNormalTex->GetRelativeFileName());
		irrMaterial.setTexture(ETL_NORMALMAP, irrTex);
		// Could also need "BumpFactor" (not sure if property is called that, would have to test)
	}

	// Other textures, not sure yet if we need one:
	// "Ambient", "Specular", "Emissive", "Bump", "Reflection", "TransparentColor", "DisplacementColor"
	// Maybe also "Shininess" (could be used for roughness)
}

void FbxSceneLoader::addNodeWithChildren(fbxsdk::FbxNode* fbxNode, irr::u32 parentIndex)
{
	NodeInfo ni;
	ni.parentIndex = parentIndex;

	// Find Irrlicht mesh for given fbx mesh 
	SIrrMeshWithFbxMaterialIndices irrMeshWithFbxMats;
	fbxsdk::FbxMesh* fbxMesh = fbxNode->GetMesh();
	if ( fbxMesh )
	{
		auto element = MapFbxMeshIndices.find(fbxMesh);
		if ( element )
		{
			irrMeshWithFbxMats = IrrMeshes[element->getValue()];
		}
	}

	// Get node materials
	if ( irrMeshWithFbxMats.irrMesh )
	{
		ni.mesh = irrMeshWithFbxMats.irrMesh;

		// Always add as many materials as we need
		// They are not really initialized to anything special at this point as 
		// fbx has no geometry materials
		for ( irr::u32 i=0; i < ni.mesh->getMeshBufferCount(); ++i )
		{
			ni.materials.push_back(ni.mesh->getMeshBuffer(i)->getMaterial());
		}

		for ( int i=0; i < fbxNode->GetMaterialCount(); ++i )
		{
			irr::u32 irrMaterialIndex = irrMeshWithFbxMats.fbxToIrrMaterialIndex(i);
			if ( irrMaterialIndex >= ni.materials.size() ) 
				continue; // should probably not happen

			irr::video::SMaterial& irrMaterial = ni.materials[irrMaterialIndex];
			fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(i);
			if ( fbxMaterial )
				fbxToIrrMaterial(*fbxMaterial, irrMaterial);
		}
	}

	// Get node transformation
	// Bit trouble here that fbx has more complex transformation options than Irrlicht
	// So just using fbxNode->LclTranslation, LclRotation, LclScaling often won't be enough
	// Sadly going over matrix transformations means we lose the original values for those 
	// even if we end up with the same end-result.

#if 1 
	// Giving up on the scene-graph and make it flat instead - all nodes end up on top and use their global transformation
	// So far the only solution I managed to get to work (for most models)
	ni.parentIndex = (irr::u32)-1;
	// TODO: Some models end up with bad rotation, I think those with PreRotation mess up. I couldn't figure out the reason and gave up.
	// No idea about parameters, in my test-models none made a real difference, so I left them all with defaults
	fbxsdk::FbxAMatrix localMatrix = fbxNode->EvaluateGlobalTransform(FBXSDK_TIME_INFINITE);
#else
	// Not sure yet why that wasn't good enough, but I didn't manage to get the correct axis transformations for the scene that way
	fbxsdk::FbxAMatrix localMatrix = fbxNode->EvaluateLocalTransform(FBXSDK_TIME_INFINITE, fbxsdk::FbxNode::eSourcePivot, true, true);
#endif

	ni.localTransform = toIrrMatrix(localMatrix);

	// add node
	NodeInfos.push_back(ni);
	irr::u32 nodeIndex = NodeInfos.size()-1;

	// recursively handle children
	int numChildren = fbxNode->GetChildCount();
	for ( int i=0; i < numChildren; ++i )
	{
		addNodeWithChildren(fbxNode->GetChild(i), nodeIndex);
	}
}

FbxSceneLoader::SIrrMeshWithFbxMaterialIndices FbxSceneLoader::createIrrMesh(fbxsdk::FbxMesh * fbxMesh)
{
	FbxSceneLoader::SIrrMeshWithFbxMaterialIndices result;

	if ( !fbxMesh->IsTriangleMesh() )
		return result;	// not supported, we should have triangulated meshes before this is called

	const int numPolygons = fbxMesh->GetPolygonCount();
	if ( numPolygons < 1 )
		return result;

	fbxsdk::FbxVector4* controlPoints = fbxMesh->GetControlPoints();
	if (!controlPoints )
		return result;

	// Bit tricky to get how many materials a mesh uses
	// Fbx has materials per polygon
	// It does have polygon groups as well and often they are per material, but no guarantee.
	// We could split our mesh buffers further per group, but ignoring those for now, meaning groups are lost on import.
	irr::core::map<int, irr::u32> polygonsPerMaterial;
	fbxsdk::FbxGeometryElementMaterial* elementMaterial = fbxMesh->GetElementMaterial(); // we only care about the first
	if ( elementMaterial && elementMaterial->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygon)
	{
		for ( int p=0; p < numPolygons; ++p )
		{
			int materialIndex = elementMaterial->GetIndexArray().GetAt(p);
			auto* node = polygonsPerMaterial.find(materialIndex);
			if ( node )
				node->getValue() ++;
			else
				polygonsPerMaterial.insert(materialIndex, 1);
		}
	}
	else
	{
		polygonsPerMaterial.insert(-1, numPolygons);
	}

	result.irrMesh = new irr::scene::SMesh();
	for ( auto it = polygonsPerMaterial.getIterator(); it.getNode(); it++ )
	{
		irr::scene::CDynamicMeshBuffer* mb = new irr::scene::CDynamicMeshBuffer(irr::video::EVT_STANDARD, irr::video::EIT_32BIT);

		irr::scene::IIndexBuffer &irrIndices = mb->getIndexBuffer();
		irr::scene::IVertexBuffer &irrVertices = mb->getVertexBuffer();

		// No shared vertices for now: one index = one vertex
		irr::u32 numIndices = it->getValue()*3;
		irr::u32 numVertices = numIndices;	
		irrVertices.reallocate(numVertices);
		irrIndices.set_used(numIndices);
		for ( irr::u32 i=0; i < numIndices; ++i )
			irrIndices.setValue(i, i);

		result.irrMesh->addMeshBuffer(mb);
		result.fbxMaterialIndices.push_back(it->getKey());
		mb->drop();	// irrMesh still has a reference
	}

	fbxsdk::FbxGeometryElementUV* elementUV = fbxMesh->GetElementUV(); // getting first one and ignoring further layers (could also try to find first fbxsdk::FbxLayerElement::eTextureDiffuse, not sure if better, but probably not in our case)
	//fbxsdk::FbxGeometryElementVertexColor* elementColor = fbxMesh->GetElementVertexColor();

	irr::video::S3DVertex irrVertex;
	fbxsdk::FbxVector4 fbxNormal;
	fbxsdk::FbxVector2 fbxUV;
	bool pUnmapped = false; // not used, same as result of GetPolygonVertexUV
	for ( int p=0; p < numPolygons; ++p )
	{
		int fbxIndex = elementMaterial ? elementMaterial->GetIndexArray().GetAt(p) : -1;
		irr::scene::CDynamicMeshBuffer* mb = static_cast<irr::scene::CDynamicMeshBuffer*>(result.irrMesh->getMeshBuffer(result.fbxToIrrMaterialIndex(fbxIndex)));
		irr::scene::IVertexBuffer &irrVertices = mb->getVertexBuffer();
		for ( int s=0; s < 3; ++s )
		{
			int idx = fbxMesh->GetPolygonVertex(p, s);
			double* val4 = (double*)(controlPoints[idx]);
			irrVertex.Pos.set( (irr::f32)val4[0], (irr::f32)val4[1], (irr::f32)val4[2]);

			if ( fbxMesh->GetPolygonVertexNormal(p, s, fbxNormal) )
				irrVertex.Normal.set( (irr::f32)fbxNormal.mData[0], (irr::f32)fbxNormal.mData[1], (irr::f32)fbxNormal.mData[2] );

			if ( elementUV && fbxMesh->GetPolygonVertexUV(p, s, elementUV->GetName(), fbxUV, pUnmapped) )
				irrVertex.TCoords.set((irr::f32)fbxUV[0], 1.f-(irr::f32)fbxUV[1]);

			// TODO: vertex colors, currently ignoring as they are a bit work with all the different mapping modes (and I didn't need them yet)
			//if ( elementColor )
			//{
			//}

			irrVertices.push_back(irrVertex);
		}
	}

	for ( irr::u32 i=0; i<result.irrMesh->getMeshBufferCount(); ++i)
	{
		result.irrMesh->getMeshBuffer(i)->recalculateBoundingBox();
	}
	result.irrMesh->recalculateBoundingBox();

	return result;
}

/// 
/// Code below just some test to show loader work
/// 

#if defined(USE_BATCHING_MESH)
#include "CBatchingMesh.h" 
#endif

using namespace irr;
using namespace gui;
using namespace scene;
using namespace video;

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(IrrlichtDevice * device) 
		: DlgFileOpen(0), Device(device)
	{ }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			//s32 id = event.GUIEvent.Caller->getID();
			switch(event.GUIEvent.EventType)
			{
				case EGET_DIRECTORY_SELECTED:
				{
					//IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
				}
				break;
				case EGET_FILE_SELECTED:
				{
					//IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
					FbxSceneLoader fbxLoader(Device->getVideoDriver());
					irr::core::stringc f = DlgFileOpen->getFileNameP();
					if (   irr::core::hasFileExtension(f, "fbx") 
						&& fbxLoader.loadScene(f.c_str()) )
					{
						ISceneManager* smgr = Device->getSceneManager();
						smgr->clear();
						smgr->addCameraSceneNodeFPS();
						const irr::core::array<FbxSceneLoader::NodeInfo>& fxNodes = fbxLoader.getNodes();

#if defined(USE_BATCHING_MESH)
						irr::scene::CBatchingMesh* batchingMesh = new irr::scene::CBatchingMesh();

						for ( irr::u32 i=0; i<fxNodes.size(); ++i )
						{
							FbxSceneLoader::NodeInfo ni = fxNodes[i];
							if ( !ni.hasMesh() )
								continue;

							irr::core::matrix4 mat = fbxLoader.getAbsoluteTransformation(i);
							batchingMesh->addMesh( ni.mesh, ni.materials, mat);
						}
						batchingMesh->finalize();
						IMeshSceneNode* node = smgr->addMeshSceneNode(batchingMesh);
						node->setMaterialFlag(EMF_LIGHTING, false);
#else
						for ( irr::u32 i=0; i < fxNodes.size(); ++i )
						{
							FbxSceneLoader::NodeInfo ni = fxNodes[i];
							if ( !ni.hasMesh() )
								continue;

							irr::core::matrix4 mat = fbxLoader.getAbsoluteTransformation(i);
							IMeshSceneNode* node = smgr->addMeshSceneNode(ni.mesh);
							for ( irr::u32 m=0; m < ni.materials.size(); ++ m)
							{
								node->getMaterial(m) = ni.materials[m];
							}

							node->setMaterialFlag(EMF_LIGHTING, false);
						}
#endif
					}
					DlgFileOpen = 0;
				}
				break;
				case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				{
					DlgFileOpen = 0;
				}
				break;
			default:
				break;
			}
		}

		return false;
	}

	IGUIFileOpenDialog* DlgFileOpen;
	IrrlichtDevice* Device;
};

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();
	ISceneManager* smgr = device->getSceneManager();

	MyEventReceiver receiver(device);
	device->setEventReceiver(&receiver);

	receiver.DlgFileOpen = env->addFileOpenDialog(L"example file open dialog");

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));

			smgr->drawAll();
			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
