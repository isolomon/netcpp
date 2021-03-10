#ifndef HANDLER_H
#define HANDLER_H

#include "delegate.h"
#include "errors.h"

typedef delegate2<void, int, void*> EventHandler;
typedef delegate2<bool, int, void*> CancelEventHandler;

struct ProgressEventArgs
{
    int         progress;
    int         total;
    int         index;
    int         tasks;
    const char* message;
    bool        cancel;
    void*       state;

    ProgressEventArgs(int p = 0, int t = 0, const char* m = 0) : progress(p), total(t), message(m), cancel(false), index(0), tasks(0), state(0) {}
};

//////////////////////////////////////////////////////////////////////////

struct StatusEventArgs
{
    int         status;
    const char* message;

    StatusEventArgs(int c, const char* m = 0) : status(c), message(m) {}
};

#endif//HANDLER_H