
//==============================================================================
//
//   log.h - exported_h
//
//==============================================================================
//  cybercastor - made in quebec 2020 <cybercastor@icloud.com>
//==============================================================================


#ifndef __CORELIB_LOG_H__
#define __CORELIB_LOG_H__


#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <ctime>



const std::string CONSOLE_COLOR_BKGRND_WHITE_BLUE = "\u001b[37;44;1m > ";
const std::string CONSOLE_COLOR_BKGRND_WHITE_GREEN = "\u001b[32;44;1m > ";
const std::string CONSOLE_COLOR_BKGRND_YELLOW = "\u001b[33;41;1m > ";
const std::string CONSOLE_COLOR_RED_BRIGHT = "\u001b[31;1m";
const std::string CONSOLE_COLOR_GREEN_BRIGHT = "\u001b[32;1m";
const std::string CONSOLE_COLOR_YELLOW_BRIGHT = "\u001b[33;1m";
const std::string CONSOLE_COLOR_MAGENTA_BRIGHT = "\u001b[35;1m";
const std::string CONSOLE_COLOR_CYAN_BRIGHT = "\u001b[36;1m";
const std::string CONSOLE_COLOR_BLUE = "\u001b[34m";
const std::string CONSOLE_COLOR_MAGENTA = "\u001b[35m";
const std::string CONSOLE_COLOR_CYAN = "\u001b[36m";
const std::string CONSOLE_COLOR_RED = "\u001b[31m";
const std::string CONSOLE_COLOR_GREEN = "\u001b[32m";
const std::string CONSOLE_COLOR_YELLOW = "\u001b[33m";

void __cdecl ConsoleDemo();
void __cdecl ConsoleOut( std::string color, const char *format, ...);
void __cdecl SystemDebugOutput(const wchar_t *channel, const char *format, ...);
void __cdecl ConsoleLog(const char *format, ...);
void __cdecl ConsoleError(const char *format, ...);
void __cdecl ConsoleWarning(const char* format, ...);
void __cdecl ConsoleTitle(const char* title);
void __cdecl ConsoleCmdOption(const char* name, std::string  value);
void __cdecl ConsoleCmdOptionH(const char* name, std::string  value);
void __cdecl ConsoleCmdOptionW(const char* name, std::wstring  value);




# define COUTCMD( ... )		{ ConsoleLog(  __VA_ARGS__ );  }	
# define CTITLE( t )	{ ConsoleTitle(  t );  }	
# define COPTION( n,v)	{ ConsoleCmdOption(  n, v );  }	
# define COPTIONW( n,v)	{ ConsoleCmdOptionW(  n, v );  }	
# define COPTIONH( n,v)	{ ConsoleCmdOptionH(  n, v );  }	
# define COUTWARN( ... )	{ ConsoleWarning(  __VA_ARGS__ );  }	
# define COUTERR( ... )		{ ConsoleError(  __VA_ARGS__ );  }	

# define COUTRS( ... ) { ConsoleOut(CONSOLE_COLOR_RED,  __VA_ARGS__ );  }	
# define COUTR( ... ) { ConsoleOut(CONSOLE_COLOR_RED_BRIGHT,  __VA_ARGS__ );  }	
# define COUTG( ... ) { ConsoleOut(CONSOLE_COLOR_GREEN_BRIGHT,  __VA_ARGS__ );  }	
# define COUTY( ... ) { ConsoleOut(CONSOLE_COLOR_YELLOW_BRIGHT,  __VA_ARGS__ );  }	
# define COUTM( ... ) { ConsoleOut(CONSOLE_COLOR_MAGENTA_BRIGHT,  __VA_ARGS__ );  }	
# define COUTC( ... ) { ConsoleOut(CONSOLE_COLOR_CYAN_BRIGHT,  __VA_ARGS__ );  }	
# define COUTCS( ... ) { ConsoleOut(CONSOLE_COLOR_CYAN,  __VA_ARGS__ );  }

# define LOG_INSTALL( ... )		{ ConsoleLogInstall(  __VA_ARGS__ );  }	
# define LOG_CONSOLE( color, ... ) { ConsoleOut( color, __VA_ARGS__ );  }	

#ifdef _DEBUG
		# define LOG_PROFILE( channel, ... )
		# define LOG_TRACE( channel, ... ) { SystemDebugOutput( TEXT("trace: "),  __VA_ARGS__ );  }
		# define LOG_INFO( channel, ... ) {  SystemDebugOutput( TEXT("info: "),  __VA_ARGS__ );  }
		# define LOG_WARNING( channel, ... ){  SystemDebugOutput( TEXT("warning: "),  __VA_ARGS__ );  }
		# define LOG_ERROR( channel, ... ) {  SystemDebugOutput( TEXT("error: "),  __VA_ARGS__ ); }
#else
		# define LOG_PROFILE( channel, ... ) 
		# define LOG_TRACE( channel, ... )
		# define LOG_INFO( channel, ... )
		# define LOG_WARNING( channel, ... )
		# define LOG_ERROR( channel, ... ) 
#endif


#endif //__CORELIB_LOG_H__

