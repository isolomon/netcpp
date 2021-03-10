#include "process.h"
#include "files.h"
#include "errors.h"
#include <errno.h>

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

BEGIN_NAMESPACE_LIB

int Process::start(const char* cmdfile, const char* args)
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    pid_t pid = fork();

    if (pid == 0) // child
    {
        int code = ::execlp(cmdfile, "mpeg", args, 0);
        ::exit(code);
    }

    return pid;
}

void Process::kill(int pid, int sig)
{
    ::kill(pid, sig);
}

bool Process::wait(int pid)
{
    int code = ::waitpid(pid, 0, WEXITED);
    return code > 0;
}

bool Process::exited(int pid)
{
    int code = ::waitpid(pid, 0, WNOHANG);
    return code != 0;
}

ProcessStream* Process::popen(const char* cmdfile)
{
    FILE* pf = ::popen(cmdfile, "r");
    return new ProcessStream(pf);
}

int Process::popen(const char* cmdfile, string& output)
{
    ProcessStream stream(::popen(cmdfile, "r"));

    output = File::readContent(&stream);

    return stream.exitCode();
}

//////////////////////////////////////////////////////////////////////////
ProcessStream::ProcessStream(FILE* handle)
{
    m_handle = handle;
    m_exitCode = -1;

    if (handle == 0) throw InvalidArgumentException();
}

ProcessStream::~ProcessStream()
{
    close();
}

int ProcessStream::read(void* data, int offset, int size)
{
    if (size <= 0) return 0;

    char* dest = (char*)data + offset;

    while (true)
    {
        int num = fread(dest, 1, size, m_handle);
        if (num == 0 && errno == EINTR) continue;
        if (num || feof(m_handle)) return num;

        throw IOException("Can not read on ProcessStream");
    }
}

int ProcessStream::write(const void* data, int offset, int size)
{
    throw NotSupportedException();

    if (size <= 0) return 0;

    char* dest = (char*)data + offset;
    int num = fwrite(dest, 1, size, m_handle);
    if (num > 0) return num;

    throw IOException("Can not write to ProcessStream");;
}

void ProcessStream::close()
{
    if (m_handle)
    {
        m_exitCode = ::pclose(m_handle);
        m_handle = 0;
    }
}

END_NAMESPACE_LIB

