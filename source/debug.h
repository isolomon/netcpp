#ifndef LIB_DEBUG_H
#define LIB_DEBUG_H

#include <string>

void dumpData     (const void*   data, int size);
void dumpFloat    (const float*  data, int count);
void dumpDouble   (const double* data, int count);

void setLogFile   (const char* file);
void setLogFile   (const std::string& file);

void setLogLevel  (int level);
void setLogFlags  (int flags);

void clearLogs    ();

void writelog     (const void* data, int size);
void writelog     (const char* data);
int  logLevel     ();

void logmsg       (const char* fmt, ...);
void log_verb     (const char* fmt, ...);
void log_info     (const char* fmt, ...);
void log_warn     (const char* fmt, ...);
void log_error    (const char* fmt, ...);

enum LogLevel
{
    LOG_LEVEL_OFF       = 0,
    LOG_LEVEL_ERROR     = 1,
    LOG_LEVEL_WARN      = 2,
    LOG_LEVEL_INFO      = 3,
    LOG_LEVEL_VERBOSE   = 4,
};

enum LogOptions
{
    LogNothing   = 0x00,
    LogToFile    = 0x01,
    LogToConsole = 0x02,
};

#endif//LIB_LOGS_H
