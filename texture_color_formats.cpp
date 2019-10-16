// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Test using textures with different color formats

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

// test should run on older Irrlicht versions as well for comparison
#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 9 
	#define PRE_IRR_1_9
#endif

#ifdef PRE_IRR_1_9
	const c8* const ColorFormatNames[] =
	{
		"A1R5G5B5",
		"R5G6B5",
		"R8G8B8",
		"A8R8G8B8",
		0
	};
#endif

bool compareData(stringw& logText,  irr::u8* data1, irr::u8* data2, ECOLOR_FORMAT format1, ECOLOR_FORMAT format2, const irr::core::dimension2du& dim1, const irr::core::dimension2du& dim2)
{
	if( !data1 || !data2 )
	{
		logText += " data missing";
		return false;
	}

	if (format1 != format2)
	{
		logText += L" different color formats: ";
		logText += stringw(ColorFormatNames[format1]);
		logText += L" ";
		logText += stringw(ColorFormatNames[format2]);

		return false;
	}
	if ( dim1 != dim2 )
	{
		logText += L" different image sizes";
		return false;
	}

	u32 bitsPerPixel = IImage::getBitsPerPixelFromFormat(format1);
	u32 bytesPerPixel = bitsPerPixel/8;
	u32 dataSize = dim1.Width * dim1.Height * bytesPerPixel;

	for ( u32 i=0; i < dataSize; ++i )
	{
		if ( data1[i] != data2[i] )
		{
			logText += L"different data at pos ";
			logText += i;

			u32 end = core::min_(i+bytesPerPixel*10, dataSize);

			for ( u32 a=i; a<end; ++a )	// next 10 pixels
			{
				logText += (int)data1[a];
			}
			logText += L" ";
			for ( u32 a=i; a<end; ++a )	// next 10 pixels
			{
				logText += (int)data2[a];
			}

			return false;
		}
	}

	logText += L" equal : ";
	logText += stringw(ColorFormatNames[format1]);

	return true;
}

int main()
{
	SIrrlichtCreationParameters deviceParams;
	deviceParams.DriverType = video::EDT_OPENGL;
	//deviceParams.DriverType = video::EDT_DIRECT3D9;
	//deviceParams.DriverType = video::EDT_BURNINGSVIDEO;
	//deviceParams.DriverType = video::EDT_SOFTWARE;
	deviceParams.WindowSize = core::dimension2d<u32>(1600, 800);
	IrrlichtDevice * device = createDeviceEx(deviceParams);
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	const SColor bkCol(0,200,200,200);
	IGUIListBox * logBox = env->addListBox( recti(10,10, deviceParams.WindowSize.Width-10, 100), 0, -1, true );

	driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, false);
	//driver->setTextureCreationFlag(ETCF_ALWAYS_16_BIT, true);
	//driver->setTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL, true);
#ifndef PRE_IRR_1_9
	driver->setTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY, false);
