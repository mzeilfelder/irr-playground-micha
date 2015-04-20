// Copyright (C) <2014> <Jehan-antoine vayssade>
// Ovan/Magun contact on irrlicht-fr.org or ovan@sleek-think.ovh
// Code is under the zlib license (same as Irrlicht)
// For conditions of distribution and use, see copyright notice in irrlicht.h
// File was altered by Michael Zeilfelder

#include "CGUIColorPicker.h"
#include <IGUIEnvironment.h>
#include <IGUIButton.h>
#include <IGUIScrollBar.h>
#include <IVideoDriver.h>
#include <irrMath.h>
#include <SColor.h>
#include <S3DVertex.h>

namespace
{
	// TODO: I needed color-constants a few times already. We should just add a bunch of them to Irrlicht
	const irr::video::SColor COL_WHITE = irr::video::SColor(255, 255, 255, 255);
	const irr::video::SColor COL_BLACK = irr::video::SColor(255, 0, 0, 0);
};

namespace irr
{
	// TODO: Either give this it's own class like SColorHSL has or add some functions to the HSL class.
    inline core::vector3df RGBftoHSV(const video::SColorf &rgb)
    {
        core::vector3df hsv;
 
        f32 M = core::max_(rgb.getRed(), rgb.getGreen(), rgb.getBlue());
        f32 m = core::min_(rgb.getRed(), rgb.getGreen(), rgb.getBlue());
        f32 C = M - m;
 
        if(C == 0)
            hsv.X = 0;
        else if(M <= rgb.getRed())
            hsv.X =((rgb.getGreen() - rgb.getBlue()) / C);
        else if(M <= rgb.getGreen())
            hsv.X =((rgb.getBlue() - rgb.getRed()) / C) + 2;
        else if(M <= rgb.getBlue())
            hsv.X =((rgb.getRed() - rgb.getGreen()) / C) + 4;
 
        hsv.X *= 60;
        if(hsv.X < 0)
            hsv.X += 360;
 
        hsv.Y = M;
 
        if(hsv.Y == 0) 
			hsv.Z = 0;
        else 
			hsv.Z = C / hsv.Y;
 
        return hsv;
    }
	
    namespace gui
    {
        CGUIColorPicker::CGUIColorPicker(IGUIEnvironment *environment, IGUIElement *parent, s32 id)
            : IGUIElement(EGUIET_COLOR_SELECT_DIALOG , environment, parent, id, core::rect<s32>(200, 200, 310, 360))
			, Close(0), Scroll(0), GradientTexture(0), IsGradient(false), IsColor(false), IsInside(false)
			, Background(video::SColor(64, 255, 255, 255))
			, ColorPos(0)
        {
            Close = Environment->addButton(core::rect<s32>(5, 140, 85, 156), this, 0, L"take this color");
 
            Scroll = Environment->addScrollBar(true, core::rect<s32>(5, 125, 85, 135), this);
            Scroll->setMin(0);
            Scroll->setMax(255);
            Scroll->setPos(255);
 
            createAlphaTexture();
            createGradientTexture();
 
            setPickedColor(video::SColor(255, 64, 64, 128));
            updateAbsolutePosition();
        }
		
        CGUIColorPicker::~CGUIColorPicker() 
        {
			if ( Close )
				Close->drop();
			if ( Scroll )
				Scroll->drop();
			if ( AlphaTexture )
				AlphaTexture->drop();
			if ( GradientTexture )
				GradientTexture->drop();
        }
		
        void CGUIColorPicker::createAlphaTexture() 
        {
            AlphaTexture = Environment->getVideoDriver()->addTexture(core::dimension2d<u32>(16, 16), "alpha", video::ECF_A8R8G8B8);
            u32 *tmp = (u32*) AlphaTexture->lock();
 
            video::SColor color;
 
            #define square(colorstart, sx, sy, sz, sw)                         \
                color = colorstart;                                            \
                for(int y=sy; y<sw; ++y)                                       \
                    for(int x=sx; x<sz; ++x)                                   \
                        color.getData(&tmp[x + y*16], video::ECF_A8R8G8B8);    \

            square(video::SColor(255, 153, 153, 153), 0, 0,  8,  8);
            square(video::SColor(255, 153, 153, 153), 8, 8, 16, 16);
            square(video::SColor(255, 102, 102, 102), 8, 0, 16,  8);
            square(video::SColor(255, 102, 102, 102), 0, 8,  8, 16);
 
            AlphaTexture->unlock();
        }
		
