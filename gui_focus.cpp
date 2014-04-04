// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
//
// Testing focus behavior

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

enum
{
	GUI_ID_STATIC_FOCUSED,
	GUI_ID_CHECKBOX_RANDOM_DISABLING,
	GUI_ID_CHECKBOX_ON_LMOUSE_DOWN,
	GUI_ID_CHECKBOX_ON_RMOUSE_DOWN,
	GUI_ID_CHECKBOX_ON_MOUSE_OVER,
	GUI_ID_CHECKBOX_ON_TAB,
	GUI_ID_CHECKBOX_CAN_FOCUS_DISABLED
};

struct SAppContext
{
	IrrlichtDevice * device;
	IGUIElement* mGuiParent;
	array<IGUIElement*> mGuiElements;
};


void ClearAllTestGuiElements(SAppContext & context)
{
	const core::list<IGUIElement*>& children = context.mGuiParent->getChildren();
	while (!children.empty())
		(*children.getLast())->remove();
	context.mGuiElements.clear();
}

void SetTabStopsForAllElements(SAppContext & context)
{
    for ( u32 i=0; i < context.mGuiElements.size(); ++i )
    {
        context.mGuiElements[i]->setTabStop(true);
        context.mGuiElements[i]->setTabOrder((s32)i);
    }
}

void EnableAllElements(SAppContext & context)
{
    for ( u32 i=0; i < context.mGuiElements.size(); ++i )
    {
        context.mGuiElements[i]->setEnabled(true);
    }
}

void RandomDisableElements(SAppContext & context)
{
    for ( u32 i=0; i < context.mGuiElements.size(); ++i )
    {
		if ( rand() % 2 == 0 )
			context.mGuiElements[i]->setEnabled(false);
    }
}