#endif

	ITexture * rgbwbTex = driver->getTexture("my_media/redgreenbluewhiteblack_64x64.png");
	ITexture * rgbwbaTex = driver->getTexture("my_media/redgreenbluewhiteblackalpha_64x64.png");
	

	array<ECOLOR_FORMAT> colorFormatsToTest;

	// common formats
	colorFormatsToTest.push_back(ECF_A1R5G5B5);
	colorFormatsToTest.push_back(ECF_R5G6B5);   
	colorFormatsToTest.push_back(ECF_R8G8B8);
	colorFormatsToTest.push_back(ECF_A8R8G8B8);

	//// Compressed image formats.
	colorFormatsToTest.push_back(ECF_DXT1);
	colorFormatsToTest.push_back(ECF_DXT2);
	colorFormatsToTest.push_back(ECF_DXT3);
	colorFormatsToTest.push_back(ECF_DXT4);
	colorFormatsToTest.push_back(ECF_DXT5);
	colorFormatsToTest.push_back(ECF_PVRTC_RGB2);
	colorFormatsToTest.push_back(ECF_PVRTC_ARGB2);
	colorFormatsToTest.push_back(ECF_PVRTC_RGB4);
	colorFormatsToTest.push_back(ECF_PVRTC_ARGB4);
	colorFormatsToTest.push_back(ECF_PVRTC2_ARGB2);
	colorFormatsToTest.push_back(ECF_PVRTC2_ARGB4);
	colorFormatsToTest.push_back(ECF_ETC1);
	colorFormatsToTest.push_back(ECF_ETC2_RGB);
	colorFormatsToTest.push_back(ECF_ETC2_ARGB);

	// float formats
	colorFormatsToTest.push_back(ECF_R16F);
	colorFormatsToTest.push_back(ECF_G16R16F);
	colorFormatsToTest.push_back(ECF_A16B16G16R16F);
	colorFormatsToTest.push_back(ECF_R32F);
	colorFormatsToTest.push_back(ECF_G32R32F);
	colorFormatsToTest.push_back(ECF_A32B32G32R32F);

	// Unsigned normalized integer formats.
	colorFormatsToTest.push_back(ECF_R8);
	colorFormatsToTest.push_back(ECF_R8G8);
	colorFormatsToTest.push_back(ECF_R16);
	colorFormatsToTest.push_back(ECF_R16G16);

	// Depth and stencil formats.
	colorFormatsToTest.push_back(ECF_D16);
	colorFormatsToTest.push_back(ECF_D32);
	colorFormatsToTest.push_back(ECF_D24S8);

	const core::dimension2d<u32> imgSize(64, 64);
	array<ITexture*> TexturesFromImages;
	array<ITexture*> TexturesFromData;
	array<ITexture*> copiedTextures;
	array<ITexture*> rttTextures;
	array<ITexture*> TexturesFromRttData;
	for ( u32 i=0; i<colorFormatsToTest.size(); ++i )
	{
		ECOLOR_FORMAT format = colorFormatsToTest[i];

		u32 bitsPerPixel = IImage::getBitsPerPixelFromFormat(format);
		u32 bytesPerPixel = bitsPerPixel/8;
		if ( bytesPerPixel*8 != bitsPerPixel )
			continue;
		u32 dataSize = imgSize.Width* imgSize.Height * bytesPerPixel;	// Irrlicht 1.9 could use getDataSizeFromFormat
		u8* data = new u8[dataSize];

		// Fill with all colors (or a good selection of as many colors as we can fit into those pixels)
		// Note - it's not perfect. It's somewhat tricky having all colors in regular intervals without accidentally
		// stepping over certain bits. And not having completely chaotic results which are hard to interpret.
		// I haven't really found a mix yet which works super good.
		u64 valMax = (u64)pow(2.0, (double)bitsPerPixel);	// as many as possible of that format
		u64 numPixels = imgSize.Width * imgSize.Height;
		u64 stepVal = core::max_<u64>(1, valMax / numPixels);	// gives as regular values, but can step over bits or even bytes completely
		u64 fixVal = stepVal > 256 ? core::max_((u64)1, numPixels / 256) : 0xffff; // give each bit a bit more of a chance to be set

		u64 val = 0;
		u64 val2 = 0;
		for ( u32 a=0; a < dataSize; a += bytesPerPixel)
		{
			for ( u32 b=0; b < bytesPerPixel; ++b )
			{
				u8 n = (val >> (b*8)) & 255;
				n += (u8)(a/fixVal);		// results are nicer and sometimes easier to compare without this line, but then it can skip colors.
				data[a+b] = n;
			}
			val = (val+stepVal)%valMax;
		}

		irr::io::path name(i);

		ITexture * texFromData = driver->addTexture(imgSize, name + "d", format);
		if ( texFromData )
		{
			TexturesFromData.push_back(texFromData);

			if ( texFromData->getColorFormat() == format && imgSize == texFromData->getSize() )
			{
				u8* lockData = (u8*)texFromData->lock();
				if ( lockData ) 
					memcpy(lockData, data, dataSize);

				stringw logTxt(i);
				if ( !compareData(logTxt, data, lockData, format, texFromData->getColorFormat(), imgSize, texFromData->getSize()) )
				{
					logBox->addItem(logTxt.c_str());
				}
				texFromData->unlock();
			}
			else
			{
				stringw logTxt(i);
				logTxt  += " texture from data has change format or size ";
				logTxt += stringw(ColorFormatNames[format]);
				logTxt += L" ";
				logTxt += stringw(ColorFormatNames[texFromData->getColorFormat()]);
				logBox->addItem(logTxt.c_str());
				}
		}
		else
		{
			stringw logTxt(i);
			logTxt  += " can't create texture from data with format ";
			logTxt += stringw(ColorFormatNames[format]);
			logBox->addItem(logTxt.c_str());
		}

		IImage* img = driver->createImageFromData(format, imgSize, data);

		if (img)
		{
			ITexture * tex = driver->addTexture(name, img);
			img->drop();

			if ( tex )
			{
				TexturesFromImages.push_back( tex );
				u8* lockData = (u8*)tex->lock(ETLM_READ_ONLY);
				ECOLOR_FORMAT texColFormat = tex->getColorFormat();

				stringw logTxt(i);
				compareData(logTxt, data, lockData, format, texColFormat, imgSize, tex->getSize());
				logBox->addItem(logTxt.c_str());


				IImage* img2 = lockData ? driver->createImageFromData(texColFormat, tex->getSize(), lockData) : 0;

				tex->unlock();
				if ( img2 )
				{
					name += "c";
					tex = driver->addTexture(name, img2);
					copiedTextures.push_back( tex );
					img2->drop();
				}

				ITexture * texRt = driver->addRenderTargetTexture(imgSize, name + "rt", format);
				rttTextures.push_back(texRt);
				if ( !texRt )
				{
					stringw logTxt(name);
					logTxt += L" failed to create rtt";
					logBox->addItem(logTxt.c_str());
				}
			}
			else
			{
				stringw logTxt(i);
				logTxt += L" could not create texture from image with format ";
				logTxt += stringw(ColorFormatNames[format]);
				logBox->addItem(logTxt.c_str());
			}
		}

		delete[] data;
	}

	if ( TexturesFromImages.size() == rttTextures.size() )
	{
		driver->beginScene(true, true, bkCol);
		for ( u32 i=0; i<TexturesFromImages.size(); ++i )
		{
			if ( rttTextures[i] )
			{
				driver->setRenderTarget( rttTextures[i], true, true, bkCol);
				driver->draw2DImage(TexturesFromImages[i], irr::core::vector2di(0,0));
			}
		}
		driver->setRenderTarget( 0, false, false);
		driver->endScene();
	}

	for ( u32 i=0; i<rttTextures.size(); ++i )
	{
		ITexture * tex = rttTextures[i];
		ITexture * tex2 = 0;
		
		if ( tex )
		{
			u8* lockData = (u8*)tex->lock(ETLM_READ_ONLY);
			ECOLOR_FORMAT texColFormat = tex->getColorFormat();

			IImage* img = lockData ? driver->createImageFromData(texColFormat, tex->getSize(), lockData) : 0;
			if ( img )
			{
				path name(i);
				name += "rttdata";
				tex2 = driver->addTexture(name, img);
				img->drop();
			}
			tex->unlock();
		}
		TexturesFromRttData.push_back( tex2 );
	}

