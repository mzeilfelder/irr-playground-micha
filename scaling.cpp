// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Scaling a texture (by going over CImage)
// And testing CImage::copyToScaling

#include <irrlicht.h>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;


irr::video::ITexture* createScaledTexture(irr::video::IVideoDriver* videoDriver, irr::video::ITexture* texture, irr::f32 scale)
{
	const irr::core::dimension2d<irr::u32>& texSize = texture->getSize();
	irr::video::IImage* image = videoDriver->createImage (texture, irr::core::position2d<irr::s32>(0,0), texSize);
	const irr::core::dimension2d<irr::u32> texSizeScaled((u32)((f32)texSize.Width*scale), (u32)((f32)texSize.Height*scale));
	irr::video::IImage* imageSmall = videoDriver->createImage(texture->getColorFormat(), texSizeScaled);
	image->copyToScaling(imageSmall);
	static int count = 1;
	irr::io::path p(++count);
	irr::video::ITexture* texScaled = videoDriver->addTexture(p, imageSmall);
	image->drop();
	imageSmall->drop();

	return texScaled;
}

class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			if (!event.KeyInput.PressedDown)
			{
				switch ( event.KeyInput.Key )
				{
				case KEY_PLUS:
				case KEY_ADD:
					Scale += 0.1f;
				break;
				case KEY_MINUS:
				case KEY_SUBTRACT:
					Scale -= 0.1f;
				break;
				default:
				break;
				}
			}
		}
		return false;
	}

	irr::f32 Scale = 1.f;
};

int main()
{
	irr::SIrrlichtCreationParameters params;
	params.DriverType = video::EDT_OPENGL;
	params.WindowSize = core::dimension2d<u32>(1000, 800);
	IrrlichtDevice* device = createDeviceEx(params);

	MyEventReceiver eventHandler;
	device->setEventReceiver(&eventHandler);

	if (!device)
		return 1;

	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	video::ITexture* textureOrig = driver->getTexture("my_media/patterns_270x270.png");
	video::ITexture* textureAlphaOrig = driver->getTexture("my_media/redgreenbluewhiteblackalpha_64x64.png");
	textureOrig->grab();
	textureAlphaOrig->grab();

	gui::IGUIImage *imgTexture = env->addImage(textureOrig, core::position2d<s32>(1, 100));
	gui::IGUIImage *imgTextureAlpha = env->addImage(textureAlphaOrig, core::position2d<s32>(1, 1));

	irr::f32 oldScale = eventHandler.Scale;
	device->setWindowCaption( irr::core::stringw(eventHandler.Scale).c_str() );
	while (device->run())
	{
		if (device->isWindowActive())
		{
			if ( oldScale != eventHandler.Scale )
			{
				video::ITexture* prevTexture = imgTexture->getImage();
				video::ITexture* prevTextureAlpha = imgTextureAlpha->getImage();
				video::ITexture* scaledTex = createScaledTexture(driver, textureOrig, eventHandler.Scale);
				video::ITexture* scaledTexAlpha = createScaledTexture(driver, textureAlphaOrig, eventHandler.Scale);
				core::dimension2du dimTex(scaledTex->getSize());
				core::dimension2du dimTexAlpha(scaledTexAlpha->getSize());
				imgTexture->setImage(scaledTex);
				imgTextureAlpha->setImage(scaledTexAlpha);
				imgTextureAlpha->setRelativePosition( core::recti(1,1,dimTexAlpha.Width+1, dimTexAlpha.Height+1) );
				imgTexture->setRelativePosition(  core::recti(1, dimTexAlpha.Height+10, dimTex.Width+1, dimTexAlpha.Height+10+dimTex.Height) );
				driver->removeTexture(prevTexture);
				driver->removeTexture(prevTextureAlpha);
				device->setWindowCaption( irr::core::stringw(eventHandler.Scale).c_str() );
				oldScale = eventHandler.Scale;
			}


			driver->beginScene(true, true, video::SColor(255, 0, 0, 255));

			env->drawAll();

			driver->endScene();
		}
	}

	textureOrig->drop();
	textureAlphaOrig->drop();

	device->drop();
	return 0;
}
