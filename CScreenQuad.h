// Written by Ahmed Hilali
// Some changes by Michael Zeilfelder

#ifndef CSCREENQUAD_H
#define CSCREENQUAD_H

#include <irrlicht.h>

class ScreenQuadCB : public irr::video::IShaderConstantSetCallBack
{
public:
	// mapSize only used when fixedMapSize is true, otherwise current RT size passed to shader
	//\param pixelTextureIDs For manually passing texture-id's to OpenGL (was once necessary). But it's better to set "#version 430 compatibility" and use layout binding in shader now.
	ScreenQuadCB(const irr::core::dimension2d<irr::u32>& mapSize, bool fixedMapSize, const irr::core::array<irr::core::stringc>& pixelTextureIDs = irr::core::array<irr::core::stringc>())
		: MapSize(mapSize), FixedMapSize(fixedMapSize), PixelTextureIDs(pixelTextureIDs), ScreenxvID(-1), ScreenyvID(-1)
	{}

	virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
	{
		if ( ScreenxvID < 0  ) // first update
		{
			ScreenxvID = services->getVertexShaderConstantID("SCREENXV");
			ScreenyvID = services->getVertexShaderConstantID("SCREENYV");

			// texture id's (needed only for opengl)
			for (irr::s32 i = 0; i < (irr::s32)PixelTextureIDs.size(); ++i)
			{
				irr::s32 textureConstantId = services->getPixelShaderConstantID(PixelTextureIDs[i].c_str());
				services->setPixelShaderConstant(textureConstantId, &i, 1);
			}
		}

		irr::f32 screenX, screenY;

		if(FixedMapSize)
		{
			screenX = static_cast<irr::f32>(MapSize.Width);
			screenY = static_cast<irr::f32>(MapSize.Height);
		}
		else
		{
			screenX = static_cast<irr::f32>(services->getVideoDriver()->getCurrentRenderTargetSize().Width);
			screenY = static_cast<irr::f32>(services->getVideoDriver()->getCurrentRenderTargetSize().Height);
		}
		services->setVertexShaderConstant(ScreenxvID, &screenX, 1);
		services->setVertexShaderConstant(ScreenyvID, &screenY, 1);
	}

private:
	irr::core::dimension2d<irr::u32> MapSize;
	bool FixedMapSize;
	irr::core::array<irr::core::stringc> PixelTextureIDs;
	irr::s32 ScreenxvID;
	irr::s32 ScreenyvID;
};

// ScreenQuad is 2 triangles which have vertices at the borders of the clipspace. 
// So no vertex transformation needed inside the shader.
// You just write into the current rendertarget
class CScreenQuad
{
public:

	irr::video::SMaterial Material;

	CScreenQuad(irr::f32 flipY = 0.f)
		: FlipY(flipY)
	{
		Material.Wireframe = false;
		Material.Lighting = false;
		Material.ZWriteEnable = false;

		MeshBuffer = new irr::scene::SMeshBuffer();
		MeshBuffer->Vertices.set_used(6);
		MeshBuffer->Indices.set_used(6);

		updateVertices();

		for(irr::u32 i = 0;i < 6;++i)
			MeshBuffer->Indices[i] = i;
	}

	~CScreenQuad()
	{
		MeshBuffer->drop();
	}

	void disableTextureFilters(irr::u32 layer=0)
	{
		Material.TextureLayer[layer].BilinearFilter = false;
		Material.TextureLayer[layer].TrilinearFilter = false;
		Material.TextureLayer[layer].AnisotropicFilter = 0;
	}

	void render(irr::video::IVideoDriver* driver)
	{
		driver->setMaterial(Material);
		driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
		driver->drawMeshBuffer(MeshBuffer);
	}

	void setVertexAlpha(irr::u32 alpha)
	{
		VertexColor.setAlpha(alpha);
		for ( int i=0; i<6; ++i )
			MeshBuffer->Vertices[i].Color.setAlpha(alpha);
	}

	void setVertexColor(irr::video::SColor& col)
	{
		VertexColor = col;
		void updateVertices();
	}

	irr::video::SColor getVertexColor() const
	{
		return VertexColor;
	}

	// Set to 0.f (no flip) or 1.f (flip). Flips uv's for input texture upside down.
	void setFlipY(irr::f32 flipy)
	{
		FlipY = flipy;
		updateVertices();
	}

	irr::f32 getFlipY() const
	{
		return FlipY;
	}

	void setDepth(float depth)
	{
		Depth = depth;
		updateVertices();
	}

	irr::f32 getDepth() const
	{
		return Depth;
	}

protected:
	void updateVertices()
	{
		MeshBuffer->Vertices[0] = irr::video::S3DVertex(-1.0f, -1.0f, Depth, 0, 0, 1, VertexColor, 0.0f, 1.f - FlipY);
		MeshBuffer->Vertices[1] = irr::video::S3DVertex(-1.0f, 1.0f, Depth, 0, 0, 1, VertexColor, 0.0f, FlipY);
		MeshBuffer->Vertices[2] = irr::video::S3DVertex(1.0f, 1.0f, Depth, 0, 0, 1, VertexColor, 1.0f, FlipY);
		MeshBuffer->Vertices[3] = irr::video::S3DVertex(1.0f, -1.0f, Depth, 0, 0, 1, VertexColor, 1.0f, 1.f - FlipY);
		MeshBuffer->Vertices[4] = irr::video::S3DVertex(-1.0f, -1.0f, Depth, 0, 0, 1, VertexColor, 0.0f, 1.f - FlipY);
		MeshBuffer->Vertices[5] = irr::video::S3DVertex(1.0f, 1.0f, Depth, 0, 0, 1, VertexColor, 1.0f, FlipY);
	}

private:
	irr::scene::SMeshBuffer * MeshBuffer = nullptr;
	irr::video::SColor VertexColor = irr::video::SColor(0, 255, 255, 255);

	// Nearest z-clip is 0 on D3D and -1 on opengl. But using 0 always is fine as long as we don't want to enforce the ScreenQuad to be at the front.
	// Clipping space is cuboid (and not for example a frustum), so any distance which is still inside is usually OK here.
	irr::f32 Depth = 0.f;

	// 0 for no flip and 1 for flipping input texture (mirroring at x-axis through center)
	// Reason to flip the input is usually because OpenGL flips the output (y going up in RTT's).
	// So when rendering to a rtt setting this to 1.0 keeps input/output textures the same way on GL.
	irr::f32 FlipY = 0.f;	
};

#endif
