#ifndef _MyCEGUILogger_h_
#define _MyCEGUILogger_h_
//------------------------------------------------------------------------------------

#include <CEGUIDefaultLogger.h>

//------------------------------------------------------------------------------------
namespace CEGUI {
//------------------------------------------------------------------------------------

class MyCEGUILogger : public DefaultLogger
{
public:
	MyCEGUILogger(void);
	virtual ~MyCEGUILogger(void);

	virtual void logEvent(const String& message, LoggingLevel level = Standard);
    virtual void setLogFilename(const String& filename, bool append = false);

private:
	std::ostringstream d_logstream;

	void _logEvent(const String& message, LoggingLevel level = Standard);
};

//------------------------------------------------------------------------------------
} // namespace CEGUI
//------------------------------------------------------------------------------------

#endif	// #ifndef _MyCEGUILogger_h_