// To enable disable alpha drawing. Done with define because 1.8 didn't have this parameter yet
#ifdef PRE_IRR_1_9
	#define ALPHA
#else
	#define ALPHA ,true
#endif

	const s32 LEFT = 10;
	const s32 GAP_X = 5;
	const s32 GAP_Y = 5;

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, bkCol);
	
			irr::core::vector2di destPos(LEFT, logBox->getAbsolutePosition().LowerRightCorner.Y + GAP_Y);
			for ( u32 i=0; i<TexturesFromImages.size(); ++i )
			{
				driver->draw2DImage(TexturesFromImages[i], destPos ALPHA);
				destPos.X += TexturesFromImages[i]->getSize().Width + GAP_X;
			}

			destPos.X = LEFT;
			destPos.Y += imgSize.Height + GAP_Y;
			for ( u32 i=0; i<copiedTextures.size(); ++i )
			{
				driver->draw2DImage(copiedTextures[i], destPos ALPHA);
				destPos.X += copiedTextures[i]->getSize().Width + GAP_X;
			}

			destPos.X = LEFT;
			destPos.Y += imgSize.Height + GAP_Y;
			for ( u32 i=0; i<TexturesFromData.size(); ++i )
			{
				driver->draw2DImage(TexturesFromData[i], destPos ALPHA);
				destPos.X += TexturesFromData[i]->getSize().Width + GAP_X;
			}

			destPos.X = LEFT;
			destPos.Y += imgSize.Height + GAP_Y;
			for ( u32 i=0; i<rttTextures.size(); ++i )
			{
				if ( rttTextures[i] )
				{
					driver->draw2DImage(rttTextures[i], destPos ALPHA);
				}
				destPos.X += imgSize.Width + GAP_X;
			}

			destPos.X = LEFT;
			destPos.Y += imgSize.Height + GAP_Y;
			for ( u32 i=0; i<TexturesFromRttData.size(); ++i )
			{
				if ( TexturesFromRttData[i] )
				{
					driver->draw2DImage(TexturesFromRttData[i], destPos ALPHA);
				}
				destPos.X += imgSize.Width + GAP_X;
			}

			destPos.X = LEFT;
			destPos.Y += 2*imgSize.Height;
			driver->draw2DImage(rgbwbTex, destPos ALPHA);
			destPos.X += core::max_(rgbwbTex->getSize().Width, imgSize.Width) + GAP_X;
			driver->draw2DImage(rgbwbaTex, destPos ALPHA);

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
