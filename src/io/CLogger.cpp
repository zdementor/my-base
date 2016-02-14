//|-------------------------------------------------------------------------
//| File:        CLogger.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CLogger.h"
#include "CWriteFile.h"
#include <io/IFileSystem.h>
#include <os/os.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

CLogger::CLogger(IEventReceiver* r)
    : m_LogLevel(ELL_INFORMATION), m_Receiver(r), m_FormatLevel(0),
m_OnLogCallback(0), m_CaretReturnEnabled(true), m_LoggingRecursion(0)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CLogger");
#endif

	if (!FILE_SYSTEM_PTR)
		io::createFileSystem();
	else
		FILE_SYSTEM.grab();

	core::stringc file_name = os::getCurrentProcessName();
	core::changeFileExt(file_name, "log");

	m_LogFileName.sprintf("%s\\%s",
		FILE_SYSTEM.getWorkingDirectory(), file_name.c_str());

	m_LogFile = new io::CWriteFile(m_LogFileName.c_str(), false);

	logInfo("Logger created.");
}

//---------------------------------------------------------------------------

CLogger::~CLogger()
{
	SAFE_DROP(m_LogFile);
	FILE_SYSTEM.drop();
	logInfo("Logger destroyed.");
}

//---------------------------------------------------------------------------

