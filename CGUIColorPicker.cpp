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

namespace irr
{
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
			, close(0), scroll(0), isGradient(false), isColor(false), isInside(false)
			, background(video::SColor(64, 255, 255, 255))
			, white(video::SColor(255, 255, 255, 255))
			, black(video::SColor(255, 0, 0, 0))
			, colorpos(0)
        {
            close = Environment->addButton(core::rect<s32>(5, 140, 85, 156), this, 0, L"take this color");
 
            scroll = Environment->addScrollBar(true, core::rect<s32>(5, 125, 85, 135), this);
            scroll->setMin(0);
            scroll->setMax(255);
            scroll->setPos(255);
 
            createAlphaTexture();
            createGradientTexture();
 
            setPickedColor(video::SColor(255, 64, 64, 128));
            updateAbsolutePosition();
        }
		
        CGUIColorPicker::~CGUIColorPicker() 
        {
            close->drop();
            scroll->drop();
            img[0]->drop();
            img[1]->drop();
        }
		
        void CGUIColorPicker::createAlphaTexture() 
        {
            img[0] = Environment->getVideoDriver()->addTexture(core::dimension2d<u32>(16, 16), "alpha", video::ECF_A8R8G8B8);
            u32 *tmp = (u32*) img[0]->lock();
 
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
 
            img[0]->unlock();
        }
		
        void CGUIColorPicker::createGradientTexture() 
        {
			// TODO: can't use non pot textures
            img[1] = Environment->getVideoDriver()->addTexture(core::dimension2d<u32>(15, 151), "gradient", video::ECF_A8R8G8B8);
            u32 *tmp = (u32*) img[1]->lock();
 
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
 
            img[1]->unlock();
        }
		
        void CGUIColorPicker::setRelativePosition(const core::recti &r)
        {
            RelativeRect.UpperLeftCorner = r.UpperLeftCorner;
            RelativeRect.LowerRightCorner.X = r.UpperLeftCorner.X + 110;
            RelativeRect.LowerRightCorner.Y = r.UpperLeftCorner.X + 160;
        }
		
