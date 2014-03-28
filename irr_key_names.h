#ifndef IRR_KEY_NAMES_H
#define IRR_KEY_NAMES_H

#include "Keycodes.h"

class IrrKeyName
{
	public:
	const char* get(irr::EKEY_CODE code) const
	{
		switch ( code )
		{
			case irr::KEY_LBUTTON      : return "Left mouse button";
			case irr::KEY_RBUTTON      : return "Right mouse button";
			case irr::KEY_CANCEL       : return "Control-break processing";
			case irr::KEY_MBUTTON      : return "Middle mouse button (three-button mouse)";
			case irr::KEY_XBUTTON1     : return "Windows 2000/XP: X1 mouse button";
			case irr::KEY_XBUTTON2     : return "Windows 2000/XP: X2 mouse button";
			case irr::KEY_BACK         : return "BACKSPACE key";
			case irr::KEY_TAB          : return "TAB key";
			case irr::KEY_CLEAR        : return "CLEAR key";
			case irr::KEY_RETURN       : return "ENTER key";
			case irr::KEY_SHIFT        : return "SHIFT key";
			case irr::KEY_CONTROL      : return "CTRL key";
			case irr::KEY_MENU         : return "ALT key";
			case irr::KEY_PAUSE        : return "PAUSE key";
			case irr::KEY_CAPITAL      : return "CAPS LOCK key";
			case irr::KEY_KANA         : return "IME Kana or Hanguel mode";
			case irr::KEY_JUNJA        : return "IME Junja mode";
			case irr::KEY_FINAL        : return "IME final mode";
			case irr::KEY_HANJA        : return "IME Hanja or Kanji mode";
			case irr::KEY_ESCAPE       : return "ESC key";
			case irr::KEY_CONVERT      : return "IME convert";
			case irr::KEY_NONCONVERT   : return "IME nonconvert";
			case irr::KEY_ACCEPT       : return "IME accept";
			case irr::KEY_MODECHANGE   : return "IME mode change request";
			case irr::KEY_SPACE        : return "SPACEBAR";
			case irr::KEY_PRIOR        : return "PAGE UP key";
			case irr::KEY_NEXT         : return "PAGE DOWN key";
			case irr::KEY_END          : return "END key";
			case irr::KEY_HOME         : return "HOME key";
			case irr::KEY_LEFT         : return "LEFT ARROW key";
			case irr::KEY_UP           : return "UP ARROW key";
			case irr::KEY_RIGHT        : return "RIGHT ARROW key";
			case irr::KEY_DOWN         : return "DOWN ARROW key";
			case irr::KEY_SELECT       : return "SELECT key";
			case irr::KEY_PRINT        : return "PRINT key";
			case irr::KEY_EXECUT       : return "EXECUTE key";
			case irr::KEY_SNAPSHOT     : return "PRINT SCREEN key";
			case irr::KEY_INSERT       : return "INS key";
			case irr::KEY_DELETE       : return "DEL key";
			case irr::KEY_HELP         : return "HELP key";
			case irr::KEY_KEY_0        : return "0 key";
			case irr::KEY_KEY_1        : return "1 key";
			case irr::KEY_KEY_2        : return "2 key";
			case irr::KEY_KEY_3        : return "3 key";
			case irr::KEY_KEY_4        : return "4 key";
			case irr::KEY_KEY_5        : return "5 key";
			case irr::KEY_KEY_6        : return "6 key";
			case irr::KEY_KEY_7        : return "7 key";
			case irr::KEY_KEY_8        : return "8 key";
			case irr::KEY_KEY_9        : return "9 key";
			case irr::KEY_KEY_A        : return "A key";
			case irr::KEY_KEY_B        : return "B key";
			case irr::KEY_KEY_C        : return "C key";
			case irr::KEY_KEY_D        : return "D key";
			case irr::KEY_KEY_E        : return "E key";
			case irr::KEY_KEY_F        : return "F key";
			case irr::KEY_KEY_G        : return "G key";
			case irr::KEY_KEY_H        : return "H key";
			case irr::KEY_KEY_I        : return "I key";
			case irr::KEY_KEY_J        : return "J key";
			case irr::KEY_KEY_K        : return "K key";
			case irr::KEY_KEY_L        : return "L key";
			case irr::KEY_KEY_M        : return "M key";
			case irr::KEY_KEY_N        : return "N key";
			case irr::KEY_KEY_O        : return "O key";
			case irr::KEY_KEY_P        : return "P key";
			case irr::KEY_KEY_Q        : return "Q key";
			case irr::KEY_KEY_R        : return "R key";
			case irr::KEY_KEY_S        : return "S key";
			case irr::KEY_KEY_T        : return "T key";
			case irr::KEY_KEY_U        : return "U key";
			case irr::KEY_KEY_V        : return "V key";
			case irr::KEY_KEY_W        : return "W key";
			case irr::KEY_KEY_X        : return "X key";
			case irr::KEY_KEY_Y        : return "Y key";
			case irr::KEY_KEY_Z        : return "Z key";
			case irr::KEY_LWIN         : return "Left Windows key (Natural keyboard)";
			case irr::KEY_RWIN         : return "Right Windows key (Natural keyboard)";
			case irr::KEY_APPS         : return "Applications key (Natural keyboard)";
			case irr::KEY_SLEEP        : return "Computer Sleep key";
			case irr::KEY_NUMPAD0      : return "Numeric keypad 0 key";
			case irr::KEY_NUMPAD1      : return "Numeric keypad 1 key";
			case irr::KEY_NUMPAD2      : return "Numeric keypad 2 key";
			case irr::KEY_NUMPAD3      : return "Numeric keypad 3 key";
			case irr::KEY_NUMPAD4      : return "Numeric keypad 4 key";
			case irr::KEY_NUMPAD5      : return "Numeric keypad 5 key";
			case irr::KEY_NUMPAD6      : return "Numeric keypad 6 key";
			case irr::KEY_NUMPAD7      : return "Numeric keypad 7 key";
			case irr::KEY_NUMPAD8      : return "Numeric keypad 8 key";
			case irr::KEY_NUMPAD9      : return "Numeric keypad 9 key";
			case irr::KEY_MULTIPLY     : return "Multiply key";
			case irr::KEY_ADD          : return "Add key";
			case irr::KEY_SEPARATOR    : return "Separator key";
			case irr::KEY_SUBTRACT     : return "Subtract key";
			case irr::KEY_DECIMAL      : return "Decimal key";
			case irr::KEY_DIVIDE       : return "Divide key";
			case irr::KEY_F1           : return "F1 key";
			case irr::KEY_F2           : return "F2 key";
			case irr::KEY_F3           : return "F3 key";
			case irr::KEY_F4           : return "F4 key";
			case irr::KEY_F5           : return "F5 key";
			case irr::KEY_F6           : return "F6 key";
			case irr::KEY_F7           : return "F7 key";
			case irr::KEY_F8           : return "F8 key";
			case irr::KEY_F9           : return "F9 key";
			case irr::KEY_F10          : return "F10 key";
			case irr::KEY_F11          : return "F11 key";
			case irr::KEY_F12          : return "F12 key";
			case irr::KEY_F13          : return "F13 key";
			case irr::KEY_F14          : return "F14 key";
			case irr::KEY_F15          : return "F15 key";
			case irr::KEY_F16          : return "F16 key";
			case irr::KEY_F17          : return "F17 key";
			case irr::KEY_F18          : return "F18 key";
			case irr::KEY_F19          : return "F19 key";
			case irr::KEY_F20          : return "F20 key";
			case irr::KEY_F21          : return "F21 key";
			case irr::KEY_F22          : return "F22 key";
			case irr::KEY_F23          : return "F23 key";
			case irr::KEY_F24          : return "F24 key";
			case irr::KEY_NUMLOCK      : return "NUM LOCK key";
			case irr::KEY_SCROLL       : return "SCROLL LOCK key";
			case irr::KEY_LSHIFT       : return "Left SHIFT key";
			case irr::KEY_RSHIFT       : return "Right SHIFT key";
			case irr::KEY_LCONTROL     : return "Left CONTROL key";
			case irr::KEY_RCONTROL     : return "Right CONTROL key";
			case irr::KEY_LMENU        : return "Left MENU key";
			case irr::KEY_RMENU        : return "Right MENU key";
			case irr::KEY_OEM_1        : return "for US ;:";
			case irr::KEY_PLUS         : return "Plus Key +";
			case irr::KEY_COMMA        : return "Comma Key  ,";
			case irr::KEY_MINUS        : return "Minus Key  -";
			case irr::KEY_PERIOD       : return "Period Key .";
			case irr::KEY_OEM_2        : return "for US    /?";
			case irr::KEY_OEM_3        : return "for US    `~";
			case irr::KEY_OEM_4        : return "for US    [{";
			case irr::KEY_OEM_5        : return "for US    |";
			case irr::KEY_OEM_6        : return "for US    ]}";
			case irr::KEY_OEM_7        : return "for US    '";
			case irr::KEY_OEM_8        : return "None";
			case irr::KEY_OEM_AX       : return "for Japan AX";
			case irr::KEY_OEM_102      : return "<> or |";
			case irr::KEY_ATTN         : return "Attn key";
			case irr::KEY_CRSEL        : return "CrSel key";
			case irr::KEY_EXSEL        : return "ExSel key";
			case irr::KEY_EREOF        : return "Erase EOF key";
			case irr::KEY_PLAY         : return "Play key";
			case irr::KEY_ZOOM         : return "Zoom key";
			case irr::KEY_PA1          : return "PA1 key";
			case irr::KEY_OEM_CLEAR    : return "Clear key";
			default: return "UNKNOWN"; 
		}
	}
};


#endif // IRR_KEY_NAMES_H