        void CGUIColorPicker::createGradientTexture() 
        {
			// TODO: can't use non pot textures
            GradientTexture = Environment->getVideoDriver()->addTexture(core::dimension2d<u32>(15, 151), "gradient", video::ECF_A8R8G8B8);
            u32 *tmp = (u32*) GradientTexture->lock();
 
            video::SColor from;
            video::SColor to;
 
            #define interpolate(colorstart, colorend, start, end)              \
                from = colorstart;                                             \
                to = colorend;                                                 \
                                                                               \
                for(int y=start; y<end; ++y)                                   \
                {                                                              \
                    video::SColor c = to.getInterpolated(from, (y-start)/25.f);\
                    for(int x=0; x<15; ++x)                                    \
                        c.getData(&tmp[x + y*15], video::ECF_A8R8G8B8);        \
                }
 
            interpolate(video::SColor(255, 255, 0, 0),   video::SColor(255, 255, 0, 255),   0,  25);
            interpolate(video::SColor(255, 255, 0, 255), video::SColor(255, 0, 0, 255),    25,  50);
 
            interpolate(video::SColor(255, 0, 0, 255), video::SColor(255, 0, 255, 255),  50,  75);
            interpolate(video::SColor(255, 0, 255, 255), video::SColor(255, 0, 255, 0),  75, 100);
 
            interpolate(video::SColor(255, 0, 255, 0), video::SColor(255, 255, 255, 0), 100, 125);
            interpolate(video::SColor(255, 255, 255, 0), video::SColor(255, 255, 0, 0), 125, 151);
 
            GradientTexture->unlock();
        }
		
        void CGUIColorPicker::setRelativePosition(const core::recti &r)
        {
			// The idea is that the color-picker can't be resized. But setRelativePosition is not a virtual function. So maybe we rather make this resizable?
			// Although making IGUIElement::setRelativePosition virtual sounds also like it would make sense.
            RelativeRect.UpperLeftCorner += r.UpperLeftCorner-RelativeRect.UpperLeftCorner;
        }
		
