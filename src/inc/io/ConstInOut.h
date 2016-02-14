//|-------------------------------------------------------------------------
//| File:        ConstInOut.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_ioHPP
#define my_const_ioHPP

//----------------------------------------------------------------------------
namespace my {
namespace io {
//-----------------------------------------------------------------------------

//! enumeration for input types
enum E_INPUT_TYPE
{
    //! standard input
    EIT_STANDARD_INPUT =0,

    //! for counting enumeration
    E_INPUT_TYPE_COUNT,

    E_INPUT_TYPE_COUNT_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_LOG_LEVEL
{
    //! High log level, warnings, errors and important information
    //! texts are printed out.
    ELL_INFORMATION = 0,

    //! Default log level, warnings and errors are printed out
    ELL_WARNING,

    //! Low log level, only errors are printed into the log
    ELL_ERROR,

    //! Nothing is printed out to the log
    ELL_NONE,

    E_LOG_LEVEL_COUNT,

    E_LOG_LEVEL_FORCE_32_BIT = 0xffffffff
};  

//----------------------------------------------------------------------------

//! Enumeration of scan codes.  
//! Customised from table taken from Ogre, which took them from DirectInput...
enum E_KEY_CODE
{
    //                =0x00
    EKC_ESCAPE        =0x01,
    EKC_1             =0x02,
    EKC_2             =0x03,
    EKC_3             =0x04,
    EKC_4             =0x05,
    EKC_5             =0x06,
    EKC_6             =0x07,
    EKC_7             =0x08,
    EKC_8             =0x09,
    EKC_9             =0x0A,
    EKC_0             =0x0B,
    EKC_MINUS         =0x0C,    /* - on main keyboard */
    EKC_EQUALS        =0x0D,
    EKC_BACKSPACE     =0x0E,    /* backspace */
    EKC_TAB           =0x0F,
    EKC_Q             =0x10,
    EKC_W             =0x11,
    EKC_E             =0x12,
    EKC_R             =0x13,
    EKC_T             =0x14,
    EKC_Y             =0x15,
    EKC_U             =0x16,
    EKC_I             =0x17,
    EKC_O             =0x18,
    EKC_P             =0x19,
    EKC_LBRACKET      =0x1A,
    EKC_RBRACKET      =0x1B,
    EKC_RETURN        =0x1C,    /* Enter on main keyboard */
    EKC_LCONTROL      =0x1D,
    EKC_A             =0x1E,
    EKC_S             =0x1F,
    EKC_D             =0x20,
    EKC_F             =0x21,
    EKC_G             =0x22,
    EKC_H             =0x23,
    EKC_J             =0x24,
    EKC_K             =0x25,
    EKC_L             =0x26,
    EKC_SEMICOLON     =0x27,
    EKC_APOSTROPHE    =0x28,
    EKC_GRAVE         =0x29,    /* ~ accent grave */
    EKC_LSHIFT        =0x2A,
    EKC_BACKSLASH     =0x2B,
    EKC_Z             =0x2C,
    EKC_X             =0x2D,
    EKC_C             =0x2E,
    EKC_V             =0x2F,
    EKC_B             =0x30,
    EKC_N             =0x31,
    EKC_M             =0x32,
    EKC_COMMA         =0x33,    /* , on main keyboard */
    EKC_PERIOD        =0x34,    /* . on main keyboard */
    EKC_SLASH         =0x35,    /* '/' on main keyboard */
    EKC_RSHIFT        =0x36,
    EKC_MULTIPLY      =0x37,    /* * on numeric keypad */
    EKC_LALT          =0x38,    /* left Alt */
    EKC_SPACE         =0x39,
    EKC_CAPITAL       =0x3A,    /* CAPS LOCK key  */
    EKC_F1            =0x3B,
    EKC_F2            =0x3C,
    EKC_F3            =0x3D,
    EKC_F4            =0x3E,
    EKC_F5            =0x3F,
    EKC_F6            =0x40,
    EKC_F7            =0x41,
    EKC_F8            =0x42,
    EKC_F9            =0x43,
    EKC_F10           =0x44,
    EKC_NUMLOCK       =0x45,
    EKC_SCROLL        =0x46,    /* Scroll Lock */
    EKC_NUMPAD7       =0x47,
    EKC_NUMPAD8       =0x48,
    EKC_NUMPAD9       =0x49,
    EKC_SUBSTRACT     =0x4A,    /* - on numeric keypad */
    EKC_NUMPAD4       =0x4B,
    EKC_NUMPAD5       =0x4C,
    EKC_NUMPAD6       =0x4D,
    EKC_ADD           =0x4E,    /* + on numeric keypad */
    EKC_NUMPAD1       =0x4F,
    EKC_NUMPAD2       =0x50,
    EKC_NUMPAD3       =0x51,
    EKC_NUMPAD0       =0x52,
    EKC_DECIMAL       =0x53,    /* . on numeric keypad */
    //                =0x54
    //                =0x55
    EKC_OEM_102       =0x56,    /* < > | on UK/Germany keyboards */
    EKC_F11           =0x57,
    EKC_F12           =0x58,
    //                =0x59
    //                =0x5A
    //                =0x5B
    //                =0x5C
    //                =0x5D
    //                =0x5E
    //                =0x5F
    //                =0x60
    //                =0x61
    //                =0x62
    //                =0x63
    EKC_F13           =0x64,    /*                     (NEC PC98) */
    EKC_F14           =0x65,    /*                     (NEC PC98) */
    EKC_F15           =0x66,    /*                     (NEC PC98) */
    //                =0x67 
    //                =0x68 
    //                =0x69
    //                =0x6A
    //                =0x6B
    //                =0x6C
    //                =0x6D
    //                =0x6E
    //                =0x6F
    EKC_KANA          =0x70,    /* (Japanese keyboard)            */
    //                =0x71
    //                =0x72
    EKC_ABNT_C1       =0x73,    /* / ? on Portugese (Brazilian) keyboards */
    //                =0x74
    //                =0x75
    //                =0x76
    //                =0x77
    //                =0x78
    EKC_CONVERT       =0x79,    /* (Japanese keyboard)            */
    //                =0x7A
    EKC_NONCONVERT    =0x7B,    /* (Japanese keyboard)            */
    //                =0x7C
    EKC_YEN           =0x7D,    /* (Japanese keyboard)            */
    EKC_ABNT_C2       =0x7E,    /* Numpad . on Portugese (Brazilian) keyboards */
    //                =0x7F
    //                =0x80
    //                =0x81
    //                =0x82
    //                =0x83
    //                =0x84
    //                =0x85
    //                =0x86
    //                =0x87
    //                =0x88
    //                =0x89
    //                =0x8A
    //                =0x8B
    //                =0x8C 
    EKC_NUMPAD_EQUALS =0x8D,    /* = on numeric keypad (NEC PC98) */
    //                =0x8E
    //                =0x8F
    EKC_PREV_TRACK    =0x90,    /* Previous Track (KC_CIRCUMFLEX on Japanese keyboard) */
    EKC_AT            =0x91,    /*                     (NEC PC98) */
    EKC_COLON         =0x92,    /*                     (NEC PC98) */
    EKC_UNDERLINE     =0x93,    /*                     (NEC PC98) */
    EKC_KANJI         =0x94,    /* (Japanese keyboard)            */
    EKC_STOP          =0x95,    /*                     (NEC PC98) */
    EKC_AX            =0x96,    /*                     (Japan AX) */
    EKC_UNLABELED     =0x97,    /*                        (J3100) */
    //                =0x98
    EKC_NEXT_TRACK    =0x99,    /* Next Track */
    //                =0x9A
    //                =0x9B
    EKC_NUMPAD_ENTER  =0x9C,    /* Enter on numeric keypad */
    EKC_RCONTROL      =0x9D,
    //                =0x9E
    //                =0x9F
    EKC_MUTE          =0xA0,    /* Mute */
    EKC_CALCULATOR    =0xA1,    /* Calculator */
    EKC_PLAY_PAUSE    =0xA2,    /* Play / Pause */
    //                =0xA3
    EKC_MEDIA_STOP    =0xA4,    /* Media Stop */
    //                =0xA5
    //                =0xA6
    //                =0xA7
    //                =0xA8
    //                =0xA9
    //                =0xAA
    //                =0xAB
    //                =0xAC
    //                =0xAD
    EKC_VOLUME_DOWN   =0xAE,    /* Volume - */
    //                =0xAF
    EKC_VOLUME_UP     =0xB0,    /* Volume + */
    //                =0xB1
    EKC_WEB_HOME      =0xB2,    /* Web home */
    EKC_NUMPAD_COMMA  =0xB3,    /* , on numeric keypad (NEC PC98) */
    //                =0xB4
    EKC_DIVIDE        =0xB5,    /* / on numeric keypad */
    //                =0xB6
    EKC_SYS_RQ        =0xB7,
    EKC_RALT          =0xB8,    /* right Alt */
    //                =0xB9
    //                =0xBA
    //                =0xBB
    //                =0xBC
    //                =0xBD
    //                =0xBE
    //                =0xBF
    //                =0xC0
    //                =0xC1
    //                =0xC2
    //                =0xC3
    //                =0xC4
    EKC_PAUSE         =0xC5,    /* Pause */
    //                =0xC6
    EKC_HOME          =0xC7,    /* Home on arrow keypad */
    EKC_UP            =0xC8,    /* UpArrow on arrow keypad */
    EKC_PAGE_UP       =0xC9,    /* PgUp on arrow keypad */
    //                =0xCA
    EKC_LEFT          =0xCB,    /* LeftArrow on arrow keypad */
    //                =0xCC
    EKC_RIGHT         =0xCD,    /* RightArrow on arrow keypad */
    //                =0xCE
    EKC_END           =0xCF,    /* End on arrow keypad */
    EKC_DOWN          =0xD0,    /* DownArrow on arrow keypad */
    EKC_PAGE_DOWN     =0xD1,    /* PgDn on arrow keypad */
    EKC_INSERT        =0xD2,    /* Insert on arrow keypad */
    EKC_DELETE        =0xD3,    /* Delete on arrow keypad */
    //                =0xD4
    //                =0xD5
    //                =0xD6
    //                =0xD7
    //                =0xD8
    //                =0xD9
    //                =0xDA
    EKC_LWIN          =0xDB,    /* Left Windows key */
    EKC_RWIN          =0xDC,    /* Right Windows key */
    EKC_APPS          =0xDD,    /* AppMenu key */
    EKC_POWER         =0xDE,    /* System Power */
    EKC_SLEEP         =0xDF,    /* System Sleep */
    //                =0xE0
    //                =0xE1
    //                =0xE2
    EKC_WAKE          =0xE3,    /* System Wake */
    //                =0xE4
    EKC_WEB_SEARCH    =0xE5,    /* Web Search */
    EKC_WEB_FAVORITES =0xE6,    /* Web Favorites */
    EKC_WEB_REFRESH   =0xE7,    /* Web Refresh */
    EKC_WEB_STOP      =0xE8,    /* Web Stop */
    EKC_WEB_FORWARD   =0xE9,    /* Web Forward */
    EKC_WEB_BACK      =0xEA,    /* Web Back */
    EKC_MY_COMPUTER   =0xEB,    /* My Computer */
    EKC_MAIL          =0xEC,    /* Mail */
    EKC_MEDIA_SELECT  =0xED,    /* Media Select */
    //                =0xEE
    //                =0xEF
    //                =0xF0
    //                =0xF1
    //                =0xF2
    //                =0xF3
    //                =0xF4
    //                =0xF5
    //                =0xF6
    //                =0xF7
    //                =0xF8
    //                =0xF9
    //                =0xFA
    //                =0xFB
    //                =0xFC
    //                =0xFD
    //                =0xFE

