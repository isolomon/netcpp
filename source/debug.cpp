#include "debug.h"
#include "files.h"
#include <stdarg.h>

using namespace lib;

char g_logfile [512] = { 0 };
int  g_logLevel = LOG_LEVEL_WARN;
int  g_logFlag  = LogToFile | LogToConsole;

int logLevel()
{
    return g_logLevel;
}

void setLogLevel(int level)
{
    g_logLevel = level;
}

void setLogFlags(int flags)
{
    g_logFlag = flags;
}

void clearLogs()
{
    if (File::exists(g_logfile)) File::truncate(g_logfile);
}

void setLogFile(const char* file)
{
    strcpy(g_logfile, file);
}

void setLogFile(const std::string& file)
{
    setLogFile(file.c_str());
}

void writelog(const char* data)
{
    if (hasFlag(g_logFlag, LogToFile))
    {
        if (g_logfile[0])
        {
            File::append(g_logfile, data);
        }
    }
    
    if (hasFlag(g_logFlag, LogToConsole))
    {
        printf(data);
        fflush(stdout);
    }
}

void writelog(const void* data, int size)
{
    if (hasFlag(g_logFlag, LogToFile))
    {
        if (g_logfile[0])
        {
            File::append(g_logfile, data, size);
        }
    }

    if (hasFlag(g_logFlag, LogToConsole))
    {
        fwrite(data, 1, size, stdout);
        fflush(stdout);
    }
}

void logmsg(const char* fmt, ...)
{
    if (g_logLevel < LOG_LEVEL_ERROR) return;

    char bufer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(bufer, sizeof(bufer), fmt, args);
    va_end (args);

    writelog(bufer);
}

void log_verb(const char* fmt, ...)
{
    if (g_logLevel < LOG_LEVEL_VERBOSE) return;

    char bufer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(bufer, sizeof(bufer), fmt, args);
    va_end (args);

    writelog(bufer);
}

void log_info(const char* fmt, ...)
{
    if (g_logLevel < LOG_LEVEL_INFO) return;

    char bufer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(bufer, sizeof(bufer), fmt, args);
    va_end (args);

    writelog(bufer);
}

void log_warn(const char* fmt, ...)
{
    if (g_logLevel < LOG_LEVEL_WARN) return;

    char bufer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(bufer, sizeof(bufer), fmt, args);
    va_end (args);

    writelog(bufer);
}

void log_error(const char* fmt, ...)
{
    if (g_logLevel < LOG_LEVEL_ERROR) return;

    char bufer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(bufer, sizeof(bufer), fmt, args);
    va_end (args);

    writelog(bufer);
}

//////////////////////////////////////////////////////////////////////////
void dumpData(const void* data, int size)
{
    logmsg("---------- %d bytes ----------\n", size);
    for (int n = 0; n < size; n++)
    {
        logmsg("0x%02x, ", ((char*)data)[n]);
        if (n % 16 == 15) logmsg("\n");
    }
    logmsg("---------- end ----------\n");
}

void dumpDouble(const double* data, int count, const char* msg)
{
    logmsg("---------- count = %d ----------\n", count);
    for (int n = 0; n < count; n++)
    {
        logmsg("%.02f, ", data[n]);
        if (n % 16 == 15) logmsg("\n");
    }
    logmsg("---------- end ----------\n");
}

void dumpFloat(const float* data, int count)
{
    logmsg("---------- count = %d ----------\n", count);
    for (int n = 0; n < count; n++)
    {
        logmsg("%.02f, ", data[n]);
        if (n % 16 == 15) logmsg("\n");
    }
    logmsg("---------- end ----------\n");
}