        bool CGUIColorPicker::OnEvent(const SEvent &event) 
        {
            if(event.EventType == EET_MOUSE_INPUT_EVENT)
            {
                core::vector2di pos(event.MouseInput.X, event.MouseInput.Y);
 
                if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                {
                    IsGradient = Gradient.isPointInside(pos);
                    IsColor = Box.isPointInside(pos);
                    IsInside = AbsoluteRect.isPointInside(pos);
                }
 
                if(event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
                {
                    // does not work since 1.7 (i think)
                    // probably need CGUIModalScreen if this module is add to the engine
                    //if(!AbsoluteRect.isPointInside(pos) && !IsInside)
                    //{
                    //    SEvent event;
                    //    event.EventType = EET_GUI_EVENT;
                    //    event.GUIEvent.Caller = this;
                    //    event.GUIEvent.Element = 0;
                    //    event.GUIEvent.EventType = EGET_ELEMENT_CLOSED;
                    //    Parent->OnEvent(event);
                    //    remove();
                    //    drop();
                    //}
                    IsGradient = IsColor = false;
                }
 
                if(IsGradient)
                {
                    if(pos.X < Gradient.UpperLeftCorner.X)
                        pos.X = Gradient.UpperLeftCorner.X;
                    if(pos.Y < Gradient.UpperLeftCorner.Y)
                        pos.Y = Gradient.UpperLeftCorner.Y;
 
                    if(pos.X > Gradient.LowerRightCorner.X)
                        pos.X = Gradient.LowerRightCorner.X;
                    if(pos.Y > Gradient.LowerRightCorner.Y)
                        pos.Y = Gradient.LowerRightCorner.Y;
 
                    ColorPos = pos.Y - Gradient.UpperLeftCorner.Y;
                    u32 *tmp =(u32*)GradientTexture->lock();
                    SomeColor.set(tmp[ColorPos*GradientTexture->getOriginalSize().Width]);
                    GradientTexture->unlock();
                    recalculatePickedColor();
                }
 
                if(IsColor)
                {
                    if(pos.X < Box.UpperLeftCorner.X)
                        pos.X = Box.UpperLeftCorner.X;
                    if(pos.Y < Box.UpperLeftCorner.Y)
                        pos.Y = Box.UpperLeftCorner.Y;
 
                    if(pos.X > Box.LowerRightCorner.X)
                        pos.X = Box.LowerRightCorner.X;
                    if(pos.Y > Box.LowerRightCorner.Y)
                        pos.Y = Box.LowerRightCorner.Y;
 
                    PickPos.X = pos.X - Box.UpperLeftCorner.X;
                    PickPos.Y = pos.Y - Box.UpperLeftCorner.Y;
                    recalculatePickedColor();
                }
 
                if(IsGradient || IsColor)
                    return true;
            }
 
            if(event.EventType == EET_GUI_EVENT)
            {
                switch(event.GUIEvent.EventType)
                {
                    case EGET_BUTTON_CLICKED:
                        SEvent event;
                        event.EventType = EET_GUI_EVENT;
                        event.GUIEvent.Caller = this;
                        event.GUIEvent.Element = 0;
                        event.GUIEvent.EventType = EGET_FILE_SELECTED;	// TODO: needs new event-type
                        Parent->OnEvent(event);
                    break;
                    case EGET_SCROLL_BAR_CHANGED:
                        recalculatePickedColor();
                        return true;
                    break;
					default:
					break;
                }
            }
 
            return IGUIElement::OnEvent(event);
        }
		
        void CGUIColorPicker::recalculatePickedColor() 
        {
            video::SColor hcolor = SomeColor.getInterpolated(COL_WHITE, PickPos.X/80.f);
 
            PickColor = COL_BLACK.getInterpolated(hcolor, PickPos.Y/80.f);
            PickColor.setAlpha(Scroll->getPos());
 
            Alpha = SomeColor;
            Alpha.setAlpha(0);
        }
		
        void CGUIColorPicker::setPickedColor(video::SColor c) 
        {
            PickColor = c;
 
            core::vector3df hsv = RGBftoHSV({
                c.getRed()/255.f,
                c.getGreen()/255.f,
                c.getBlue()/255.f,
                c.getAlpha()/255.f
            });
 
            ColorPos = 150-hsv.X/360.f*150.f;
            PickPos.X = hsv.Y*80.f;
            PickPos.Y = 80-(hsv.Z)*80.f;
 
            u32 *tmp =(u32*)GradientTexture->lock();
            SomeColor.set(tmp[ColorPos*GradientTexture->getOriginalSize().Width]);
            GradientTexture->unlock();
 
            Alpha = SomeColor;
            Alpha.setAlpha(0);
        }
		
        video::SColor CGUIColorPicker::getPickedColor() const 
        {
            return PickColor;
        }
		
        void CGUIColorPicker::setBackgroundColor(video::SColor b) 
        {
            Background = b;
        }
		
        video::SColor CGUIColorPicker::getBackgroundColor() const 
        {
            return Background;
        }
		
        void CGUIColorPicker::updateAbsolutePosition()
        {
            IGUIElement::updateAbsolutePosition();
 
            Box.UpperLeftCorner = AbsoluteRect.UpperLeftCorner;
            Box.LowerRightCorner = AbsoluteRect.UpperLeftCorner;
            Box.UpperLeftCorner.X += 5;
            Box.UpperLeftCorner.Y += 5;
            Box.LowerRightCorner.X += 85;
            Box.LowerRightCorner.Y += 85;
 
            Gradient.UpperLeftCorner = AbsoluteRect.UpperLeftCorner;
            Gradient.LowerRightCorner = AbsoluteRect.UpperLeftCorner;
            Gradient.UpperLeftCorner.X += 90;
            Gradient.UpperLeftCorner.Y += 5;
            Gradient.LowerRightCorner.X += 105;
            Gradient.LowerRightCorner.Y += 155;
 
            PickBox.UpperLeftCorner = AbsoluteRect.UpperLeftCorner;
            PickBox.LowerRightCorner = AbsoluteRect.UpperLeftCorner;
            PickBox.UpperLeftCorner.X += 5;
            PickBox.UpperLeftCorner.Y += 90;
            PickBox.LowerRightCorner.X += 85;
            PickBox.LowerRightCorner.Y += 120;
        }
		
        void CGUIColorPicker::draw()
        {
            Environment->getSkin()->draw3DSunkenPane(
                this, Background,
                false, true,
                AbsoluteRect,
                &AbsoluteClippingRect
            );
 
            IGUIElement::draw();
 
            Environment->getVideoDriver()->draw2DImage(GradientTexture, 
					core::position2d<s32>(AbsoluteRect.UpperLeftCorner.X+90, AbsoluteRect.UpperLeftCorner.Y+5));
 
            // 2 draw because the interpolation in the diagonal is not well rendered
            Environment->getVideoDriver()->draw2DRectangle(COL_BLACK, Box, &AbsoluteClippingRect);
            Environment->getVideoDriver()->draw2DRectangle(Box, COL_WHITE, SomeColor, Alpha, Alpha, &AbsoluteClippingRect);
 
            {
				using namespace core;
                const vector2di start(AbsoluteRect.UpperLeftCorner.X+90,  AbsoluteRect.UpperLeftCorner.Y+5+ColorPos);
                const vector2di end(AbsoluteRect.UpperLeftCorner.X+105,  AbsoluteRect.UpperLeftCorner.Y+5+ColorPos);
                const vector2di hstart(Box.UpperLeftCorner.X,  Box.UpperLeftCorner.Y+PickPos.Y);
                const vector2di hend(Box.LowerRightCorner.X, Box.UpperLeftCorner.Y+PickPos.Y);
                const vector2di vstart(Box.UpperLeftCorner.X+PickPos.X, Box.UpperLeftCorner.Y);
                const vector2di vend(Box.UpperLeftCorner.X+PickPos.X, Box.LowerRightCorner.Y);
 
                Environment->getVideoDriver()->draw2DLine(position2di( start.X,    start.Y-1), position2di( end.X,    end.Y-1), COL_WHITE);
                Environment->getVideoDriver()->draw2DLine(position2di( start.X,    start.Y+1), position2di( end.X,    end.Y+1), COL_WHITE);
                Environment->getVideoDriver()->draw2DLine(position2di(hstart.X,   hstart.Y-1), position2di(hend.X,   hend.Y-1), COL_WHITE);
                Environment->getVideoDriver()->draw2DLine(position2di(hstart.X,   hstart.Y+1), position2di(hend.X,   hend.Y+1), COL_WHITE);
                Environment->getVideoDriver()->draw2DLine(position2di(vstart.X-1,   vstart.Y), position2di(vend.X-1, vend.Y  ), COL_WHITE);
                Environment->getVideoDriver()->draw2DLine(position2di(vstart.X+1,   vstart.Y), position2di(vend.X+1, vend.Y  ), COL_WHITE);
 
                Environment->getVideoDriver()->draw2DLine(start,   end, COL_BLACK);
                Environment->getVideoDriver()->draw2DLine(hstart, hend, COL_BLACK);
                Environment->getVideoDriver()->draw2DLine(vstart, vend, COL_BLACK);
            }
 
            Environment->getVideoDriver()->draw2DImage(AlphaTexture, PickBox, PickBox);
            Environment->getVideoDriver()->draw2DRectangle( PickColor, PickBox, &AbsoluteClippingRect);
        }
    }
}

// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Just a quick test for the color picker
#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

struct SAppContext
{
	IrrlichtDevice * device;
	CGUIColorPicker* colorPicker;
	IGUIColorSelectDialog *colorSelect; // old Irrlicht dialog for comparison
};


class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			//s32 id = event.GUIEvent.Caller->getID();
			switch(event.GUIEvent.EventType)
			{
				case EGET_FILE_SELECTED:	// used by IGUIColorSelectDialog and CGUIColorPicker so far.
				{
					if ( event.GUIEvent.Caller == Context.colorSelect )
					{
						video::SColor picked = Context.colorSelect->getColor();
						std::cout << "red: " << picked.getRed() << " green: " << picked.getGreen() << " blue: " << picked.getBlue() << " alpha: " << picked.getAlpha() << "\n";
					}
					else if ( event.GUIEvent.Caller == Context.colorPicker )
					{
						video::SColor picked = Context.colorPicker->getPickedColor();
						std::cout << "red: " << picked.getRed() << " green: " << picked.getGreen() << " blue: " << picked.getBlue() << " alpha: " << picked.getAlpha() << "\n";
					}
				}
				break;
			default:
				break;
			}
		}

		return false;
	}

private:
	SAppContext & Context;
};


int main()
{
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));
	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();

	SAppContext context;
	context.device = device;
	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	context.colorPicker = new CGUIColorPicker(env, env->getRootGUIElement(), -1);	
	context.colorPicker->setRelativePosition(core::rect<s32>(10, 10, 200, 500));
	//context.colorSelect = env->addColorSelectDialog (L"irr color select", false);

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
