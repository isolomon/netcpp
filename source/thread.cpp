#include "thread.h"
#include <pthread.h>
#include <signal.h>

BEGIN_NAMESPACE_LIB

timespec timeout_timespec(int timeout)
{
    timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);

    tm.tv_sec  += timeout / 1000;
    tm.tv_nsec += (timeout % 1000) * 1000000;

    if (tm.tv_nsec >= 1000000000) { tm.tv_sec++; tm.tv_nsec -= 1000000000; }

    return tm;
}

//////////////////////////////////////////////////////////////////////////
Mutex::Mutex ()
{
    handle = new pthread_mutex_t();

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init((pthread_mutex_t*)handle, &attr);
    pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex ()
{
    pthread_mutex_destroy((pthread_mutex_t*)handle);    
    delete (pthread_mutex_t*)handle;
}

bool Mutex::lock ()
{
    return pthread_mutex_lock((pthread_mutex_t*)handle);
}

bool Mutex::lock (int timeout)
{
    if (timeout < 0) return lock();

    timespec tm = timeout_timespec(timeout);

    return pthread_mutex_timedlock((pthread_mutex_t*)handle, &tm) == 0;
}

bool Mutex::tryLock ()
{
    return (pthread_mutex_trylock((pthread_mutex_t*)handle) == 0);
}

void Mutex::unlock ()
{
    pthread_mutex_unlock((pthread_mutex_t*)handle);
}

///////////////////////////////////////////////////////////////
Condition::Condition() 
{
    handle = new pthread_cond_t();
    pthread_cond_init((pthread_cond_t*)handle, 0);
}

Condition::~Condition()
{
    pthread_cond_destroy((pthread_cond_t*)handle);
    delete (pthread_cond_t*)handle;
}

bool Condition::wait(Mutex& mutex, int timeout)
{
    if (timeout < 0)
    {
        return pthread_cond_wait((pthread_cond_t*)handle, (pthread_mutex_t*)mutex.handle) == 0;
    }
    else
    {
        timespec tm = timeout_timespec(timeout);

        int code = pthread_cond_timedwait((pthread_cond_t*)handle, (pthread_mutex_t*)mutex.handle, &tm);

        return code == 0;
    }
}

void Condition::broadcast()
{
    pthread_cond_broadcast((pthread_cond_t*)handle);
}

void Condition::signal()
{
    pthread_cond_signal((pthread_cond_t*)handle);
}

///////////////////////////////////////////////////////////////
Event::Event () : signaled (false)
{
}

void Event::notify ()
{
    mutex.lock();
    signaled = true;
    mutex.unlock();

    cond.signal();
}

void Event::notifyAll ()
{
    mutex.lock();
    signaled = true;
    mutex.unlock();

    cond.broadcast();
}

bool Event::wait (int timeout)
{
    AutoLock lock(mutex);

    while (signaled == false)
    {
        // false return indicates a timeout
        if (!cond.wait(mutex, timeout)) return false;
    }

    return true;
}

void Event::reset()
{
    mutex.lock();
    signaled = false;
    mutex.unlock();
}

//////////////////////////////////////////////////////////////////////////
thread_t Thread::createThread(void* (*routine)(void*), void* args)
{
    pthread_t handle;
    pthread_create(&handle, 0, routine, args);

    return handle;
}

bool Thread::join(int timeout)
{
    if (handle)
    {
        int code = 0;

        if (timeout < 0)
        {
            code = pthread_join(handle, 0);
        }
        else
        {
            timespec tm = timeout_timespec(timeout);
            code = pthread_timedjoin_np(handle, 0, &tm);
        }
        
        handle = 0;
        return code == 0;
    }

    return false;
}

thread_t Thread::currentId()
{
    return (thread_t)pthread_self();
}

END_NAMESPACE_LIB