void AddTestGuiElements(IGUIEnvironment* env, IGUIElement * parent, SAppContext & context)
{
	context.mGuiElements.push_back( env->addToolBar (parent, -1) );

	s32 top = 40;
	s32 default_height = 15;
	s32 default_width = 150;
	s32 default_gap = 20;
	core::rect<s32> rect(10, top, 10 + default_width, top + default_height);

	IGUIButton * button = env->addButton (rect, parent, -1, L"button", L"tooltip");
	context.mGuiElements.push_back(button);

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	IGUICheckBox * checkbox = env->addCheckBox (true, rect, parent, -1, L"checkbox");
	checkbox->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(checkbox);

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	IGUIComboBox * combo = env->addComboBox (rect, parent, -1);
	for ( int i = 0; i < 100; ++i)
	{
		core::stringw wstr(i);
		wstr += L"Alpha";
		combo->addItem(wstr.c_str());
		wstr += L"-Beta";
		combo->addItem(wstr.c_str());
	}
	combo->setToolTipText(L"tooltip");
	context.mGuiElements.push_back( combo );

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 2* default_height;
	IGUIEditBox * multiLineEditBox = env->addEditBox (L"editbox", rect, true, parent, -1);
	multiLineEditBox->setMultiLine(true);
	multiLineEditBox->setWordWrap(true);
	multiLineEditBox->setToolTipText(L"tooltip");
	context.mGuiElements.push_back( multiLineEditBox );

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	IGUIEditBox * editboxnotext = env->addEditBox (L"", rect, true, parent, -1);
	editboxnotext->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(editboxnotext);


	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height*3;
	IGUIListBox * lb = env->addListBox (rect, parent, -1, true);
	for ( int i = 0; i < 100; ++i)
	{
		core::stringw wstr(i);
		wstr += L"Alpha";
		lb->addItem(wstr.c_str());
		wstr += L"-Beta";
		lb->addItem(wstr.c_str());
	}
	lb->setToolTipText(L"tooltip");
	context.mGuiElements.push_back( lb );

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	IGUIScrollBar * scrollbar = env->addScrollBar (true, rect, parent, -1);
	scrollbar->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(scrollbar);

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	IGUISpinBox * spinbox = env->addSpinBox (L"0", rect, true, parent, -1);
	spinbox->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(spinbox);

	rect.UpperLeftCorner.X = 10 + default_width + default_gap;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + default_width;
	rect.UpperLeftCorner.Y = top;
	rect.LowerRightCorner.Y = top + default_height;
	IGUIStaticText * statictext = env->addStaticText (L"static", rect, false, true, parent, -1, true);
	statictext->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(statictext);

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height;
	IGUIStaticText * statictextnotext = env->addStaticText (L"", rect, true, true, parent, -1, true);
	statictextnotext->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(statictextnotext);	// no text

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 4*default_height;
	IGUITabControl * tabctrl = env->addTabControl (rect, parent, true, true, -1);
	tabctrl->addTab(L"tab1", -1);
	tabctrl->addTab(L"tab2", -1);
	tabctrl->addTab(L"tab3", -1);
	tabctrl->addTab(L"tab4", -1);
	tabctrl->addTab(L"tab5", -1);
	tabctrl->setToolTipText(L"tooltip");
	context.mGuiElements.push_back( tabctrl );

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + default_height*5;
	IGUITable * table = env->addTable (rect, parent, -1, true);
	table->addColumn(L"col1");
	table->addColumn(L"col2");
	table->addColumn(L"col3");
	for ( int i=0; i<10; ++i )
		table->addRow(i);
	table->setCellText(1, 1, L"text");
	table->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(table );

	rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 4*default_height;
	IGUIWindow * window = env->addWindow (rect, false, L"window long titletext for colortesting", parent, -1);
	window->setToolTipText(L"tooltip");
	context.mGuiElements.push_back(window);

    rect.UpperLeftCorner.Y = rect.LowerRightCorner.Y + default_gap;
    rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 4*default_height;
    bool treeViewBackground = false;
    bool treeScrollBarVertical=true;
    bool treeScrollBarHorizontal=true;
    IGUITreeView * tree = env->addTreeView(rect, parent, -1, treeViewBackground, treeScrollBarVertical, treeScrollBarHorizontal);
    IGUITreeViewNode * treeNode = tree->getRoot();
    for ( int i=0; i < 5; ++i )
        treeNode->addChildBack(L"child");
	IGUITreeViewNode * branchNode = treeNode;
    for ( int i=0; i < 5; ++i )
    {
        branchNode = branchNode->addChildBack(L"branch");
        branchNode->setExpanded(true);
    }
	treeNode->addChildBack(L"lastchild");
    context.mGuiElements.push_back(tree);

    IGUIContextMenu * contextMenu =	env->addMenu(parent);
    contextMenu->addItem(L"File", -1, false, true);
    contextMenu->getSubMenu (0)->addItem(L"One", -1, false, true);
    contextMenu->getSubMenu (0)->addItem(L"Two", -1, true, true);
	contextMenu->addItem(L"Options", -1, true, true);
	contextMenu->getSubMenu (1)->addItem(L"One", -1, false, true);
    contextMenu->getSubMenu (1)->addItem(L"Two", -1, true, true);
	contextMenu->addItem(L"Help", -1, true, true);
	contextMenu->getSubMenu (2)->addItem(L"One", -1, false, true);
    contextMenu->getSubMenu (2)->addItem(L"Two", -1, true, true);
	context.mGuiElements.push_back(contextMenu);
}

void AddControlElements(IGUIEnvironment* env, IGUIElement * parent)
{
	env->addStaticText(L"test", rect<s32>(510, 30, 620, 55), true, false, parent, GUI_ID_STATIC_FOCUSED, true);
	env->addCheckBox (false, rect<s32>(510, 60, 620, 80), parent, GUI_ID_CHECKBOX_RANDOM_DISABLING, L"random disabling");
	env->addCheckBox (true, rect<s32>(510, 100, 620, 120), parent, GUI_ID_CHECKBOX_ON_LMOUSE_DOWN, L"focus mouse L");
	env->addCheckBox (false, rect<s32>(510, 130, 620, 150), parent, GUI_ID_CHECKBOX_ON_RMOUSE_DOWN, L"focus mouse R");
	env->addCheckBox (false, rect<s32>(510, 160, 620, 180), parent, GUI_ID_CHECKBOX_ON_MOUSE_OVER, L"focus mouse over");
	env->addCheckBox (true, rect<s32>(510, 190, 620, 210), parent, GUI_ID_CHECKBOX_ON_TAB, L"focus on tab");
	env->addCheckBox (false, rect<s32>(510, 220, 620, 240), parent, GUI_ID_CHECKBOX_CAN_FOCUS_DISABLED, L"focus disabled elements");
}

