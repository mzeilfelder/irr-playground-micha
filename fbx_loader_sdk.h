#pragma once

// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder (aside from FbxUtils::LoadScene which is mostly copied from sdk examples)

// Note: This needs the Autodesk fbxsdk (https://aps.autodesk.com/developer/overview/fbx-sdk) 
// You have to set your include paths and linker paths to it and link to libfbxsdk.lib


// Using CBatchingMesh to put whole scene into a single mesh
// #define USE_BATCHING_MESH

#include <irrlicht.h>

namespace fbxsdk
{
	class FbxManager;
	class FbxDocument;
	class FbxNode;
	class FbxMesh;
	class FbxSurfaceMaterial;
	class FbxColor;
	class FbxAMatrix;
};

struct FbxUtils
{
	// Note: you have to create the manager and scene first
	// scene is usually FbxScene*
	static bool LoadScene(fbxsdk::FbxManager* manager, fbxsdk::FbxDocument* scene, const char* filename);
};


// Fbx scene loader for Irrlicht using the Autodesk fbxsdk
// Note: Only loading static meshes, animations not supported
//       The static loader convers the fbxsdk data into Irrlich structures
//       When doing animations I might code this very different and _not_ convert to Irrlich structures at all 
//       Instead I'd likely keep fbxsdk and use their animation functions and the write some SceneNode to render those directly.
// Note: I needed to code this with delay loaded dll's in mind.
//       Some stuff could be simplified a bit when linking dll's the usual way or linking the fbxsdk statically.
// Note: I couldn't get node hierarchies working (parent-child nodes), so right now all nodes in a scene are with absolute 
//       coordinates and at top-level
class FbxSceneLoader
{
public:
	FbxSceneLoader(irr::video::IVideoDriver* driver)
		: videoDriver(driver)
	{}
	~FbxSceneLoader();

	void clearScene();
	bool loadScene(const char* filename);

	// Kinda like ISceneNode, but simpler
	// Could also work with ISceneNode, but that needs more memory allocations and is a bit less flexible 
	// For example we might decide to pass on attributes later etc, then this is easier to extent
	struct NodeInfo
	{
		// Mesh only has geometry buffers, materials are not initialized here as those are per node in fbx
		irr::scene::IMesh* mesh = nullptr;	// weak ptr, valid until clearScene is called
		irr::u32 parentIndex = (irr::u32)-1;	// for node hierarchies - currently not used

		irr::core::array<irr::video::SMaterial> materials;

		irr::core::matrix4 localTransform;	// Note: with no parentIndex it's just absolute

		bool hasMesh() const
		{
			return mesh != nullptr;
		}

		irr::core::matrix4 getTransformation() const
		{
			return localTransform;
		}
	};

	// Note: We only return nodes if there is more than one.
	// Otherwise we assume it's all about loading a single mesh
	const irr::core::array<NodeInfo>& getNodes() const
	{
		return NodeInfos;
	}

	// Only needed if you have a hierarchy - which is currently disabled in this loader as I couldn't get it working
	irr::core::matrix4 getAbsoluteTransformation(irr::u32 nodeIndex) const
	{
		const NodeInfo& ni = NodeInfos[nodeIndex];
		irr::core::matrix4 mat = ni.getTransformation();

		if ( ni.parentIndex == (irr::u32)-1 )
			return mat;

		return getAbsoluteTransformation(ni.parentIndex) * mat;
	}

protected:

	struct SIrrMeshWithFbxMaterialIndices
	{
		irr::scene::SMesh* irrMesh = nullptr;

		// original fbx material indices per irrlicht mesh buffer.
		// size is the same as number of mesh buffers in mesh
		irr::core::array<int> fbxMaterialIndices;

		inline irr::u32 fbxToIrrMaterialIndex(int fbxIndex ) const
		{
			const irr::u32 size = fbxMaterialIndices.size();
			for ( irr::u32 i=0; i < size; ++i )
				if ( fbxMaterialIndices[i] == fbxIndex )
					return i;
			return 0;
		}
	};

	// Convert fbxMesh to Irrlicht IMesh.
	// \param appendToMesh When not 0 append mesh buffers to this mesh instead of creating a new one
	SIrrMeshWithFbxMaterialIndices createIrrMesh(fbxsdk::FbxMesh * fbxMesh);

	void fbxToIrrMaterial(const fbxsdk::FbxSurfaceMaterial& fbxMaterial, irr::video::SMaterial& irrMaterial);
	irr::video::SColor toIrrColor(const fbxsdk::FbxColor& fbxColor) const;
	irr::core::matrix4 toIrrMatrix(const fbxsdk::FbxAMatrix& fbxMatrix) const;
	irr::video::ITexture* getIrrTexture(const char* relativeName);

	void addNodeWithChildren(fbxsdk::FbxNode* node, irr::u32 parentIndex=(irr::u32)-1);

private:
	irr::video::IVideoDriver* videoDriver = nullptr;
	fbxsdk::FbxManager* sdkManager = nullptr;
	irr::io::path fbxFilePath;

	irr::core::map<const fbxsdk::FbxMesh*, irr::u32> MapFbxMeshIndices;	// TODO: should be local and passed around as pointers become invalid after loading
	irr::core::array<SIrrMeshWithFbxMaterialIndices> IrrMeshes;
	irr::core::array<NodeInfo> NodeInfos;
};

// Which texture indeces are used in Irrlicht material for corresponding fbx stuff
// Modify according to your needs
enum ETextureLayer
{
	ETL_DIFFUSE = 0,
	ETL_NORMALMAP = 1
};
