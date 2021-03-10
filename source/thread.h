#ifndef LIB_THREAD_H
#define LIB_THREAD_H

#include "types.h"
#include "errors.h"
#include <deque>

typedef void*           handle_t;
typedef unsigned long   thread_t;

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
class Mutex
{
public:
    Mutex ();

    virtual ~Mutex();

    virtual bool lock    () ;

    virtual bool lock    (int timeout);

    virtual bool tryLock ();

    virtual void unlock  ();

protected:
    handle_t handle;

    friend class Condition;
};

class AutoLock
{
public:
    AutoLock    (Mutex* m) : mutex(m)  { lock(); }

    AutoLock    (Mutex& m) : mutex(&m) { lock(); }

    ~AutoLock   ()  { if (locked) unlock(); }

    void lock   ()  { mutex->lock();   locked = true;  }

    void unlock ()  { mutex->unlock(); locked = false; }

private:
    Mutex* mutex;
    bool   locked;
};


//////////////////////////////////////////////////////////////////////////
class Condition
{
public:
    Condition  ();

    ~Condition ();

    void signal ();
    
    void broadcast ();

    bool wait (Mutex& mutex, int timeout = -1);

protected:
    handle_t handle;
};

class Event
{
public:
    Event ();

    void notify    ();

    void notifyAll ();

    bool wait      (int timeout = -1);

    void reset     ();

protected:
    Mutex mutex;
    Condition cond;
    bool signaled;
};


//////////////////////////////////////////////////////////////////////////
class Thread
{
public:
    Thread() : handle(0) { }

    template<class T>
    void start (T* object, void (T::*entry) ())
    {
        if (handle) logmsg("*** thread was launched multi-times ***\n");
        if (sizeof(wrapper<T>) > HolderSize) throw std::bad_alloc();

        wrapper<T>* args = (wrapper<T>*)&holder;
        args->object = object;
        args->entry = entry;

        handle = createThread(wrapper<T>::threadEntry, args);
    }

    template<class T>
    void start (T* object, void (T::*entry) (void*), void* arg)
    {
        if (handle) logmsg("*** thread was launched multi-times ***\n");
        if (sizeof(wrapper2<T>) > HolderSize) throw std::bad_alloc();

        wrapper2<T>* args = (wrapper2<T>*)&holder;
        args->object = object;
        args->entry = entry;
        args->arg = arg;

        handle = createThread(wrapper2<T>::threadEntry, args);
    }

    bool join    (int timeout = -1);
    
    bool idle    () { return handle == 0; }

    bool started () { return handle != 0; }

    thread_t id  () { return handle; }

    static thread_t currentId ();

protected:
    template<class T> struct wrapper
    {
        typedef void (T::*Entry) ();
        T* object; Entry entry;

        static void* threadEntry (void* args)
        {
            wrapper* w = (wrapper*)args;
            (w->object->*w->entry)();
            return 0;
        }
    };

    template<class T> struct wrapper2
    {
        typedef void (T::*Entry) (void*);
        T* object; Entry entry; void* arg;

        static void* threadEntry (void* args)
        {
            wrapper2* w = (wrapper2*)args;
            (w->object->*w->entry)(w->arg);
            return 0;
        }
    };

    static thread_t createThread(void* (*routine)(void*), void* args);

protected:
    enum { HolderSize = 28 }; // be careful about the data size
    thread_t  handle;
    char      holder[HolderSize];
};


//////////////////////////////////////////////////////////////////////////
template <class T>
class EventQueue
{
public:
    typedef typename std::deque<T>::iterator iterator;

    void push (T object)
    {
        AutoLock lock(mutex);
        queue.push_back(object);
        cond.signal();
    }

    template <class Predicate>
    T find (Predicate pred, int timeout = -1)
    {
        AutoLock lock(mutex);

        while (true)
        {
            for (iterator it = queue.begin(); it != queue.end(); ++it)
            {
                if (pred(*it)) { T obj = *it; queue.erase(it); return obj; }
            }

            // return false indicates a timeout
            if (!cond.wait(mutex, timeout)) break;
        }

        throw TimeoutException("Timeout while EventQueue::find");
    }

    void clear (bool del = false)
    {
        AutoLock lock(mutex);

        if (del)
        {
            for (iterator it = queue.begin(); it != queue.end(); ++it)
            {
                delete *it;
            }
        }

        queue.clear();
    }

    bool remove (T object)
    {
        AutoLock lock(mutex);

        for (iterator it = queue.begin(); it != queue.end(); ++it)
        {
            if ((*it) == object) { queue.erase(it); return true; }
        }
        
        return false;
    }

    T peek (int timeout = -1)
    {
        AutoLock lock(mutex);

        while (queue.size() == 0)
        {
            // return false indicates a timeout
            if (!cond.wait(mutex, timeout)) throw TimeoutException("Timeout while EventQueue::wait");
        }

        T item = queue.front();
        return item;
    }

    T wait (int timeout = -1)
    {
        AutoLock lock(mutex);

        while (queue.size() == 0)
        {
            // return false indicates a timeout
            if (!cond.wait(mutex, timeout)) throw TimeoutException("Timeout while EventQueue::wait");
        }

        T item = queue.front();
        queue.pop_front();

        return item;
    }

protected:
    std::deque<T> queue;
    Mutex mutex;
    Condition cond;
};

END_NAMESPACE_LIB

#endif //LIB_THREAD_H
