// Copyright (C) <2014> <Jehan-antoine vayssade>
// Ovan/Magun contact on irrlicht-fr.org or ovan@sleek-think.ovh
// Code is under the zlib license (same as Irrlicht)
// For conditions of distribution and use, see copyright notice in irrlicht.h
// File was altered by Michael Zeilfelder

#ifndef __C_GUI_COLOR_PICKER_HEADER__
#define __C_GUI_COLOR_PICKER_HEADER__
 
#include <IGUIElement.h>
 
namespace irr
{
    namespace gui
    {
        class IGUIButton;
        class IGUIStaticText;
        class IGUIScrollBar;
 
        class CGUIColorPicker : public IGUIElement
        {
            public:
                CGUIColorPicker(IGUIEnvironment *environment, IGUIElement *parent, s32 id = 0);
                virtual ~CGUIColorPicker();
 
				// TODO: documentation
                virtual bool OnEvent(const SEvent&) _IRR_OVERRIDE_;	
			
                void setRelativePosition(const core::rect<s32> &r);	// TODO: this is _not_ a virtual function! Change in Irrlicht or here?
                virtual void updateAbsolutePosition() _IRR_OVERRIDE_;
			
				virtual void draw() _IRR_OVERRIDE_;			
 
                virtual void setPickedColor(video::SColor color);
                virtual video::SColor getPickedColor() const;
 
			
				// TODO: also needs setDrawBackground flag like other elements and should use skin-value by default.
                virtual void setBackgroundColor(video::SColor color);
                virtual video::SColor getBackgroundColor() const;
 
                		
            protected:
                virtual void recalculatePickedColor();
                virtual void createAlphaTexture();
                virtual void createGradientTexture();

				// TODO: members start upper-case in Irrlicht
                IGUIButton      *Close;
                IGUIScrollBar   *Scroll;
				video::ITexture *AlphaTexture;
				video::ITexture *GradientTexture;
                bool IsGradient, IsColor, IsInside;				
                video::SColor    PickColor;
				video::SColor    SomeColor;	// TODO: which one is that? hsv?
                video::SColor    Background, Alpha;
                core::recti      Box, PickBox, Gradient;	// TODO: which box is a box?
                core::vector2di  PickPos;
                int              ColorPos;
        };
    }
}
 
#endif
