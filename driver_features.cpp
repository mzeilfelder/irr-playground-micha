// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Display driver features.

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

const char* FeatureAsString(irr::video::E_VIDEO_DRIVER_FEATURE feature)
{
	using namespace video;
	switch ( feature )
	{
		case EVDF_RENDER_TO_TARGET: 		return "render_to_target";
		case EVDF_HARDWARE_TL:				return "hardware_tl";
		case EVDF_MULTITEXTURE:				return "multitexture";
		case EVDF_BILINEAR_FILTER: 			return "bilinear_filter";
		case EVDF_MIP_MAP: 					return "mip_map";
		case EVDF_MIP_MAP_AUTO_UPDATE: 		return "mip_map_auto_update";
		case EVDF_STENCIL_BUFFER: 			return "stencil_buffer";
		case EVDF_VERTEX_SHADER_1_1:		return "vertex_shader_1_1";
		case EVDF_VERTEX_SHADER_2_0:		return "vertex_shader_2_0";
		case EVDF_VERTEX_SHADER_3_0:		return "vertex_shader_3_0";
		case EVDF_PIXEL_SHADER_1_1:			return "pixel_shader_1_1";
		case EVDF_PIXEL_SHADER_1_2:			return "pixel_shader_1_2";
		case EVDF_PIXEL_SHADER_1_3:			return "pixel_shader_1_3";
		case EVDF_PIXEL_SHADER_1_4:			return "pixel_shader_1_4";
		case EVDF_PIXEL_SHADER_2_0:			return "pixel_shader_2_0";
		case EVDF_PIXEL_SHADER_3_0:			return "pixel_shader_3_0";
		case EVDF_ARB_VERTEX_PROGRAM_1:		return "arb_vertex_program_1";
		case EVDF_ARB_FRAGMENT_PROGRAM_1:	return "arb_fragment_program_1";
		case EVDF_ARB_GLSL:					return "arb_glsl";
		case EVDF_HLSL:						return "hlsl";
		case EVDF_TEXTURE_NSQUARE:			return "texture_nsquare";
		case EVDF_TEXTURE_NPOT:				return "texture_npot";
		case EVDF_FRAMEBUFFER_OBJECT:		return "framebuffer_object";
		case EVDF_VERTEX_BUFFER_OBJECT:		return "vertex_buffer_object";
		case EVDF_ALPHA_TO_COVERAGE:		return "alpha_to_coverage";
		case EVDF_COLOR_MASK:				return "color_mask";
		
		case EVDF_MULTIPLE_RENDER_TARGETS:	return "multiple_render_targets";
		case EVDF_MRT_BLEND:				return "mrt_blend";
		case EVDF_MRT_COLOR_MASK:			return "mrt_color_mask";
		case EVDF_MRT_BLEND_FUNC: 			return "mrt_blend_func";
		case EVDF_GEOMETRY_SHADER:			return "geometry_shader";
		
		case EVDF_OCCLUSION_QUERY:			return "occlusion_query";
		case EVDF_POLYGON_OFFSET:			return "polygon_offset";
		case EVDF_BLEND_OPERATIONS:			return "blend_operations";
		case EVDF_TEXTURE_MATRIX:			return "texture_matrix";
		case EVDF_TEXTURE_COMPRESSED_DXT:	return "texture_compressed_dxt";
		
		case EVDF_COUNT:					return "count is not a feature";
	}

	return "";
}

void LogDriverFeatures(irr::video::IVideoDriver* driver)
{
	if (!driver)
		return;

	for ( int i=0; i < (int)video::EVDF_COUNT; ++i )
	{
		printf("%s : %s\n",  FeatureAsString((E_VIDEO_DRIVER_FEATURE)i), driver->queryFeature((E_VIDEO_DRIVER_FEATURE)i) ? "true" : "false" );
	}
}

int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	
	LogDriverFeatures(driver);
		
	device->drop();

	return 0;
}

