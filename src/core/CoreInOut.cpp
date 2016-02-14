//|-------------------------------------------------------------------------
//| File:        CoreInOut.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

#include "CoreSingletonsTable.h"

//----------------------------------------------------------------------------
namespace my {
namespace io {
//----------------------------------------------------------------------------

E_KEY_CODE win2my_key[E_KEY_CODE_COUNT];

//---------------------------------------------------------------------------

class CDataInitialisator
{
public:

	CDataInitialisator()
	{
		// init key codes converting array

		memset(win2my_key,0, E_KEY_CODE_COUNT);

		win2my_key[0x00                  ]=E_KEY_CODE(0); // 0x00
		win2my_key[VK_LBUTTON            ]=E_KEY_CODE(0); // 0x01
		win2my_key[VK_RBUTTON            ]=E_KEY_CODE(0); // 0x02
		win2my_key[VK_CANCEL             ]=E_KEY_CODE(0); // 0x03
		win2my_key[VK_MBUTTON            ]=E_KEY_CODE(0); // 0x04    /* NOT contiguous with L & RBUTTON */
		win2my_key[0x05                  ]=E_KEY_CODE(0); // 0x05    /* NOT contiguous with L & RBUTTON */
		win2my_key[0x06                  ]=E_KEY_CODE(0); // 0x06    /* NOT contiguous with L & RBUTTON */
		win2my_key[0x07                  ]=E_KEY_CODE(0); // 0x07
		win2my_key[VK_BACK               ]=EKC_BACKSPACE; // 0x08
		win2my_key[VK_TAB                ]=EKC_TAB;       // 0x09
		win2my_key[0x0A                  ]=E_KEY_CODE(0); // 0x0A
		win2my_key[0x0B                  ]=E_KEY_CODE(0); // 0x0B
		win2my_key[VK_CLEAR              ]=E_KEY_CODE(0); // 0x0C
		win2my_key[VK_RETURN             ]=EKC_RETURN;    // 0x0D
		win2my_key[0x0E                  ]=E_KEY_CODE(0); // 0x0E
		win2my_key[0x0F                  ]=E_KEY_CODE(0); // 0x0F
		win2my_key[VK_SHIFT              ]=EKC_LSHIFT;    // 0x10
		win2my_key[VK_CONTROL            ]=EKC_LCONTROL;  // 0x11
		win2my_key[VK_MENU               ]=EKC_LALT;      // 0x12
		win2my_key[VK_PAUSE              ]=EKC_PAUSE;     // 0x13
		win2my_key[VK_CAPITAL            ]=EKC_CAPITAL;   // 0x14
		win2my_key[VK_KANA               ]=EKC_KANA;      // 0x15
		win2my_key[0x16                  ]=E_KEY_CODE(0); // 0x16
		win2my_key[VK_JUNJA              ]=E_KEY_CODE(0); // 0x17
		win2my_key[VK_FINAL              ]=E_KEY_CODE(0); // 0x18
		win2my_key[VK_HANJA              ]=E_KEY_CODE(0); // 0x19
		win2my_key[0x1A                  ]=E_KEY_CODE(0); // 0x1A
		win2my_key[VK_ESCAPE             ]=EKC_ESCAPE;    // 0x1B
		win2my_key[VK_CONVERT            ]=EKC_CONVERT;   // 0x1C
		win2my_key[VK_NONCONVERT         ]=EKC_NONCONVERT;// 0x1D
		win2my_key[VK_ACCEPT             ]=E_KEY_CODE(0); // 0x1E
		win2my_key[VK_MODECHANGE         ]=E_KEY_CODE(0); // 0x1F
		win2my_key[VK_SPACE              ]=EKC_SPACE;     // 0x20
		win2my_key[VK_PRIOR              ]=EKC_PAGE_UP;   // 0x21
		win2my_key[VK_NEXT               ]=EKC_PAGE_DOWN; // 0x22
		win2my_key[VK_END                ]=EKC_END;       // 0x23
		win2my_key[VK_HOME               ]=EKC_HOME;      // 0x24
		win2my_key[VK_LEFT               ]=EKC_LEFT;      // 0x25
		win2my_key[VK_UP                 ]=EKC_UP;        // 0x26
		win2my_key[VK_RIGHT              ]=EKC_RIGHT;     // 0x27
		win2my_key[VK_DOWN               ]=EKC_DOWN;      // 0x28
		win2my_key[VK_SELECT             ]=E_KEY_CODE(0); // 0x29
		win2my_key[VK_PRINT              ]=EKC_SYS_RQ;    // 0x2A
		win2my_key[VK_EXECUTE            ]=E_KEY_CODE(0); // 0x2B
		win2my_key[VK_SNAPSHOT           ]=E_KEY_CODE(0); // 0x2C
		win2my_key[VK_INSERT             ]=EKC_INSERT;    // 0x2D
		win2my_key[VK_DELETE             ]=EKC_DELETE;    // 0x2E
		win2my_key[VK_HELP               ]=E_KEY_CODE(0); // 0x2F  
		win2my_key[0x30                  ]=EKC_0;         // 0x30
		win2my_key[0x31                  ]=EKC_1;         // 0x31
		win2my_key[0x32                  ]=EKC_2;         // 0x32
		win2my_key[0x33                  ]=EKC_3;         // 0x33
		win2my_key[0x34                  ]=EKC_4;         // 0x34
		win2my_key[0x35                  ]=EKC_5;         // 0x35
		win2my_key[0x36                  ]=EKC_6;         // 0x36
		win2my_key[0x37                  ]=EKC_7;         // 0x37
		win2my_key[0x38                  ]=EKC_8;         // 0x38
		win2my_key[0x39                  ]=EKC_9;         // 0x39
		win2my_key[0x3A                  ]=E_KEY_CODE(0); // 0x3A
		win2my_key[0x3B                  ]=E_KEY_CODE(0); // 0x3B
		win2my_key[0x3C                  ]=E_KEY_CODE(0); // 0x3C
		win2my_key[0x3D                  ]=E_KEY_CODE(0); // 0x3D
		win2my_key[0x3E                  ]=E_KEY_CODE(0); // 0x3E
		win2my_key[0x3F                  ]=E_KEY_CODE(0); // 0x3F
		win2my_key[0x40                  ]=E_KEY_CODE(0); // 0x40
		win2my_key[0x41                  ]=EKC_A;         // 0x41
		win2my_key[0x42                  ]=EKC_B;         // 0x42
		win2my_key[0x43                  ]=EKC_C;         // 0x43
		win2my_key[0x44                  ]=EKC_D;         // 0x44
		win2my_key[0x45                  ]=EKC_E;         // 0x45
		win2my_key[0x46                  ]=EKC_F;         // 0x46
		win2my_key[0x47                  ]=EKC_G;         // 0x47
		win2my_key[0x48                  ]=EKC_H;         // 0x48
		win2my_key[0x49                  ]=EKC_I;         // 0x49
		win2my_key[0x4A                  ]=EKC_J;         // 0x4A
		win2my_key[0x4B                  ]=EKC_K;         // 0x4B
		win2my_key[0x4C                  ]=EKC_L;         // 0x4C
		win2my_key[0x4D                  ]=EKC_M;         // 0x4D
		win2my_key[0x4E                  ]=EKC_N;         // 0x4E
		win2my_key[0x4F                  ]=EKC_O;         // 0x4F
		win2my_key[0x50                  ]=EKC_P;         // 0x50
		win2my_key[0x51                  ]=EKC_Q;         // 0x51
		win2my_key[0x52                  ]=EKC_R;         // 0x52
		win2my_key[0x53                  ]=EKC_S;         // 0x53
		win2my_key[0x54                  ]=EKC_T;         // 0x54
		win2my_key[0x55                  ]=EKC_U;         // 0x55
		win2my_key[0x56                  ]=EKC_V;         // 0x56
		win2my_key[0x57                  ]=EKC_W;         // 0x57
		win2my_key[0x58                  ]=EKC_X;         // 0x58
		win2my_key[0x59                  ]=EKC_Y;         // 0x59
		win2my_key[0x5A                  ]=EKC_Z;         // 0x5A
		win2my_key[VK_LWIN               ]=EKC_LWIN;      // 0x5B
		win2my_key[VK_RWIN               ]=EKC_RWIN;      // 0x5C
		win2my_key[VK_APPS               ]=EKC_APPS;      // 0x5D
		win2my_key[0x5E                  ]=E_KEY_CODE(0); // 0x5E
		win2my_key[VK_SLEEP              ]=EKC_SLEEP;     // 0x5F
		win2my_key[VK_NUMPAD0            ]=EKC_NUMPAD0;   // 0x60
		win2my_key[VK_NUMPAD1            ]=EKC_NUMPAD1;   // 0x61
		win2my_key[VK_NUMPAD2            ]=EKC_NUMPAD2;   // 0x62
		win2my_key[VK_NUMPAD3            ]=EKC_NUMPAD3;   // 0x63
		win2my_key[VK_NUMPAD4            ]=EKC_NUMPAD4;   // 0x64
		win2my_key[VK_NUMPAD5            ]=EKC_NUMPAD5;   // 0x65
		win2my_key[VK_NUMPAD6            ]=EKC_NUMPAD6;   // 0x66
		win2my_key[VK_NUMPAD7            ]=EKC_NUMPAD7;   // 0x67
		win2my_key[VK_NUMPAD8            ]=EKC_NUMPAD8;   // 0x68
		win2my_key[VK_NUMPAD9            ]=EKC_NUMPAD9;   // 0x69
		win2my_key[VK_MULTIPLY           ]=EKC_MULTIPLY;  // 0x6A
		win2my_key[VK_ADD                ]=EKC_ADD;       // 0x6B
		win2my_key[VK_SEPARATOR          ]=E_KEY_CODE(0); // 0x6C
		win2my_key[VK_SUBTRACT           ]=EKC_SUBSTRACT; // 0x6D
		win2my_key[VK_DECIMAL            ]=EKC_DECIMAL;   // 0x6E
		win2my_key[VK_DIVIDE             ]=EKC_DIVIDE;    // 0x6F
		win2my_key[VK_F1                 ]=EKC_F1;        // 0x70
		win2my_key[VK_F2                 ]=EKC_F2;        // 0x71
		win2my_key[VK_F3                 ]=EKC_F3;        // 0x72
		win2my_key[VK_F4                 ]=EKC_F4;        // 0x73
		win2my_key[VK_F5                 ]=EKC_F5;        // 0x74
		win2my_key[VK_F6                 ]=EKC_F6;        // 0x75
		win2my_key[VK_F7                 ]=EKC_F7;        // 0x76
		win2my_key[VK_F8                 ]=EKC_F8;        // 0x77
		win2my_key[VK_F9                 ]=EKC_F9;        // 0x78
		win2my_key[VK_F10                ]=EKC_F10;       // 0x79
		win2my_key[VK_F11                ]=EKC_F11;       // 0x7A
		win2my_key[VK_F12                ]=EKC_F12;       // 0x7B
		win2my_key[VK_F13                ]=EKC_F13;       // 0x7C
		win2my_key[VK_F14                ]=EKC_F14;       // 0x7D
		win2my_key[VK_F15                ]=EKC_F15;       // 0x7E
		win2my_key[VK_F16                ]=E_KEY_CODE(0); // 0x7F
		win2my_key[VK_F17                ]=E_KEY_CODE(0); // 0x80
		win2my_key[VK_F18                ]=E_KEY_CODE(0); // 0x81
		win2my_key[VK_F19                ]=E_KEY_CODE(0); // 0x82
		win2my_key[VK_F20                ]=E_KEY_CODE(0); // 0x83
		win2my_key[VK_F21                ]=E_KEY_CODE(0); // 0x84
		win2my_key[VK_F22                ]=E_KEY_CODE(0); // 0x85
		win2my_key[VK_F23                ]=E_KEY_CODE(0); // 0x86
		win2my_key[VK_F24                ]=E_KEY_CODE(0); // 0x87
		win2my_key[0x88                  ]=E_KEY_CODE(0); // 0x88
		win2my_key[0x89                  ]=E_KEY_CODE(0); // 0x89
		win2my_key[0x8A                  ]=E_KEY_CODE(0); // 0x8A
		win2my_key[0x8B                  ]=E_KEY_CODE(0); // 0x8B
		win2my_key[0x8C                  ]=E_KEY_CODE(0); // 0x8C
		win2my_key[0x8D                  ]=E_KEY_CODE(0); // 0x8D
		win2my_key[0x8E                  ]=E_KEY_CODE(0); // 0x8E
		win2my_key[0x8F                  ]=E_KEY_CODE(0); // 0x8F
		win2my_key[VK_NUMLOCK            ]=EKC_NUMLOCK;   // 0x90
		win2my_key[VK_SCROLL             ]=EKC_SCROLL;    // 0x91
		win2my_key[VK_OEM_FJ_JISHO       ]=E_KEY_CODE(0); // 0x92   // 'Dictionary' key (Fujitsu/OASYS kbd)
		win2my_key[VK_OEM_FJ_MASSHOU     ]=E_KEY_CODE(0); // 0x93   // 'Unregister word' key (Fujitsu/OASYS kbd)
		win2my_key[VK_OEM_FJ_TOUROKU     ]=E_KEY_CODE(0); // 0x94   // 'Register word' key (Fujitsu/OASYS kbd)
		win2my_key[VK_OEM_FJ_LOYA        ]=E_KEY_CODE(0); // 0x95   // 'Left OYAYUBI' key (Fujitsu/OASYS kbd)
		win2my_key[VK_OEM_FJ_ROYA        ]=E_KEY_CODE(0); // 0x96   // 'Right OYAYUBI' key (Fujitsu/OASYS kbd)
		win2my_key[0x97                  ]=E_KEY_CODE(0); // 0x97
		win2my_key[0x98                  ]=E_KEY_CODE(0); // 0x98
		win2my_key[0x99                  ]=E_KEY_CODE(0); // 0x99
		win2my_key[0x9A                  ]=E_KEY_CODE(0); // 0x9A
		win2my_key[0x9B                  ]=E_KEY_CODE(0); // 0x9B
		win2my_key[0x9C                  ]=E_KEY_CODE(0); // 0x9C
		win2my_key[0x9D                  ]=E_KEY_CODE(0); // 0x9D
		win2my_key[0x9E                  ]=E_KEY_CODE(0); // 0x9E
		win2my_key[0x9F                  ]=E_KEY_CODE(0); // 0x9F
		win2my_key[VK_LSHIFT             ]=EKC_LSHIFT;    // 0xA0
		win2my_key[VK_RSHIFT             ]=EKC_RSHIFT;    // 0xA1
		win2my_key[VK_LCONTROL           ]=EKC_LCONTROL;  // 0xA2
		win2my_key[VK_RCONTROL           ]=EKC_RCONTROL;  // 0xA3
		win2my_key[VK_LMENU              ]=EKC_LALT;      // 0xA4
		win2my_key[VK_RMENU              ]=EKC_RALT;      // 0xA5
		win2my_key[0xA6                  ]=EKC_WEB_BACK;      // 0xA6
		win2my_key[0xA7                  ]=EKC_WEB_FORWARD;   // 0xA7
		win2my_key[0xA8                  ]=EKC_WEB_REFRESH;   // 0xA8
		win2my_key[0xA9                  ]=EKC_WEB_STOP;      // 0xA9
		win2my_key[0xAA                  ]=EKC_WEB_SEARCH;    // 0xAA
		win2my_key[0xAB                  ]=EKC_WEB_FAVORITES; // 0xAB
		win2my_key[0xAC                  ]=EKC_WEB_HOME;      // 0xAC
		win2my_key[0xAD                  ]=EKC_MUTE;          // 0xAD
		win2my_key[0xAE                  ]=EKC_VOLUME_DOWN;   // 0xAE
		win2my_key[0xAF                  ]=EKC_VOLUME_UP;     // 0xAF
		win2my_key[0xB0                  ]=EKC_NEXT_TRACK;    // 0xB0
		win2my_key[0xB1                  ]=EKC_PREV_TRACK;    // 0xB1
		win2my_key[0xB2                  ]=EKC_MEDIA_STOP;    // 0xB2
		win2my_key[0xB3                  ]=EKC_PLAY_PAUSE;    // 0xB3
		win2my_key[0xB4                  ]=EKC_MAIL;          // 0xB4
		win2my_key[0xB5                  ]=EKC_MEDIA_SELECT;  // 0xB5
		win2my_key[0xB6                  ]=E_KEY_CODE(0); // 0xB6
		win2my_key[0xB7                  ]=E_KEY_CODE(0); // 0xB7
		win2my_key[0xB8                  ]=E_KEY_CODE(0); // 0xB8
		win2my_key[0xB9                  ]=E_KEY_CODE(0); // 0xB9
		win2my_key[VK_OEM_1              ]=E_KEY_CODE(0); // 0xBA  // ';:' for US
		win2my_key[VK_OEM_PLUS           ]=EKC_ADD;       // 0xBB  // '+' any country
		win2my_key[VK_OEM_COMMA          ]=EKC_COMMA;     // 0xBC  // ',' any country
		win2my_key[VK_OEM_MINUS          ]=EKC_SUBSTRACT; // 0xBD  // '-' any country
		win2my_key[VK_OEM_PERIOD         ]=EKC_PERIOD;    // 0xBE  // '.' any country
		win2my_key[VK_OEM_2              ]=EKC_SLASH;     // 0xBF  // '/?' for US
		win2my_key[0xC0                  ]=EKC_GRAVE;     // 0xC0  // '`~' for US
		win2my_key[0xC1                  ]=E_KEY_CODE(0); // 0xC1
		win2my_key[0xC2                  ]=E_KEY_CODE(0); // 0xC2
		win2my_key[0xC3                  ]=E_KEY_CODE(0); // 0xC3
		win2my_key[0xC4                  ]=E_KEY_CODE(0); // 0xC4
		win2my_key[0xC5                  ]=E_KEY_CODE(0); // 0xC5
		win2my_key[0xC6                  ]=E_KEY_CODE(0); // 0xC6
		win2my_key[0xC7                  ]=E_KEY_CODE(0); // 0xC7
		win2my_key[0xC8                  ]=E_KEY_CODE(0); // 0xC8
		win2my_key[0xC9                  ]=E_KEY_CODE(0); // 0xC9
		win2my_key[0xCA                  ]=E_KEY_CODE(0); // 0xCA
		win2my_key[0xCB                  ]=E_KEY_CODE(0); // 0xCB
		win2my_key[0xCC                  ]=E_KEY_CODE(0); // 0xCC
		win2my_key[0xCD                  ]=E_KEY_CODE(0); // 0xCD
		win2my_key[0xCE                  ]=E_KEY_CODE(0); // 0xCE
		win2my_key[0xCF                  ]=E_KEY_CODE(0); // 0xCF
		win2my_key[0xD0                  ]=E_KEY_CODE(0); // 0xD0
		win2my_key[0xD1                  ]=E_KEY_CODE(0); // 0xD1
		win2my_key[0xD2                  ]=E_KEY_CODE(0); // 0xD2
		win2my_key[0xD3                  ]=E_KEY_CODE(0); // 0xD3
		win2my_key[0xD4                  ]=E_KEY_CODE(0); // 0xD4
		win2my_key[0xD5                  ]=E_KEY_CODE(0); // 0xD5
		win2my_key[0xD6                  ]=E_KEY_CODE(0); // 0xD6
		win2my_key[0xD7                  ]=E_KEY_CODE(0); // 0xD7
		win2my_key[0xD8                  ]=E_KEY_CODE(0); // 0xD8
		win2my_key[0xD9                  ]=E_KEY_CODE(0); // 0xD9
		win2my_key[0xDA                  ]=E_KEY_CODE(0); // 0xDA
		win2my_key[VK_OEM_4              ]=EKC_LBRACKET;  // 0xDB //  '[{' for US
		win2my_key[VK_OEM_5              ]=EKC_BACKSLASH; // 0xDC //  '\|' for US
		win2my_key[VK_OEM_6              ]=EKC_RBRACKET;  // 0xDD //  ']}' for US
		win2my_key[VK_OEM_7              ]=EKC_APOSTROPHE; // 0xDE //  ''"' for US
		win2my_key[0xDF                  ]=E_KEY_CODE(0); // 0xDF
		win2my_key[0xE0                  ]=E_KEY_CODE(0); // 0xE0
		win2my_key[0xE2                  ]=EKC_AX;        // 0xE1 //  'AX' key on Japanese AX kbd
		win2my_key[VK_OEM_102            ]=EKC_OEM_102;   // 0xE2 //  "<>" or "\|" on RT 102-key kbd.
		win2my_key[0xE3                  ]=E_KEY_CODE(0); // 0xE3 //  Help key on ICO
		win2my_key[0xE4                  ]=E_KEY_CODE(0); // 0xE4 //  00 key on ICO
		win2my_key[0xE5                  ]=E_KEY_CODE(0); // 0xE5
		win2my_key[0xE6                  ]=E_KEY_CODE(0); // 0xE6
		win2my_key[0xE7                  ]=E_KEY_CODE(0); // 0xE7
		win2my_key[0xE8                  ]=E_KEY_CODE(0); // 0xE8
		win2my_key[0xE9                  ]=E_KEY_CODE(0); // 0xE9
		win2my_key[0xEA                  ]=E_KEY_CODE(0); // 0xEA
		win2my_key[0xEB                  ]=E_KEY_CODE(0); // 0xEB
		win2my_key[0xEC                  ]=E_KEY_CODE(0); // 0xEC
		win2my_key[0xED                  ]=E_KEY_CODE(0); // 0xED
		win2my_key[0xEE                  ]=E_KEY_CODE(0); // 0xEE
		win2my_key[0xEF                  ]=E_KEY_CODE(0); // 0xEF
		win2my_key[0xF0                  ]=E_KEY_CODE(0); // 0xF0
		win2my_key[0xF1                  ]=E_KEY_CODE(0); // 0xF1
		win2my_key[0xF2                  ]=E_KEY_CODE(0); // 0xF2
		win2my_key[0xF3                  ]=E_KEY_CODE(0); // 0xF3
		win2my_key[0xF4                  ]=E_KEY_CODE(0); // 0xF4
		win2my_key[0xF5                  ]=E_KEY_CODE(0); // 0xF5
		win2my_key[0xF6                  ]=E_KEY_CODE(0); // 0xF6
		win2my_key[0xF7                  ]=E_KEY_CODE(0); // 0xF7
		win2my_key[0xF8                  ]=E_KEY_CODE(0); // 0xF8
		win2my_key[0xF9                  ]=E_KEY_CODE(0); // 0xF9
		win2my_key[0xFA                  ]=E_KEY_CODE(0); // 0xFA
		win2my_key[0xFB                  ]=E_KEY_CODE(0); // 0xFB
		win2my_key[0xFC                  ]=E_KEY_CODE(0); // 0xFC
		win2my_key[0xFD                  ]=E_KEY_CODE(0); // 0xFD
		win2my_key[0xFE                  ]=E_KEY_CODE(0); // 0xFE      
	}
}
DataInitialisator;  

//----------------------------------------------------------------------------

void log(const c8 *text, io::E_LOG_LEVEL ll)
{
	core::g_SingletonsTable.Ptrs.Logger->log(text, ll);	
}

//----------------------------------------------------------------------------

E_KEY_CODE win2MyKeyCode(u8 kcode)
{
	return win2my_key[kcode];
}

//----------------------------------------------------------------------------

const c8* getKeyCodeName(io::E_KEY_CODE kcode)
{
	return io::KeyCodeNameStr[kcode];
}   

//----------------------------------------------------------------------------

const c8* getMouseCodeName(io::E_MOUSE_CODE mcode)
{
	return io::MouseCodeNameStr[mcode];
}

//----------------------------------------------------------------------------

bool getActionControlState(
	io::IEventCapturer *capturer, game::E_GAME_ACTION action)
{
	IInputDispatcher &disp = INPUT_DISPATCHER;
	return capturer->getKeyState(disp.getActionKey(action)) ||
		capturer->getMouseState(disp.getActionMouse(action));
}

//----------------------------------------------------------------------------

void resetActionControlState(
	io::IEventCapturer *capturer, game::E_GAME_ACTION action)
{
	IInputDispatcher &disp = INPUT_DISPATCHER;
	capturer->resetKeyState(disp.getActionKey(action));
	capturer->resetMouseState(disp.getActionMouse(action));
}

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------