    E_KEY_CODE_COUNT  =0xFF,

    E_KEY_CODE_FORCE_32_BIT = 0xffffffff 
};

//----------------------------------------------------------------------------

// keyboard key captions as string
static const c8* KeyCodeNameStr[E_KEY_CODE_COUNT] = 
{ 
    "<???>",              //                      =0x00,
    "Esc",                //    EKC_ESCAPE        =0x01,                                                                          
    "1",                  //    EKC_1             =0x02,                                                                          
    "2",                  //    EKC_2             =0x03,                                                                          
    "3",                  //    EKC_3             =0x04,                                                                          
    "4",                  //    EKC_4             =0x05,                                                                          
    "5",                  //    EKC_5             =0x06,                                                                          
    "6",                  //    EKC_6             =0x07,                                                                          
    "7",                  //    EKC_7             =0x08,                                                                          
    "8",                  //    EKC_8             =0x09,                                                                          
    "9",                  //    EKC_9             =0x0A,                                                                          
    "0",                  //    EKC_0             =0x0B,                                                                          
    "-",                  //    EKC_MINUS         =0x0C,    /* - on main keyboard */                                              
    "=",                  //    EKC_EQUALS        =0x0D,                                                                          
    "Backspace",          //    EKC_BACK          =0x0E,    /* backspace */                                                       
    "Tab",                //    EKC_TAB           =0x0F,                                                                          
    "Q",                  //    EKC_Q             =0x10,                                                                          
    "W",                  //    EKC_W             =0x11,                                                                          
    "E",                  //    EKC_E             =0x12,                                                                          
    "R",                  //    EKC_R             =0x13,                                                                          
    "T",                  //    EKC_T             =0x14,                                                                          
    "Y",                  //    EKC_Y             =0x15,                                                                          
    "U",                  //    EKC_U             =0x16,                                                                          
    "I",                  //    EKC_I             =0x17,                                                                          
    "O",                  //    EKC_O             =0x18,                                                                          
    "P",                  //    EKC_P             =0x19,                                                                          
    "[",                  //    EKC_LBRACKET      =0x1A,                                                                          
    "]",                  //    EKC_RBRACKET      =0x1B,                                                                          
    "Enter",              //    EKC_RETURN        =0x1C,    /* Enter on main keyboard */                                          
    "LCtrl",              //    EKC_LCONTROL      =0x1D,                                                                          
    "A",                  //    EKC_A             =0x1E,                                                                          
    "S",                  //    EKC_S             =0x1F,                                                                          
    "D",                  //    EKC_D             =0x20,                                                                          
    "F",                  //    EKC_F             =0x21,                                                                          
    "G",                  //    EKC_G             =0x22,                                                                          
    "H",                  //    EKC_H             =0x23,                                                                          
    "J",                  //    EKC_J             =0x24,                                                                          
    "K",                  //    EKC_K             =0x25,                                                                          
    "L",                  //    EKC_L             =0x26,                                                                          
    "SemiColon",          //    EKC_SEMICOLON     =0x27,                                                                        
    "\"",                 //    EKC_APOSTROPHE    =0x28,                                                                        
    "~",                  //    EKC_GRAVE         =0x29,    /* ~ accent grave */                                                  
    "LShift",             //    EKC_LSHIFT        =0x2A,                                                                          
    "\\",                 //    EKC_BACKSLASH     =0x2B,                                                                          
    "Z",                  //    EKC_Z             =0x2C,                                                                          
    "X",                  //    EKC_X             =0x2D,                                                                          
    "C",                  //    EKC_C             =0x2E,                                                                          
    "V",                  //    EKC_V             =0x2F,                                                                          
    "B",                  //    EKC_B             =0x30,                                                                          
    "N",                  //    EKC_N             =0x31,                                                                          
    "M",                  //    EKC_M             =0x32,                                                                          
    ",",                  //    EKC_COMMA         =0x33,    /* , on main keyboard */                                              
    ".",                  //    EKC_PERIOD        =0x34,    /* . on main keyboard */                                              
    "/",                  //    EKC_SLASH         =0x35,    /* '/' on main keyboard */                                            
    "RShift",             //    EKC_RSHIFT        =0x36,                                                                          
    "*",                  //    EKC_MULTIPLY      =0x37,    /* * on numeric keypad */                                             
    "LAlt",               //    EKC_LALT          =0x38,    /* left Alt */                                                        
    "Space",              //    EKC_SPACE         =0x39,                                                                          
    "CapsLock",           //    EKC_CAPITAL       =0x3A,    /* CAPS LOCK key  */                                                  
    "F1",                 //    EKC_F1            =0x3B,                                                                          
    "F2",                 //    EKC_F2            =0x3C,                                                                          
    "F3",                 //    EKC_F3            =0x3D,                                                                          
    "F4",                 //    EKC_F4            =0x3E,                                                                          
    "F5",                 //    EKC_F5            =0x3F,                                                                          
    "F6",                 //    EKC_F6            =0x40,                                                                          
    "F7",                 //    EKC_F7            =0x41,                                                                          
    "F8",                 //    EKC_F8            =0x42,                                                                          
    "F9",                 //    EKC_F9            =0x43,                                                                          
    "F10",                //    EKC_F10           =0x44,                                                                          
    "NumLock",            //    EKC_NUMLOCK       =0x45,                                                                          
    "ScrollLock",         //    EKC_SCROLL        =0x46,    /* Scroll Lock */                                                     
    "Num 7",              //    EKC_NUMPAD7       =0x47,                                                                          
    "Num 8",              //    EKC_NUMPAD8       =0x48,                                                                          
    "Num 9",              //    EKC_NUMPAD9       =0x49,                                                                          
    "-",                  //    EKC_SUBSTRACT     =0x4A,    /* - on numeric keypad */                                             
    "Num 4",              //    EKC_NUMPAD4       =0x4B,                                                                          
    "Num 5",              //    EKC_NUMPAD5       =0x4C,                                                                          
    "Num 6",              //    EKC_NUMPAD6       =0x4D,                                                                          
    "+",                  //    EKC_ADD           =0x4E,    /* + on numeric keypad */                                             
    "Num 1",              //    EKC_NUMPAD1       =0x4F,                                                                          
    "Num 2",              //    EKC_NUMPAD2       =0x50,                                                                          
    "Num 3",              //    EKC_NUMPAD3       =0x51,                                                                          
    "Num 0",              //    EKC_NUMPAD0       =0x52,                                                                          
    "Decimal",            //    EKC_DECIMAL       =0x53,    /* . on numeric keypad */                                             
    "<???>",              //    //                =0x54                                                                           
    "<???>",              //    //                =0x55                                                                           
    "OEM_102",            //    EKC_OEM_102       =0x56,    /* < > | on UK/Germany keyboards */                                   
    "F11",                //    EKC_F11           =0x57,                                                                          
    "F12",                //    EKC_F12           =0x58,                                                                          
    "<???>",              //    //                =0x59                                                                           
    "<???>",              //    //                =0x5A                                                                           
    "<???>",              //    //                =0x5B                                                                           
    "<???>",              //    //                =0x5C                                                                           
    "<???>",              //    //                =0x5D                                                                           
    "<???>",              //    //                =0x5E                                                                           
    "<???>",              //    //                =0x5F                                                                           
    "<???>",              //    //                =0x60                                                                           
    "<???>",              //    //                =0x61                                                                           
    "<???>",              //    //                =0x62                                                                           
    "<???>",              //    //                =0x63                                                                           
    "F13",                //    EKC_F13           =0x64,    /*                     (NEC PC98) */                                  
    "F14",                //    EKC_F14           =0x65,    /*                     (NEC PC98) */                                  
    "F15",                //    EKC_F15           =0x66,    /*                     (NEC PC98) */                                  
    "<???>",              //    //                =0x67                                                                           
    "<???>",              //    //                =0x68                                                                           
    "<???>",              //    //                =0x69                                                                           
    "<???>",              //    //                =0x6A                                                                           
    "<???>",              //    //                =0x6B                                                                           
    "<???>",              //    //                =0x6C                                                                           
    "<???>",              //    //                =0x6D                                                                           
    "<???>",              //    //                =0x6E                                                                           
    "<???>",              //    //                =0x6F                                                                           
    "Kana",               //    EKC_KANA          =0x70,    /* (Japanese keyboard)            */                                  
    "<???>",              //    //                =0x71                                                                           
    "<???>",              //    //                =0x72                                                                           
    "ABNT_C1",            //    EKC_ABNT_C1       =0x73,    /* / ? on Portugese (Brazilian) keyboards */                          
    "<???>",              //    //                =0x74                                                                           
    "<???>",              //    //                =0x75                                                                           
    "<???>",              //    //                =0x76                                                                           
    "<???>",              //    //                =0x77                                                                           
    "<???>",              //    //                =0x78                                                                           
    "Conv",               //    EKC_CONVERT       =0x79,    /* (Japanese keyboard)            */                                  
    "<???>",              //    //                =0x7A                                                                           
    "NonConv",            //    EKC_NONCONVERT    =0x7B,    /* (Japanese keyboard)            */                                  
    "<???>",              //    //                =0x7C                                                                           
    "Yen",                //    EKC_YEN           =0x7D,    /* (Japanese keyboard)            */                                  
    "ABNT_C2",            //    EKC_ABNT_C2       =0x7E,    /* Numpad . on Portugese (Brazilian) keyboards */                     
    "<???>",              //    //                =0x7F                                                                           
    "<???>",              //    //                =0x80                                                                           
    "<???>",              //    //                =0x81                                                                           
    "<???>",              //    //                =0x82                                                                           
    "<???>",              //    //                =0x83                                                                           
    "<???>",              //    //                =0x84                                                                           
    "<???>",              //    //                =0x85                                                                           
    "<???>",              //    //                =0x86                                                                           
    "<???>",              //    //                =0x87                                                                           
    "<???>",              //    //                =0x88                                                                           
    "<???>",              //    //                =0x89                                                                           
    "<???>",              //    //                =0x8A                                                                           
    "<???>",              //    //                =0x8B                                                                           
    "<???>",              //    //                =0x8C                                                                           
    "Num =",              //    EKC_NUMPAD_EQUALS =0x8D,    /* = on numeric keypad (NEC PC98) */                                  
    "<???>",              //    //                =0x8E                                                                           
    "<???>",              //    //                =0x8F                                                                           
    "Prev track",         //    EKC_PREV_TRACK    =0x90,    /* Previous Track (KC_CIRCUMFLEX on Japanese keyboard) */             
    "AT",                 //    EKC_AT            =0x91,    /*                     (NEC PC98) */                                  
    "Colon",              //    EKC_COLON         =0x92,    /*                     (NEC PC98) */                                  
    "Underline",          //    EKC_UNDERLINE     =0x93,    /*                     (NEC PC98) */                                  
    "Kanji",              //    EKC_KANJI         =0x94,    /* (Japanese keyboard)            */                                  
    "Stop",               //    EKC_STOP          =0x95,    /*                     (NEC PC98) */                                  
    "AX",                 //    EKC_AX            =0x96,    /*                     (Japan AX) */                                  
    "Unlabeled",          //    EKC_UNLABELED     =0x97,    /*                        (J3100) */                                  
    "<???>",              //    //                =0x98                                                                           
    "Next track",         //    EKC_NEXT_TRACK    =0x99,    /* Next Track */                                                      
    "<???>",              //    //                =0x9A                                                                           
    "<???>",              //    //                =0x9B                                                                           
    "Num Enter",          //    EKC_NUMPAD_ENTER  =0x9C,    /* Enter on numeric keypad */                                         
    "RCtrl",              //    EKC_RCONTROL      =0x9D,                                                                          
    "<???>",              //    //                =0x9E                                                                           
    "<???>",              //    //                =0x9F                                                                           
    "Mute",               //    EKC_MUTE          =0xA0,    /* Mute */                                                            
    "Calculator",         //    EKC_CALCULATOR    =0xA1,    /* Calculator */                                                      
    "Play/pause",         //    EKC_PLAY_PAUSE    =0xA2,    /* Play / Pause */                                                    
    "<???>",              //    //                =0xA3                                                                           
    "Media stop",         //    EKC_MEDIA_STOP    =0xA4,    /* Media Stop */                                                      
    "<???>",              //    //                =0xA5                                                                           
    "<???>",              //    //                =0xA6                                                                           
    "<???>",              //    //                =0xA7                                                                           
    "<???>",              //    //                =0xA8                                                                           
    "<???>",              //    //                =0xA9                                                                           
    "<???>",              //    //                =0xAA                                                                           
    "<???>",              //    //                =0xAB                                                                           
    "<???>",              //    //                =0xAC                                                                           
    "<???>",              //    //                =0xAD                                                                           
    "Vol -",              //    EKC_VOLUME_DOWN   =0xAE,    /* Volume - */                                                        
    "<???>",              //    //                =0xAF                                                                           
    "Vol +",              //    EKC_VOLUME_UP     =0xB0,    /* Volume + */                                                        
    "<???>",              //    //                =0xB1                                                                           
    "Web home",           //    EKC_WEB_HOME      =0xB2,    /* Web home */                                                        
    "Num ,",              //    EKC_NUMPAD_COMMA  =0xB3,    /* , on numeric keypad (NEC PC98) */                                  
    "<???>",              //    //                =0xB4                                                                           
    "Divide",             //    EKC_DIVIDE        =0xB5,    /* / on numeric keypad */                                             
    "<???>",              //    //                =0xB6                                                                           
    "SysRq",              //    EKC_SYS_RQ        =0xB7,                                                                          
    "RAlt",               //    EKC_RALT          =0xB8,    /* right Alt */                                                       
    "<???>",              //    //                =0xB9                                                                           
    "<???>",              //    //                =0xBA                                                                           
    "<???>",              //    //                =0xBB                                                                           
    "<???>",              //    //                =0xBC                                                                           
    "<???>",              //    //                =0xBD                                                                           
    "<???>",              //    //                =0xBE                                                                           
    "<???>",              //    //                =0xBF                                                                           
    "<???>",              //    //                =0xC0                                                                           
    "<???>",              //    //                =0xC1                                                                           
    "<???>",              //    //                =0xC2                                                                           
    "<???>",              //    //                =0xC3                                                                           
    "<???>",              //    //                =0xC4                                                                           
    "Pause",              //    EKC_PAUSE         =0xC5,    /* Pause */                                                           
    "<???>",              //    //                =0xC6                                                                           
    "Home",               //    EKC_HOME          =0xC7,    /* Home on arrow keypad */                                            
    "Up",                 //    EKC_UP            =0xC8,    /* UpArrow on arrow keypad */                                         
    "PgUp",               //    EKC_PAGE_UP       =0xC9,    /* PgUp on arrow keypad */                                            
    "<???>",              //    //                =0xCA                                                                           
    "Left",               //    EKC_LEFT          =0xCB,    /* LeftArrow on arrow keypad */                                       
    "<???>",              //    //                =0xCC                                                                           
    "Right",              //    EKC_RIGHT         =0xCD,    /* RightArrow on arrow keypad */                                      
    "<???>",              //    //                =0xCE                                                                           
    "End",                //    EKC_END           =0xCF,    /* End on arrow keypad */                                             
    "Down",               //    EKC_DOWN          =0xD0,    /* DownArrow on arrow keypad */                                       
    "PgDn",               //    EKC_PAGE_DOWN     =0xD1,    /* PgDn on arrow keypad */                                            
    "Ins",                //    EKC_INSERT        =0xD2,    /* Insert on arrow keypad */                                          
    "Del",                //    EKC_DELETE        =0xD3,    /* Delete on arrow keypad */                                          
    "<???>",              //    //                =0xD4                                                                           
    "<???>",              //    //                =0xD5                                                                           
    "<???>",              //    //                =0xD6                                                                           
    "<???>",              //    //                =0xD7                                                                           
    "<???>",              //    //                =0xD8                                                                           
    "<???>",              //    //                =0xD9                                                                           
    "<???>",              //    //                =0xDA                                                                           
    "LWin",               //    EKC_LWIN          =0xDB,    /* Left Windows key */                                                
    "RWin",               //    EKC_RWIN          =0xDC,    /* Right Windows key */                                               
    "App menu",           //    EKC_APP           =0xDD,    /* AppMenu key */                                                     
    "Power",              //    EKC_POWER         =0xDE,    /* System Power */                                                    
    "Sleep",              //    EKC_SLEEP         =0xDF,    /* System Sleep */                                                    
    "<???>",              //    //                =0xE0                                                                           
    "<???>",              //    //                =0xE1                                                                           
    "<???>",              //    //                =0xE2                                                                           
    "Wake",               //    EKC_WAKE          =0xE3,    /* System Wake */                                                     
    "<???>",              //    //                =0xE4                                                                           
    "Web search",         //    EKC_WEB_SEARCH    =0xE5,    /* Web Search */                                                      
    "Web favor.",         //    EKC_WEB_FAVORITES =0xE6,    /* Web Favorites */                                                   
    "Web refresh",        //    EKC_WEB_REFRESH   =0xE7,    /* Web Refresh */                                                     
    "Web stop",           //    EKC_WEB_STOP      =0xE8,    /* Web Stop */                                                        
    "Web forw.",          //    EKC_WEB_FORWARD   =0xE9,    /* Web Forward */                                                     
    "Web back",           //    EKC_WEB_BACK      =0xEA,    /* Web Back */                                                        
    "My comp.",           //    EKC_MY_COMPUTER   =0xEB,    /* My Computer */                                                     
    "Mail",               //    EKC_MAIL          =0xEC,    /* Mail */                                                            
    "Media sel.",         //    EKC_MEDIA_SELECT  =0xED     /* Media Select */                                                    
    "<???>",              //    //                =0xEE                                                                           
    "<???>",              //    //                =0xEF                                                                           
    "<???>",              //    //                =0xF0                                                                           
    "<???>",              //    //                =0xF1                                                                           
    "<???>",              //    //                =0xF2                                                                           
    "<???>",              //    //                =0xF3                                                                           
    "<???>",              //    //                =0xF4                                                                           
    "<???>",              //    //                =0xF5                                                                           
    "<???>",              //    //                =0xF6                                                                           
    "<???>",              //    //                =0xF7                                                                           
    "<???>",              //    //                =0xF8                                                                           
    "<???>",              //    //                =0xF9                                                                           
    "<???>",              //    //                =0xFA                                                                           
    "<???>",              //    //                =0xFB                                                                           
    "<???>",              //    //                =0xFC                                                                           
    "<???>",              //    //                =0xFD                                                                           
    "<???>"               //    //                =0xFE                                                                          
};

//---------------------------------------------------------------------------

enum E_MOUSE_CODE
{
    EMC_LMOUSE           = 0x00,  // Left mouse button
    EMC_RMOUSE           = 0x01,  // Right mouse button
    EMC_MMOUSE           = 0x02,  // Middle mouse button
    EMC_MWHEEL_UP        = 0x03,  // Mouse wheel up 
    EMC_MWHEEL_DOWN      = 0x04,  // Mouse wheel down 

