#ifndef LIB_SMART_H
#define LIB_SMART_H

#include "types.h"

BEGIN_NAMESPACE_LIB

class RefCounted
{
public:
    int addRef ()
    {
        int count = ++m_refCount;
        //logmsg("RefCounted: 0x%x, ref = %d\n", this, count);
        return count;
    }

    int release ()
    {
        int count = --m_refCount;
        if (count == 0) delete this;

        //logmsg("RefCounted: 0x%x, ref = %d\n", this, count);
        return count;
    }

protected:
    RefCounted() : m_refCount(0) { addRef(); }
    virtual ~RefCounted() { };

private:
    int m_refCount;
};

//////////////////////////////////////////////////////////////////////////

template <class T>
class RefCountedPtr
{
public:
    // we would not increase the ref count by default, a new created pointer has one ref already
    RefCountedPtr(T* p = 0, bool addref = false) : m_ptr(0) { assign(p, addref); }

    RefCountedPtr(const RefCountedPtr& other)    : m_ptr(0) { assign(other.m_ptr, true); }

    ~RefCountedPtr() { if (m_ptr) m_ptr->release(); }

    RefCountedPtr& operator = (const RefCountedPtr& other)
    {
        assign(other.m_ptr, true);
        return *this;
    }

    void assign (T* p = 0, bool addref = false)
    {
        if (m_ptr != p)
        {
            if (m_ptr) m_ptr->release();

            m_ptr = p;

            if (m_ptr && addref) m_ptr->addRef();
        }
    }

    T*   ptr () const { return  m_ptr; }

    bool operator == (const RefCountedPtr& other) const { return m_ptr == other.m_ptr; }

    bool operator != (const RefCountedPtr& other) const { return m_ptr != other.m_ptr; }

    bool operator == (const T* p) const { return m_ptr == p; }

    bool operator != (const T* p) const { return m_ptr != p; }

    T&   operator *  () const           { return *m_ptr; }

    T*   operator -> () const           { return  m_ptr; }

protected:
    T* m_ptr;
};

//////////////////////////////////////////////////////////////////////////

template <typename T>
class SharedPtr
{
public:
    SharedPtr (T* p = 0) : m_ptr(p), m_ref(0)
    {
        if (m_ptr) m_ref = new size_t(1);
        //logmsg("smart ptr: 0x%x, ref = %d\n", m_ptr, *m_ref);
    }

    SharedPtr (const SharedPtr& other)
    {
        addref(other);
    }

    ~SharedPtr()
    {
        release();
    }

    T*      ptr      () const  { return  m_ptr; }

    size_t  refCount () const  { return *m_ref; }

    void reset (T* p = 0)
    {
        release();

        m_ptr = p;
        m_ref = p ? new size_t(1) : 0;
    }
    
    SharedPtr& operator = (const SharedPtr& other)
    {
        if (m_ptr != other.m_ptr)
        {
            release();
            addref(other);
        }

        return *this;
    }

    bool operator == (const SharedPtr& other) const { return m_ptr == other.m_ptr; }

    bool operator != (const SharedPtr& other) const { return m_ptr != other.m_ptr; }

    bool operator == (const T* p) const { return m_ptr == p; }

    bool operator != (const T* p) const { return m_ptr != p; }

    T&   operator *  () const           { return *m_ptr; }

    T*   operator -> () const           { return  m_ptr; }

private:
    void release()
    {
        if (m_ref)
        {
            (*m_ref)--;

            //logmsg("smart ptr: 0x%x, ref = %d\n", m_ptr, *m_ref);

            if (*m_ref == 0)
            {
                delete m_ptr;
                delete m_ref;

                //logmsg("smart ptr: 0x%x, released\n", m_ptr);
            }
        }
    }


    void addref (const SharedPtr& other)
    {
        m_ptr = other.m_ptr;
        m_ref = other.m_ref;

        if (m_ref)
        {
            (*m_ref)++;
            //logmsg("smart ptr: 0x%x, ref = %d\n", m_ptr, *m_ref);
        }
    }

private:
    T*      m_ptr;
    size_t* m_ref;
};

END_NAMESPACE_LIB

#endif //LIB_SMART_H