// move the children from source to target
void MoveChildren(irr::gui::IGUIElement* target, irr::gui::IGUIElement* source)
{
	core::list<IGUIElement*> children = source->getChildren();	// note - no reference, we need a copy here
	for ( core::list<IGUIElement*>::Iterator it = children.begin(); it != children.end();  ++it )
	{
		target->addChild(*it);
	}
}

void SaveTextGuiElements(SAppContext & context)
{
	// That line would save the complete gui (for testing to compare results)
	// context.device->getGUIEnvironment()->saveGUI("gui_all_elements.xml");

	context.device->getGUIEnvironment()->saveGUI("gui_all_elements.xml", context.mGuiParent);
}

void LoadTextGuiElements(SAppContext & context)
{
	context.device->getGUIEnvironment()->loadGUI("gui_all_elements.xml", context.mGuiParent);
	if ( !context.mGuiParent->getChildren().empty() )
	{
		// Workaround because I can't load _into_ an element so far in Irrlicht. So I have now one more copy of
		// context.mGuiParent below context.mGuiParent which I have to remove again.
		// Irrlicht might change that on loading some day with another paramter (can't easily change it on saving
		// without changing the format as xml-files need a single root).
		// First element is our dummy, remove that from the hierarchy.
		IGUIElement * dummy = *(context.mGuiParent->getChildren().begin());
		MoveChildren(context.mGuiParent, dummy);
		dummy->remove();
	}
}

class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			switch(event.GUIEvent.EventType)
			{
				case EGET_CHECKBOX_CHANGED:
				{
					IGUICheckBox *cb = static_cast<IGUICheckBox *>(event.GUIEvent.Caller);
					switch ( id )
					{
						case GUI_ID_CHECKBOX_RANDOM_DISABLING:
							if ( cb->isChecked() )
							{
								RandomDisableElements(Context);
							}
							else
							{
								EnableAllElements(Context);
							}
						break;
						case GUI_ID_CHECKBOX_ON_LMOUSE_DOWN:
							if ( cb->isChecked() )
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() | EFF_SET_ON_LMOUSE_DOWN );
							else
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() & ~EFF_SET_ON_LMOUSE_DOWN );
						break;
						case GUI_ID_CHECKBOX_ON_RMOUSE_DOWN:
							if ( cb->isChecked() )
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() | EFF_SET_ON_RMOUSE_DOWN );
							else
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() & ~EFF_SET_ON_RMOUSE_DOWN );
						break;
						case GUI_ID_CHECKBOX_ON_MOUSE_OVER:
							if ( cb->isChecked() )
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() | EFF_SET_ON_MOUSE_OVER );
							else
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() & ~EFF_SET_ON_MOUSE_OVER );
						break;
						case GUI_ID_CHECKBOX_ON_TAB:
							if ( cb->isChecked() )
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() | EFF_SET_ON_TAB );
							else
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() & ~EFF_SET_ON_TAB );
						break;
						case GUI_ID_CHECKBOX_CAN_FOCUS_DISABLED:
							if ( cb->isChecked() )
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() | EFF_CAN_FOCUS_DISABLED );
							else
								Context.device->getGUIEnvironment()->setFocusBehavior( Context.device->getGUIEnvironment()->getFocusBehavior() & ~EFF_CAN_FOCUS_DISABLED );
						break;

						default:
						break;
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
	context.mGuiParent = env->addWindow(irr::core::recti(0,0,500,450), false, L"Parent window");

	AddTestGuiElements(env, context.mGuiParent, context);
	SetTabStopsForAllElements(context);

	AddControlElements(env, 0);

	MyEventReceiver receiver(context);
	device->setEventReceiver(&receiver);

	IGUIStaticText * infoFocused = static_cast<IGUIStaticText*>(env->getRootGUIElement()->getElementFromId(GUI_ID_STATIC_FOCUSED, true));

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, SColor(0,200,200,200));

			if ( infoFocused )
			{
				core::stringw typeFocused;
				IGUIElement * focused = env->getFocus();
				if ( focused )
				{
					typeFocused = core::stringw(focused->getTypeName());
				}
				infoFocused->setText(typeFocused.c_str());
			}

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