    E_MOUSE_CODE_COUNT,

    E_MOUSE_CODE_FORCE_32_BIT = 0xFFFFFFFF
};

//----------------------------------------------------------------------------

static const c8* MouseCodeNameStr[E_MOUSE_CODE_COUNT] = 
{  
    "LMouse",           // Left mouse button    
    "RMouse",           // Right mouse button  
    "MMouse",           // Middle mouse button 
    "MWeelUp",          // Mouse wheel up       
    "MWeelDn"           // Mouse wheel down    
};

//----------------------------------------------------------------------------

//! Enumeration for all event types there are.
enum EEVENT_TYPE
{
    //! A mouse input event.
    EET_MOUSE_INPUT_EVENT = 0,

    //! A key input evant.
    EET_KEY_INPUT_EVENT,

	//! Windows Paint evant.
    EET_PAINT_EVENT,

    //! A log event
    EET_LOG_TEXT_EVENT,

    EEVENT_TYPE_COUNT,

    EEVENT_TYPE_FORCE_32_BIT = 0xFFFFFFFF
};

//---------------------------------------------------------------------------

//! Enumeration for all key input events
enum E_KEY_INPUT_EVENT
{
    //! Key was pressed down.
    EKIE_KEY_PRESSED_UP = 0,

    //! Key was pressed down.
    EKIE_KEY_PRESSED_DOWN,