void CLogger::log(const c8* text, E_LOG_LEVEL ll)
{
    if (ll < m_LogLevel)
        return;
	_logAnyWay(text, ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const c8* text, const c8* hint, E_LOG_LEVEL ll)
{
    if (ll < m_LogLevel)
        return;
    core::stringc s = text;
    s += ": ";
    s += hint;
    log(s.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const wchar_t* text, E_LOG_LEVEL ll)
{
    if (ll < m_LogLevel)
        return;
    core::stringc s = text;
    log(s.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const wchar_t* text, const wchar_t* hint, E_LOG_LEVEL ll)
{
    if (ll < m_LogLevel)
        return;
    core::stringc s1 = text;
    core::stringc s2 = hint;
    log(s1.c_str(), s2.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const core::stringc &text, E_LOG_LEVEL ll)
{
    if (ll < m_LogLevel)
        return;
    log(text.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const core::stringc &text, const core::stringc &hint, E_LOG_LEVEL ll)
{
	if (ll < m_LogLevel)
        return;
    log(text.c_str(), hint.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const core::stringw &text, E_LOG_LEVEL ll)
{
	if (ll < m_LogLevel)
        return;
    log(text.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::log(const core::stringw &text, const core::stringw &hint, E_LOG_LEVEL ll)
{
	if (ll < m_LogLevel)
        return;
    log(text.c_str(), hint.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::logAnyWay(const c8* text)
{
	_logAnyWay(text, io::ELL_INFORMATION);
}

//---------------------------------------------------------------------------

void CLogger::logAnyWay(const wchar_t* text)
{
	static core::stringc msg;
	msg = text;
	logAnyWay(msg.c_str());
}

//---------------------------------------------------------------------------

void CLogger::logAnyWay(const core::stringc &text)
{
	logAnyWay(text.c_str());
}

//---------------------------------------------------------------------------

void CLogger::logAnyWay(const core::stringw &text)
{
	logAnyWay(text.c_str());
}

//---------------------------------------------------------------------------

void CLogger::_logAnyWay(const wchar_t* text, E_LOG_LEVEL ll)
{
	static core::stringc s;
	s = text;
	_logAnyWay(s.c_str(), ll);
}

//---------------------------------------------------------------------------

void CLogger::_logAnyWay(const c8* text, E_LOG_LEVEL ll)
{
	m_LoggingRecursion++;

	static core::stringc s;
	s = "";
	for (u32 i=0; i < m_FormatLevel && ll == ELL_INFORMATION; i++)
		s += " ";	
	if (ll == ELL_ERROR)
		s += "Error: ";
	else if (ll == ELL_WARNING)
		s += "Warning: ";
	s += text;

    if (m_Receiver && m_LoggingRecursion == 1)
    {
		SEvent event;
		event.EventType = EET_LOG_TEXT_EVENT;
		event.LogEvent.Text = s.c_str();
		event.LogEvent.Level = ll;

		m_Receiver->OnEvent(event);
    }
	if (m_OnLogCallback)
		m_OnLogCallback(s.c_str(), ll);
    _print(s.c_str());

	m_LoggingRecursion--;
}

//---------------------------------------------------------------------------

#define LOGC(ll, fmt) do {	\
	if (ll < m_LogLevel)	\
		break;				\
	static c8 buf[16384];	\
	va_list ap;				\
	va_start(ap, fmt);		\
	s32 resval = _vsnprintf(buf, sizeof(buf)/sizeof(*buf)-1, fmt, ap);	\
	buf[sizeof(buf)/sizeof(*buf)-1] = 0;								\
	_logAnyWay(buf, ll);												\
	va_end(ap);															\
} while (0)

#define LOGW(ll, fmt) do {		\
	if (ll < m_LogLevel)		\
		break;					\
	static wchar_t buf[16384];	\
	va_list ap;					\
	va_start(ap, fmt);			\
	s32 resval = _vsnwprintf(buf, sizeof(buf)/sizeof(*buf)-1, fmt, ap);	\
	buf[sizeof(buf)/sizeof(*buf)-1] = 0;								\
	_logAnyWay(buf, ll);												\
	va_end(ap);															\
} while (0)

//---------------------------------------------------------------------------

void CLogger::log(io::E_LOG_LEVEL ll, const c8 *format, ...)
{ LOGC(ll, format); }

//---------------------------------------------------------------------------

void CLogger::logErr(const c8 *format, ...)
{ LOGC(ELL_ERROR, format); }

//---------------------------------------------------------------------------

void CLogger::logWarn(const c8 *format, ...)
{ LOGC(ELL_WARNING, format); }

//---------------------------------------------------------------------------

void CLogger::logInfo(const c8 *format, ...)
{ LOGC(ELL_INFORMATION, format); }

//---------------------------------------------------------------------------

void CLogger::log(io::E_LOG_LEVEL ll, const wchar_t *format, ...)
{ LOGW(ll, format); }

//---------------------------------------------------------------------------

void CLogger::logErr(const wchar_t *format, ...)
{ LOGW(ELL_ERROR, format); }

//---------------------------------------------------------------------------

void CLogger::logWarn(const wchar_t *format, ...)
{ LOGW(ELL_WARNING, format); }

//---------------------------------------------------------------------------

void CLogger::logInfo(const wchar_t *format, ...)
{ LOGW(ELL_INFORMATION, format); }

//---------------------------------------------------------------------------

void CLogger::setEventReceiver(IEventReceiver* r)
{
    m_Receiver = r;
}

//-----------------------------------------------------------------------------

void CLogger::setIdentityFormatLevel()
{
	m_FormatLevel = 0;
}

//-----------------------------------------------------------------------------

void CLogger::setFormatLevel(u32 level)
{
	m_FormatLevel = level;
}

//-----------------------------------------------------------------------------

u32 CLogger::getFormatLevel()
{
	return m_FormatLevel;
}

//-----------------------------------------------------------------------------

void CLogger::increaseFormatLevel()
{
	m_FormatLevel++;
}

//-----------------------------------------------------------------------------

void CLogger::decreaseFormatLevel()
{
	if (m_FormatLevel > 0)
		m_FormatLevel--;
}

//-----------------------------------------------------------------------------

void CLogger::setOnLogCallback(
	void (*log_callback_ptr)(const c8* text, E_LOG_LEVEL ll))
{
	m_OnLogCallback = log_callback_ptr;
}

//-----------------------------------------------------------------------------

void CLogger::setCaretReturnEnabled(bool value)
{
	m_CaretReturnEnabled = value;
}

//-----------------------------------------------------------------------------

bool CLogger::isCaretReturnEnabled()
{
	return m_CaretReturnEnabled;
}

//-----------------------------------------------------------------------------

#if MY_PLATFORM == MY_PLATFORM_WIN32 // Windows specific implementation

//-----------------------------------------------------------------------------
#include <windows.h>
#include <io.h>
//-----------------------------------------------------------------------------

void CLogger::_print(const c8* message)
{
	m_MsgString.sprintf("%s%c", message, m_CaretReturnEnabled ? '\n' : '0');

	// log debug output
	OutputDebugString(m_MsgString.c_str());

	// log text console
	printf(m_MsgString.c_str());

	if (m_LogFile)
		m_LogFile->write(m_MsgString.c_str(), m_MsgString.size(), true);    
}

//-----------------------------------------------------------------------------

#else // linux specific implementation

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

//-----------------------------------------------------------------------------

//! prints a debuginfo string
void CLogger::_print(const c8* message)
{
	if (m_CaretReturnEnabled)
		printf("%s\n", message);
	else
		printf("%s", message);
}

//---------------------------------------------------------------------------

#endif // #ifdef WIN32

//-----------------------------------------------------------------------------

__MY_EXPORT__ ILogger* createLogger(IEventReceiver* r)
{
	return new CLogger(r);
}

//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------