        bool CGUIColorPicker::OnEvent(const SEvent &event) 
        {
            if(event.EventType == EET_MOUSE_INPUT_EVENT)
            {
                core::vector2di pos(event.MouseInput.X, event.MouseInput.Y);
 
                if(event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                {
                    isGradient = gradient.isPointInside(pos);
                    isColor = box.isPointInside(pos);
                    isInside = AbsoluteRect.isPointInside(pos);
                }
 
                if(event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
                {
                    // does not work since 1.7 (i think)
                    // probably need CGUIModalScreen if this module is add to the engine
                    //if(!AbsoluteRect.isPointInside(pos) && !isInside)
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
                    isGradient = isColor = false;
                }
 
                if(isGradient)
                {
                    if(pos.X < gradient.UpperLeftCorner.X)
                        pos.X = gradient.UpperLeftCorner.X;
                    if(pos.Y < gradient.UpperLeftCorner.Y)
                        pos.Y = gradient.UpperLeftCorner.Y;
 
                    if(pos.X > gradient.LowerRightCorner.X)
                        pos.X = gradient.LowerRightCorner.X;
                    if(pos.Y > gradient.LowerRightCorner.Y)
                        pos.Y = gradient.LowerRightCorner.Y;
 
                    colorpos = pos.Y - gradient.UpperLeftCorner.Y;
                    u32 *tmp =(u32*)img[1]->lock();
                    color.set(tmp[colorpos*img[1]->getOriginalSize().Width]);
                    img[1]->unlock();
                    recalculatePickedColor();
                }
 
                if(isColor)
                {
                    if(pos.X < box.UpperLeftCorner.X)
                        pos.X = box.UpperLeftCorner.X;
                    if(pos.Y < box.UpperLeftCorner.Y)
                        pos.Y = box.UpperLeftCorner.Y;
 
                    if(pos.X > box.LowerRightCorner.X)
                        pos.X = box.LowerRightCorner.X;
                    if(pos.Y > box.LowerRightCorner.Y)
                        pos.Y = box.LowerRightCorner.Y;
 
                    pickpos.X = pos.X - box.UpperLeftCorner.X;
                    pickpos.Y = pos.Y - box.UpperLeftCorner.Y;
                    recalculatePickedColor();
                }
 
                if(isGradient || isColor)
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
            video::SColor hcolor = color.getInterpolated(white, pickpos.X/80.f);
 
            pickcolor = black.getInterpolated(hcolor, pickpos.Y/80.f);
            pickcolor.setAlpha(scroll->getPos());
 
            alpha = color;
            alpha.setAlpha(0);
        }
        void CGUIColorPicker::setPickedColor(const video::SColor &c) 
        {
            pickcolor = c;
 
            core::vector3df hsv = RGBftoHSV({
                c.getRed()/255.f,
                c.getGreen()/255.f,
                c.getBlue()/255.f,
                c.getAlpha()/255.f
            });
 
            colorpos = 150-hsv.X/360.f*150.f;
            pickpos.X = hsv.Y*80.f;
            pickpos.Y = 80-(hsv.Z)*80.f;
 
            u32 *tmp =(u32*)img[1]->lock();
            color.set(tmp[colorpos*img[1]->getOriginalSize().Width]);
            img[1]->unlock();
 
            alpha = color;
            alpha.setAlpha(0);
        }
        const video::SColor& CGUIColorPicker::getPickedColor() const 
        {
            return pickcolor;
        }
        void CGUIColorPicker::setBackgroundColor(const video::SColor &b) 
        {
            background = b;
        }
        const video::SColor& CGUIColorPicker::getBackgroundColor() const 
        {
            return background;
        }
        void CGUIColorPicker::updateAbsolutePosition()
        {
            IGUIElement::updateAbsolutePosition();
 
            box.UpperLeftCorner = AbsoluteRect.UpperLeftCorner;
            box.LowerRightCorner = AbsoluteRect.UpperLeftCorner;
            box.UpperLeftCorner.X += 5;
            box.UpperLeftCorner.Y += 5;
            box.LowerRightCorner.X += 85;
            box.LowerRightCorner.Y += 85;
 
            gradient.UpperLeftCorner = AbsoluteRect.UpperLeftCorner;
            gradient.LowerRightCorner = AbsoluteRect.UpperLeftCorner;
            gradient.UpperLeftCorner.X += 90;
            gradient.UpperLeftCorner.Y += 5;
            gradient.LowerRightCorner.X += 105;
            gradient.LowerRightCorner.Y += 155;
 
            pickbox.UpperLeftCorner = AbsoluteRect.UpperLeftCorner;
            pickbox.LowerRightCorner = AbsoluteRect.UpperLeftCorner;
            pickbox.UpperLeftCorner.X += 5;
            pickbox.UpperLeftCorner.Y += 90;
            pickbox.LowerRightCorner.X += 85;
            pickbox.LowerRightCorner.Y += 120;
        }
        void CGUIColorPicker::draw()
        {
            Environment->getSkin()->draw3DSunkenPane(
                this, background,
                false, true,
                AbsoluteRect,
                &AbsoluteClippingRect
            );
 
            IGUIElement::draw();
 
            Environment->getVideoDriver()->draw2DImage(img[1], {
                AbsoluteRect.UpperLeftCorner.X+90,
                AbsoluteRect.UpperLeftCorner.Y+5
            });
 
            // 2 draw because the interpolation in the diagonal is not well rendered
            Environment->getVideoDriver()->draw2DRectangle(black, box, &AbsoluteClippingRect);
            Environment->getVideoDriver()->draw2DRectangle(box, white, color, alpha, alpha, &AbsoluteClippingRect);
 
            {
				using namespace core;
                const vector2di start(AbsoluteRect.UpperLeftCorner.X+90,  AbsoluteRect.UpperLeftCorner.Y+5+colorpos);
                const vector2di end(AbsoluteRect.UpperLeftCorner.X+105,  AbsoluteRect.UpperLeftCorner.Y+5+colorpos);
                const vector2di hstart(box.UpperLeftCorner.X,  box.UpperLeftCorner.Y+pickpos.Y);
                const vector2di hend(box.LowerRightCorner.X, box.UpperLeftCorner.Y+pickpos.Y);
                const vector2di vstart(box.UpperLeftCorner.X+pickpos.X, box.UpperLeftCorner.Y);
                const vector2di vend(box.UpperLeftCorner.X+pickpos.X, box.LowerRightCorner.Y);
 
                Environment->getVideoDriver()->draw2DLine(position2di( start.X,    start.Y-1), position2di( end.X,    end.Y-1), white);
                Environment->getVideoDriver()->draw2DLine(position2di( start.X,    start.Y+1), position2di( end.X,    end.Y+1), white);
                Environment->getVideoDriver()->draw2DLine(position2di(hstart.X,   hstart.Y-1), position2di(hend.X,   hend.Y-1), white);
                Environment->getVideoDriver()->draw2DLine(position2di(hstart.X,   hstart.Y+1), position2di(hend.X,   hend.Y+1), white);
                Environment->getVideoDriver()->draw2DLine(position2di(vstart.X-1,   vstart.Y), position2di(vend.X-1, vend.Y  ), white);
                Environment->getVideoDriver()->draw2DLine(position2di(vstart.X+1,   vstart.Y), position2di(vend.X+1, vend.Y  ), white);
 
                Environment->getVideoDriver()->draw2DLine(start,   end, black);
                Environment->getVideoDriver()->draw2DLine(hstart, hend, black);
                Environment->getVideoDriver()->draw2DLine(vstart, vend, black);
            }
 
            Environment->getVideoDriver()->draw2DImage(img[0], pickbox, pickbox);
            Environment->getVideoDriver()->draw2DRectangle( pickcolor, pickbox, &AbsoluteClippingRect);
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
	context.colorSelect = env->addColorSelectDialog (L"irr color select", false);

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
