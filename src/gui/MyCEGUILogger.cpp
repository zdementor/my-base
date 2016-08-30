
#include "MyCEGUILogger.h"
#include <iomanip>
#include <io/ILogger.h>

//------------------------------------------------------------------------------------
namespace CEGUI {
//------------------------------------------------------------------------------------

MyCEGUILogger::MyCEGUILogger(bool mirrorLogToMyLog)
	: m_MirrorLogToMyLog(mirrorLogToMyLog)
{
	LOGGER.logInfo("My CEGUI Logger created.");
}

//------------------------------------------------------------------------------------

MyCEGUILogger::~MyCEGUILogger(void)
{
	LOGGER.logInfo("My CEGUI Logger destroyed.");
}

//------------------------------------------------------------------------------------

void MyCEGUILogger::logEvent(const String& message, LoggingLevel level)
{
	if (m_MirrorLogToMyLog)
		_logEvent(message, level);
	DefaultLogger::logEvent(message, level);
}

//------------------------------------------------------------------------------------

void MyCEGUILogger::_logEvent(const String& message, LoggingLevel level)
{
    using namespace std;

	my::io::E_LOG_LEVEL llevel = my::io::ELL_INFORMATION;

    // clear sting stream
	d_logstream.str("");
	d_logstream << "<CEGUI> ";

    // write event type code
    switch(level)
    {
    case Errors:
        d_logstream << "(Error)\t";
		llevel = my::io::ELL_ERROR;
        break;
    case Warnings:
        d_logstream << "(Warn)\t";
		llevel = my::io::ELL_WARNING;
        break;
    case Standard:
        d_logstream << "(Std) \t";
        break;
    case Informative:
        d_logstream << "(Info) \t";
        break;
    case Insane:
        d_logstream << "(Insan)\t";
        break;
    default:
        d_logstream << "(Unkwn)\t";
        break;
    }

    d_logstream << message;

	LOGGER.log(d_logstream.str().c_str(), llevel);
}

//------------------------------------------------------------------------------------

void MyCEGUILogger::setLogFilename(const String& filename, bool append)
{
	LOGGER.logInfo("Setup CEGUI Log file '%s'", filename.c_str());

	DefaultLogger::setLogFilename(filename, append);
}

//------------------------------------------------------------------------------------
} // namespace CEGUI
//------------------------------------------------------------------------------------
