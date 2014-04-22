// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef I_GUI_PROFILE_H_INCLUDED__
#define I_GUI_PROFILE_H_INCLUDED__

#include "IGUIElement.h"

// TODO: just a dummy until I add it to the real enum table
#define EGUIET_PROFILE 0

namespace irr
{
namespace gui
{
	//! Element to display profiler information
	class IGUIProfile : public IGUIElement
	{
	public:
		//! constructor
		IGUIProfile(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement((EGUI_ELEMENT_TYPE)EGUIET_PROFILE, environment, parent, id, rectangle)
			, DisplayList(0), CurrentGroupIdx(0)
		{
			// I had no table yet when programming this. Would probably be nicer.
			irr::core::recti r(0, 0, rectangle.getWidth(), rectangle.getHeight());
			DisplayList = Environment->addListBox(r, this, -1, true);
		}

		virtual void draw()
		{
			if ( isVisible() && DisplayList )
			{
				DisplayList->clear();

				irr::core::stringw wstrTitle(irr::SProfileData::makeTitleString());
				DisplayList->addItem(wstrTitle.c_str());

				irr::core::stringw wstrGroup(irr::getProfiler()->getGroupData(CurrentGroupIdx).getAsString());
				DisplayList->addItem(wstrGroup.c_str());

				// show overview over groups?
				if ( CurrentGroupIdx == 0 )
				{
					for ( irr::u32 i=1; i<irr::getProfiler()->getGroupCount(); ++i )
					{
						irr::core::stringw wstrData(irr::getProfiler()->getGroupData(i).getAsString());
						DisplayList->addItem(wstrData.c_str());
					}
				}
				// show data for current group
				else
				{
					const irr::SProfileData& data = irr::getProfiler()->getGroupData(CurrentGroupIdx);
					irr::core::stringw wstrData(data.getAsString());
					DisplayList->addItem(wstrData.c_str());
				}
			}

			IGUIElement::draw();
		}

		void nextPage()
		{
			if ( ++CurrentGroupIdx >= irr::getProfiler()->getGroupCount() )
				CurrentGroupIdx = 0;
		}

		void previousPage()
		{
			if ( CurrentGroupIdx > 0 )
				--CurrentGroupIdx;
			else
				CurrentGroupIdx = irr::getProfiler()->getGroupCount()-1;
		}

		void firstDisplayGroup()
		{
			CurrentGroupIdx = 0;
		}

	protected:
		irr::gui::IGUIListBox* DisplayList;
		irr::u32 CurrentGroupIdx;
	};

} // end namespace gui
} // end namespace irr

#endif
