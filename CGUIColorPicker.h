// Copyright (C) <2014> <Jehan-antoine vayssade>
// Ovan/Magun contact on irrlicht-fr.org or ovan@sleek-think.ovh
// Code is under the zlib license (same as Irrlicht)
// For conditions of distribution and use, see copyright notice in irrlicht.h
// File was altered by Michael Zeilfelder

#ifndef __C_GUI_COLOR_PICKER_HEADER__
#define __C_GUI_COLOR_PICKER_HEADER__
 
#include <IGUIElement.h>
#include <S3DVertex.h>
 
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
 
                virtual void setPickedColor(const video::SColor&);
				// TODO probably better use copy instead reference here as dialog might get remove()'d on event (which is currently outcommented as not working - maybe that's the reason?)
                virtual const video::SColor& getPickedColor() const;
 
                virtual void setBackgroundColor(const video::SColor&);
                virtual const video::SColor& getBackgroundColor() const;
 
                		
            protected:
                virtual void recalculatePickedColor();
                virtual void createAlphaTexture();
                virtual void createGradientTexture();

				// TODO: members start upper-case in Irrlicht
                IGUIButton      *close;
                IGUIScrollBar   *scroll;
                video::ITexture *img[2];
                bool isGradient, isColor, isInside;				
                video::SColor    pickcolor, color;
                video::SColor    background, white, black, alpha;
                core::recti      box, pickbox, gradient;
                core::vector2di  pickpos;
                int              colorpos;
        };
    }
}
 
#endif
