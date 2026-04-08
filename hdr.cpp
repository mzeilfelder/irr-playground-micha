#include "hdr.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_HDR	// stb_image could also load several other formats
#include "stb_image.h"

irr::video::ITexture* getHdrTexture(irr::video::IVideoDriver* driver, const irr::io::path& filename)
{
	// stbi_set_flip_vertically_on_load(true); // not sure yet
	int width=0, height=0, nrComponents=0;
	float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 4);
	if ( !data )
		return nullptr;
	if ( width < 1 || height < 1 )
	{
		delete[] data;
		return nullptr;
	}

	// Assuming .hdr has 32 bit (mine do... TODO: can check this with stb functions I think)
	using namespace irr;
	video::ITexture* result = nullptr;
	core::dimension2du dim(width, height);
	video::IImage* img = driver->createImageFromData(video::ECF_A32B32G32R32F, dim, data);
	delete[] data;

	if ( img )
	{
		result = driver->addTexture(filename, img);
		img->drop();
	}
	return result;
}
