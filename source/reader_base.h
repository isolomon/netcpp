#ifndef LIB_READER_BASE_H
#define LIB_READER_BASE_H

#include "stream.h"

BEGIN_NAMESPACE_LIB

#ifndef READER_BUFSIZE
#define READER_BUFSIZE  8192
#endif

class Reader
{
public:
    Reader (Stream* stream, bool ownStream, int bufferSize = READER_BUFSIZE);

    Reader (const void* data, int offset, int size);

    virtual ~Reader ();

public:
    Stream*     stream      ()  { return m_stream;  }

    void        discardBufferedData ();
    
    int         acquire     (int numBytes, int timeout = -1);   // return >= 0 on success, else for error codes

    void        ensure      (int numBytes);

    const char* buffer      ()  { return m_buf + m_pos; }
    
    int         available   ()  { return m_end - m_pos; }

    bool        eof         ();

    int         read        (void* data, int offset, int count);

    int         readBytes   (void* data, int count);

    int         read        ();

    void        unread      (int ch);

    int         peek        ();

    int         peekBytes   (void* data, int count);

    int         skip        (int numBytes);

    void        mark        ();

    void        unmark      ();

    void        reset       ();

    virtual void close      ();

protected:
    int     fillBuffer      (int numBytes);

    int     compactBuffer   ();

protected:
    char*   m_buf;
    int     m_pos;
    int     m_end;
    int     m_mark;
    int     m_size;

    Stream* m_stream;
    bool    m_ownBuf;
    bool    m_ownStream;
};

END_NAMESPACE_LIB

#endif //LIB_READER_BASE_H
