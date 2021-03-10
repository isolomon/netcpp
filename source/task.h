#ifndef LIB_TASK_H
#define LIB_TASK_H

#include "thread.h"
#include "delegate.h"
#include "typehold.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
struct Task
{
    virtual ~Task () {};
    virtual void run () = 0;
};

class TaskManager
{
public:
    TaskManager () : m_running(false) {}

    void    start       ();

    void    stop        ();

    bool    running     ();

    Task*   nextTask    (int timeout = 0);

    void    queueTask   (Task* task);

protected:
    void    threadEntry ();

protected:
    typedef EventQueue<Task*> TaskQueue;

    Thread      m_thread;
    TaskQueue   m_tasks;
    bool        m_running;
};

//////////////////////////////////////////////////////////////////////////
template <class P>
struct AsyncTask : public Task
{
    typedef delegate<void,P> method_t;
    typedef typehold<P>      params_t;

    template <class T>
    AsyncTask (T* obj, void (T::*ent)(P), P p) : method(obj, ent), params(p) {}
    AsyncTask (method_t m, P p) : method(m), params(p) {}
    ~AsyncTask() {}

    void run () { method.invoke(params.value); }

protected:
    method_t method;
    params_t params;
};

template <>
struct AsyncTask<void> : public Task
{
    typedef delegate<void,void> method_t;

    template <class T>
    AsyncTask (T* obj, void (T::*ent)()) : method(obj, ent) {}
    AsyncTask (method_t m) : method(m) {}
    ~AsyncTask() {}

    void run () { method.invoke(); }

protected:
    method_t method;
};

//////////////////////////////////////////////////////////////////////////
template <class P1, class P2>
struct AsyncTask2 : public Task
{
    typedef delegate2<void,P1,P2> method_t;
    typedef typehold2<P1,P2>      params_t;

    template <class T>
    AsyncTask2 (T* obj, void (T::*ent)(P1,P2), P1 p1, P2 p2) : method(obj, ent), params(p1, p2) {}
    AsyncTask2 (method_t m, P1 p1, P2 p2) : method(m), params(p1, p2) {}
    ~AsyncTask2() {}

    void run () { method.invoke(params.value, params.value2); }

protected:
    method_t method;
    params_t params;
};

//////////////////////////////////////////////////////////////////////////
template <class P1, class P2, class P3>
struct AsyncTask3 : public Task
{
    typedef delegate3<void,P1,P2,P3> method_t;
    typedef typehold3<P1,P2,P3>      params_t;

    template <class T>
    AsyncTask3 (T* obj, void (T::*ent)(P1,P2,P3), P1 p1, P2 p2, P3 p3) : method(obj, ent), params(p1, p2, p3) {}
    AsyncTask3 (method_t m, P1 p1, P2 p2, P3 p3) : method(m), params(p1, p2, p3) {}
    ~AsyncTask3() {}

    void run () { method.invoke(params.value, params.value2, params.value3); }

protected:
    method_t method;
    params_t params;
};

//////////////////////////////////////////////////////////////////////////
template <class P1, class P2, class P3, class P4>
struct AsyncTask4 : public Task
{
    typedef delegate4<void,P1,P2,P3,P4> method_t;
    typedef typehold4<P1,P2,P3,P4>      params_t;

    template <class T>
    AsyncTask4 (T* obj, void (T::*ent)(P1,P2,P3,P4), P1 p1, P2 p2, P3 p3, P4 p4) : method(obj, ent), params(p1, p2, p3, p4) {}
    AsyncTask4 (method_t m, P1 p1, P2 p2, P3 p3, P4 p4) : method(m), params(p1, p2, p3) {}
    ~AsyncTask4() {}

    void run () { method.invoke(params.value, params.value2, params.value3, params.value4); }

protected:
    method_t method;
    params_t params;
};

END_NAMESPACE_LIB

#endif //LIB_TASK_H
