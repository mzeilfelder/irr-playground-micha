// Written by  chronologicaldot
// Some more stuff added by Michael Zeilfelder
// 
// All kind of tests tabcontrol. Especially serializing.
// See: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=52344

#include <irrlicht.h>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;
using namespace core;
using namespace gui;


class EventRec : public IEventReceiver {
    IrrlichtDevice*  device;
 
public:
    EventRec( IrrlichtDevice* d )
        : device(d), tabControl(0)
    {}
 
    virtual bool OnEvent(const SEvent&  event) {
        switch ( event.EventType ) {
        case EET_KEY_INPUT_EVENT:
            return OnKeyEvent(event.KeyInput);
        case EET_GUI_EVENT:
            return OnGUIEvent(event.GUIEvent);
        default:
            break;
        }
        return false;
    }
 
    bool OnKeyEvent(const SEvent::SKeyInput&  event) {

		if ( !event.PressedDown )
			return true;

        switch( event.Key ) {
        case KEY_KEY_B:
            removeAllChildren( device->getGUIEnvironment()->getRootGUIElement() );
            buildGUI();
            return true;
 
        case KEY_KEY_L:
            removeAllChildren( device->getGUIEnvironment()->getRootGUIElement() );
            loadGUI();
            return true;
 
        case KEY_KEY_S:
            saveGUI();
            return true;

		case KEY_KEY_A:
			if ( tabControl)
			{
				int idx = tabControl->getTabCount() ? (rand() % tabControl->getTabCount()) : 0;
				irr::core::stringw label(L"add");
				label += idx;
				tabControl->insertTab(idx, label.c_str());
			}
			return true;
		case KEY_KEY_R:
			if ( tabControl )
				tabControl->removeTab( tabControl->getActiveTab() );
			return true;
		case KEY_KEY_C:
			if ( tabControl )
				tabControl->clear();
			return true;
 
        case KEY_KEY_Q:
            device->closeDevice();
            return true;
 
        default: break;
        }
        return false;
    }
 
    bool OnGUIEvent(const SEvent::SGUIEvent&  event) {
        switch( event.EventType ) {
        case EGET_BUTTON_CLICKED:
            if ( event.Caller->getID() == 100 ) {
 
            }
            break;
 
        default:
            break;
        }
        return false;
    }
 
    void removeAllChildren( IGUIElement* e ) {
		tabControl = 0;
        list<IGUIElement*>::ConstIterator  kidIter = e->getChildren().begin();
        while ( e->getChildren().size() > 0 ) {
            e->removeChild(*kidIter);
            kidIter = e->getChildren().begin();
        }
    }
 
    void buildGUI() {
        IGUIEnvironment*  guiEnvironment = device->getGUIEnvironment();
        IGUITab*  tab = 0;
        recti  tabSize( 20,50,200,350 );
  
        // Generate
        recti  tabCtrlRect(100,50,400,550);
        tabControl = guiEnvironment->addTabControl(tabCtrlRect, guiEnvironment->getRootGUIElement());
        tabControl->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
        tabControl->updateAbsolutePosition();

		IGUITab* delaytab = guiEnvironment->addTab(tabSize, tabControl, -1);
		delaytab->setText(L"delayed");
		delaytab->setBackgroundColor(video::SColor(0xffaaff44));
        delaytab->setDrawBackground(true);

        tab = tabControl->addTab(L"DTab1", 10);
        tab->setBackgroundColor(video::SColor(0xffff2200));
        tab->setDrawBackground(true);
 
        tab = tabControl->addTab(L"DTab2", 11);
        tab->setBackgroundColor(video::SColor(0xff0000ff));
        tab->setDrawBackground(true);

        tab = guiEnvironment->addTab(tabSize, tabControl, -1);
        tab->setText(L"GreenTab");
        tab->setBackgroundColor(video::SColor(0xff00ff00));
        tab->setDrawBackground(true);
        //tab->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
        tabControl->addTab(tab);

		tabControl->addTab(delaytab);
		
        recti  buttonSize( 0,0,100,50 );
        IGUIButton*  button = guiEnvironment->addButton(buttonSize, tab, -1, L"Push");
        button->updateAbsolutePosition();
    }
 
    void loadGUI() {
        // Load from file
        device->getGUIEnvironment()->loadGUI("saved_gui.xml");
    }
 
    void saveGUI() {
        // Save to file
        device->getGUIEnvironment()->saveGUI("saved_gui.xml");
    }

	IGUITabControl*  tabControl;
};
 
int main() {
 
    dimension2du  screenSize(900,600);
    IrrlichtDevice*  device = createDevice(video::EDT_OPENGL, screenSize);
 
    if ( !device )
        return 1;

	device->setWindowCaption(L"(b)uild (s)ave (l)oad (a)dd (r)emove (c)lear");
 
    video::IVideoDriver*  videoDriver = device->getVideoDriver();
    IGUIEnvironment*  guiEnvironment = device->getGUIEnvironment();
    EventRec  eventRec(device);
 
    device->setEventReceiver(&eventRec);
 
    while ( device->run() ) {
        videoDriver->beginScene();
        guiEnvironment->drawAll();
        videoDriver->endScene();
    }
 
    device->drop();
    return 0;
}
