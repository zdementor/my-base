//|-------------------------------------------------------------------------
//| File:        ILogger.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ILoggerHPP
#define ILoggerHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//---------------------------------------------------------------------------
namespace my {
namespace io {
//---------------------------------------------------------------------------

//! Interface for logging messages, warnings and errors
class __MY_IO_LIB_API__ ILogger : public IUnknown, public ISingleton<ILogger>
{
public:

    //! Returns the current set log level.
    virtual E_LOG_LEVEL getLogLevel() = 0;

    //! Sets a new log level. With this value, texts which are sent to
    //! the logger are filtered out. For example setting this value to
    //! io::ELL_WARNING, only warnings and
    //! errors are printed out. Setting it to io::ELL_INFORMATION, which is
    //! the default setting, warnings,
    //! errors and informational texts are printed out.
    //! \param ll: new log level filter value.
    virtual void setLogLevel(E_LOG_LEVEL ll) = 0;

    //! Prints out a text into the log
    //! \param text: Text to print out.
    //! \param ll: Log level of the text. If the text is an error, set
    //! it to io::ELL_ERROR, if it is warning set it to io::ELL_WARNING, and if it
    //! is just an informational text, set it to io::ELL_INFORMATION. Texts are
    //! filtered with these levels. If you want to be a text displayed,
    //! independent on what level filter is set, use ELL_NONE.
    virtual void log(const c8* text, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

    //! Prints out a text into the log
    //! \param text: Text to print out.
    //! \param hint: Additional info. This string is added after a " :" to the
    //! string.
    //! \param ll: Log level of the text. If the text is an error, set
    //! it to io::ELL_ERROR, if it is warning set it to io::ELL_WARNING, and if it
    //! is just an informational text, set it to io::ELL_INFORMATION. Texts are
    //! filtered with these levels. If you want to be a text displayed,
    //! independent on what level filter is set, use ELL_NONE.
    virtual void log(const c8* text, const c8* hint, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

    //! Prints out a text into the log
    //! \param text: Text to print out.
    //! \param ll: Log level of the text. If the text is an error, set
    //! it to io::ELL_ERROR, if it is warning set it to io::ELL_WARNING, and if it
    //! is just an informational text, set it to io::ELL_INFORMATION. Texts are
    //! filtered with these levels. If you want to be a text displayed,
    //! independent on what level filter is set, use ELL_NONE.
    virtual void log(const wchar_t* text, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

    //! Prints out a text into the log
    //! \param text: Text to print out.
    //! \param hint: Additional info. This string is added after a " :" to the
    //! string.
    //! \param ll: Log level of the text. If the text is an error, set
    //! it to io::ELL_ERROR, if it is warning set it to io::ELL_WARNING, and if it
    //! is just an informational text, set it to io::ELL_INFORMATION. Texts are
    //! filtered with these levels. If you want to be a text displayed,
    //! independent on what level filter is set, use ELL_NONE.
    virtual void log(const wchar_t* text, const wchar_t* hint, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

	//! Prints out a text into the log
    virtual void log(const core::stringc &text, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

    //! Prints out a text into the log
    virtual void log(const core::stringc &text, const core::stringc &hint, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

	//! Prints out a text into the log
    virtual void log(const core::stringw &text, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

    //! Prints out a text into the log
    virtual void log(const core::stringw &text, const core::stringw &hint, E_LOG_LEVEL ll=ELL_INFORMATION) = 0;

    //! Prints out a text into the log, without log level control
	virtual void logAnyWay(const c8* text) = 0;
    virtual void logAnyWay(const wchar_t* text) = 0;
    virtual void logAnyWay(const core::stringc &text) = 0;
    virtual void logAnyWay(const core::stringw &text) = 0;

	virtual void log(io::E_LOG_LEVEL ll, const c8 *format, ...) = 0;
	virtual void logErr(const c8 *format, ...) = 0;
	virtual void logWarn(const c8 *format, ...) = 0;
	virtual void logInfo(const c8 *format, ...) = 0;

	virtual void log(io::E_LOG_LEVEL ll, const wchar_t *format, ...) = 0;
	virtual void logErr(const wchar_t *format, ...) = 0;
	virtual void logWarn(const wchar_t *format, ...) = 0;
	virtual void logInfo(const wchar_t *format, ...) = 0;

    //! Sets a user event receiver
    virtual void setEventReceiver(IEventReceiver* r) = 0;

	//! Making idntity output format level
	virtual void setIdentityFormatLevel() = 0;

	virtual void setFormatLevel(u32 level) = 0;
	virtual u32 getFormatLevel() = 0;

	//! Increasing output format level
	virtual void increaseFormatLevel() = 0;

	//! Decreasing output format level
	virtual void decreaseFormatLevel() = 0;

	//! setting callback, needed to perform on log event handling
	virtual void setOnLogCallback( void (*log_callback_ptr)(const c8* text, E_LOG_LEVEL ll) ) = 0;

	//! turning on/off 'caret return' on message log
	virtual void setCaretReturnEnabled(bool value) = 0;

	//! getting on/off 'caret return' criteria
	virtual bool isCaretReturnEnabled() = 0;

	//! return log file name
    virtual const c8* getLogFileName() const = 0;

	//!	Return singleton object
	static ILogger& getSingleton();

	//! Return pointer to singleton object
	static ILogger* getSingletonPtr();
};

//---------------------------------------------------------------------------

__MY_IO_LIB_API__ ILogger* createLogger(IEventReceiver* r);

//---------------------------------------------------------------------------
} // end namespace io
} // end namespace my
//---------------------------------------------------------------------------
#endif

