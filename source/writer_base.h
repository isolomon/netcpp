#ifndef LIB_WRITER_BASE_H
#define LIB_WRITER_BASE_H

#include "stream.h"

BEGIN_NAMESPACE_LIB

#ifndef WRITER_BUFSIZE
#define WRITER_BUFSIZE  8192
#endif

//////////////////////////////////////////////////////////////////////////
//
class Writer
{
public:
    Writer(Stream* stream, bool ownStream, int bufferSize = WRITER_BUFSIZE);
    virtual ~Writer();

public:
    inline Stream*  stream      ()  { return m_stream;    }

    inline  bool    autoFlush   ()  { return m_autoFlush; }

    inline  void    autoFlush   (bool value)  { m_autoFlush = value; if (value) flush(); }
    
    virtual void    write       (const void* data, int offset, int size);

    virtual void    flush       ();

    virtual void    close       ();

    virtual void    writeChar   (char value);

    virtual void    writeByte   (byte value);

    virtual void    writeBytes  (const void* data, int size) { write(data, 0, size); }

protected:
    void  ensureBuffer (int numBytes);

protected:
    char*   m_buf;
    int     m_end;
    int     m_size;

    Stream* m_stream;
    bool    m_ownStream;
    bool    m_autoFlush;
};

END_NAMESPACE_LIB

#endif //LIB_WRITER_BASE_H
