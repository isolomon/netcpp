#ifndef LIB_HISTORY_H
#define LIB_HISTORY_H

#include "types.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

template <class T>
class History
{
public:
    typedef std::vector<T> collection;
    typedef typename std::vector<T>::iterator iterator;

public:
    History () : m_pos (-1) { }

    iterator begin ()
    {
        return m_items.begin();
    }

    iterator end ()
    {
        return m_items.end();
    }

    iterator current ()
    {
        return m_items.begin() + m_pos;
    }
    
    bool canBack () 
    {
        return m_pos > 0 && m_items.size() > 1;
    }

    bool canForward ()
    {
        return m_pos > -1 && m_pos + 1 < m_items.size();
    }

    T backward ()
    {
        if (canBack()) return m_items[--m_pos];
        else throw InvalidOperationException("no more item to backward");
    }

    T forward ()
    {
        if (canForward()) return m_items[++m_pos];
        else throw InvalidOperationException("no more item to forward");
    }

    void navigate (const T& value)
    {
        m_items.resize(m_pos + 2);
        m_items[++m_pos] = value;
    }

    T at (int index)
    {
        return m_items[index];
    }

    int pos ()
    {
        return m_pos - m_items.begin();
    }

    int count ()
    {
        return m_items.size();
    }

protected:
    collection  m_items;
    int         m_pos;
};

END_NAMESPACE_LIB

#endif //LIB_HISTORY_H