    E_KEY_INPUT_EVENT_COUNT,

    E_KEY_INPUT_EVENT_FORCE_32_BIT = 0xFFFFFFFF
};

//---------------------------------------------------------------------------

//! Enumeration for all mouse input events
enum E_MOUSE_INPUT_EVENT
{
    //! Left mouse button was pressed down.
    EMIE_LMOUSE_PRESSED_DOWN = 0,

    //! Right mouse button was pressed down.
    EMIE_RMOUSE_PRESSED_DOWN,

    //! Middle mouse button was pressed down.
    EMIE_MMOUSE_PRESSED_DOWN,

    //! Left mouse button was left up.
    EMIE_LMOUSE_LEFT_UP,

    //! Right mouse button was left up.
    EMIE_RMOUSE_LEFT_UP,

    //! Middle mouse button was left up.
    EMIE_MMOUSE_LEFT_UP,

    //! The mouse cursor changed its position.
    EMIE_MOUSE_MOVED,

    //! The mouse wheel was moved. Use Wheel value in event data to find out 
    //! in what direction and how fast.
    EMIE_MOUSE_WHEEL, 

    E_MOUSE_INPUT_EVENT_COUNT,

    E_MOUSE_INPUT_EVENT_FORCE_32_BIT = 0xFFFFFFFF
};

//----------------------------------------------------------------------------

// pre defined cursors
enum E_CURSOR_TYPE
{
	ECT_APPSTARTING = 0,// Standard arrow and small hourglass
	ECT_ARROW,			// Standard arrow
	ECT_CROSS,			// Crosshair
	ECT_HAND,			// Hand
	ECT_HELP,			// Arrow and question mark
	ECT_IBEAM,			// Text I-beam
	ECT_NO,				// Slashed circle
	ECT_SIZEALL,		// Four-pointed arrow pointing north, south, east, and west
	ECT_SIZENESW,		// Double-pointed arrow pointing northeast and southwest
	ECT_SIZENS,			// Double-pointed arrow pointing north and south
	ECT_SIZENWSE,		// Double-pointed arrow pointing northwest and southeast
	ECT_SIZEWE,			// Double-pointed arrow pointing west and east
	ECT_UPARROW,		// Vertical arrow
	ECT_WAIT,			// Hourglass	

	E_CURSOR_TYPE_COUNT,

    E_CURSOR_TYPE_FORCE_32_BIT = 0xFFFFFFFF
};

//----------------------------------------------------------------------------

static const c8* CursorTypeName[E_CURSOR_TYPE_COUNT] =
{
	"ECT_APPSTARTING",
	"ECT_ARROW",
	"ECT_CROSS",
	"ECT_HAND",
	"ECT_HELP",
	"ECT_IBEAM",
	"ECT_NO",
	"ECT_SIZEALL",
	"ECT_SIZENESW",
	"ECT_SIZENS",
	"ECT_SIZENWSE",
	"ECT_SIZEWE",
	"ECT_UPARROW",
	"ECT_WAIT",
};

//----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_ioHPP