#ifndef LIB_PROCESS_H
#define LIB_PROCESS_H

#include "stream.h"
#include <signal.h>

BEGIN_NAMESPACE_LIB

class ProcessStream : public Stream
{
public:
    ProcessStream (FILE* handle);
    ~ProcessStream ();

    virtual bool canRead        ()  { return true; }
    virtual bool canWrite       ()  { return true; }

    virtual int  read           (void* data, int offset, int size);
    virtual int  write          (const void* data, int offset, int size);
    virtual void close          ();

    virtual int  exitCode       ()  { close(); return m_exitCode; }

private:
    FILE*   m_handle;
    int     m_exitCode;
};

class Process
{
public:
    static int  start   (const char* cmdfile, const char* args);

    static bool wait    (int pid);

    static bool exited  (int pid);

    static void kill    (int pid, int sig = SIGKILL);

    static void term    (int pid) { kill(pid, SIGTERM); }

    static ProcessStream* popen (const char* cmdfile);

    static int            popen (const char* cmdfile, string& output);
};

END_NAMESPACE_LIB

#endif //LIB_PROCESS_H
