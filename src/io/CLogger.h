//|-------------------------------------------------------------------------
//| File:        CLogger.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CLoggerHPP
#define CLoggerHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <io/ILogger.h>
#include <io/IEventReceiver.h>

//-----------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! Interface for logging messages, warnings and errors
class CLogger : public ILogger
{
public:

    CLogger(IEventReceiver* r);
	virtual ~CLogger();

	virtual E_LOG_LEVEL getLogLevel() { return m_LogLevel; }
	virtual void setLogLevel(E_LOG_LEVEL ll) { m_LogLevel = ll; }

    virtual void log(const c8* text, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const c8* text, const c8* hint, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const wchar_t* text, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const wchar_t* text, const wchar_t* hint, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const core::stringc &text, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const core::stringc &text, const core::stringc &hint, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const core::stringw &text, E_LOG_LEVEL ll=ELL_INFORMATION);
    virtual void log(const core::stringw &text, const core::stringw &hint, E_LOG_LEVEL ll=ELL_INFORMATION);

	virtual void logAnyWay(const c8* text);
    virtual void logAnyWay(const wchar_t* text);
    virtual void logAnyWay(const core::stringc &text);
    virtual void logAnyWay(const core::stringw &text);

	virtual void log(io::E_LOG_LEVEL ll, const c8 *format, ...);
	virtual void logErr(const c8 *format, ...);
	virtual void logWarn(const c8 *format, ...);
	virtual void logInfo(const c8 *format, ...);

	virtual void log(io::E_LOG_LEVEL ll, const wchar_t *format, ...);
	virtual void logErr(const wchar_t *format, ...);
	virtual void logWarn(const wchar_t *format, ...);
	virtual void logInfo(const wchar_t *format, ...);

    virtual void setEventReceiver(IEventReceiver* r);

	virtual void setIdentityFormatLevel();

	virtual void setFormatLevel(u32 level);
	virtual u32 getFormatLevel();

	virtual void increaseFormatLevel();
	virtual void decreaseFormatLevel();

	virtual void setOnLogCallback(void (*log_callback_ptr)(const c8* text, E_LOG_LEVEL ll));

	virtual void setCaretReturnEnabled(bool value);

	virtual bool isCaretReturnEnabled();

    virtual const c8* getLogFileName() const
	{ return m_LogFileName.c_str(); }

private:

	void _logAnyWay(const c8* text, E_LOG_LEVEL ll);
	void _logAnyWay(const wchar_t* text, E_LOG_LEVEL ll);

	void _print(const c8* message);

    E_LOG_LEVEL m_LogLevel;
    IEventReceiver *m_Receiver;

	u32 m_FormatLevel;

	void (*m_OnLogCallback)(const c8* text, E_LOG_LEVEL ll);

	core::stringc m_LogFileName;

	bool m_CaretReturnEnabled;

	s32 m_LoggingRecursion;

	core::stringc m_MsgString;
	io::IWriteFile *m_LogFile;
};

//-----------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//-----------------------------------------------------------------------------

#endif

