#ifndef LIB_STREAM_HEADER
#define LIB_STREAM_HEADER

#include "types.h"

BEGIN_NAMESPACE_LIB

enum SeekOrigin
{
    SeekBegin = 0,
    SeekCurrent = 1,
    SeekEnd = 2,
};

class Stream
{
public:
    Stream ();
    virtual ~Stream ();

    // properties
    virtual bool    canRead         ();
    virtual bool    canWrite        ();
    virtual bool    canSeek         ();
    virtual bool    canTimeout      ();
    virtual bool    canReady        ();

    virtual int     readTimeout     ();    
    virtual int     writeTimeout    ();
    virtual void    setReadTimeout  (int timeout);
    virtual void    setWriteTimeout (int timeout);

    virtual bool    readyRead       (int timeout = 0);
    virtual bool    readyWrite      (int timeout = 0);

    // operations
    // returns the number of bytes read, zero indicates end-of-stream
    virtual int     read            (void* data, int offset, int size);

    // returns the number of bytes written, possibly zero
    virtual int     write           (const void* data, int offset, int size);
	
    virtual void    flush           ();
    virtual void    close           ();

    virtual int     seek            (int offset, int origin);
    virtual int     position        ();
    virtual int     length          ();
    virtual void    setLength       (int value);

    virtual int64   seek64          (int64 offset, int origin);
    virtual int64   position64      ();
    virtual int64   length64        ();
    virtual void    setLength64     (int64 value);

    // read the next byte, or -1 if reached end-of-stream
    virtual int     read            ();
    virtual void    write           (byte value);

    // loop to read number of bytes specified by size
    virtual void    readBytes       (void* data, int size);

    // loop and write all the bytes specified by size
    virtual void    writeBytes      (const void* data, int size);
};

class StreamWrapper : public Stream
{
public:
    StreamWrapper(Stream* stream, bool own) : m_stream(stream), m_ownStream(own) {}

public:
    virtual bool    canRead         ()                          { return m_stream->canRead();           }
    virtual bool    canWrite        ()                          { return m_stream->canWrite();          }
    virtual bool    canSeek         ()                          { return m_stream->canSeek();           }
    virtual bool    canTimeout      ()                          { return m_stream->canTimeout();        }
    virtual bool    canReady        ()                          { return m_stream->canReady();          }

    virtual int     readTimeout     ()                          { return m_stream->readTimeout();       }
    virtual int     writeTimeout    ()                          { return m_stream->writeTimeout();      }
    virtual void    setReadTimeout  (int timeout)               { m_stream->setReadTimeout(timeout);    }
    virtual void    setWriteTimeout (int timeout)               { m_stream->setWriteTimeout(timeout);   }

    virtual bool    readyRead       (int timeout = 0)           { return m_stream->readyRead(timeout);  }
    virtual bool    readyWrite      (int timeout = 0)           { return m_stream->readyWrite(timeout); }

    virtual int     read            (void* data, int offset, int size)       { return m_stream->read(data, offset, size);  }
    virtual int     write           (const void* data, int offset, int size) { return m_stream->write(data, offset, size); }
    virtual void    close           ()                                       { if (m_stream && m_ownStream) { delete m_stream; m_stream = 0; } }

    virtual void    flush           ()                          { m_stream->flush();                    }
    virtual int     seek            (int offset, int origin)    { return m_stream->seek(offset, origin);}
    virtual int     position        ()                          { return m_stream->position();          } 
    virtual int     length          ()                          { return m_stream->length();            }
    virtual void    setLength       (int value)                 { m_stream->setLength(value);           }

    Stream*         innerStream     ()                          { return m_stream; }

protected:
    Stream* m_stream;
    bool    m_ownStream;
};

struct AutoReadTimeout
{
    AutoReadTimeout (Stream* s, int t) : stream(s) { timeout = s->readTimeout(); s->setReadTimeout(t); }
    ~AutoReadTimeout() { if (stream) stream->setReadTimeout(timeout); }
    Stream* stream;
    int timeout;
};

END_NAMESPACE_LIB

#endif//LIB_STREAM_HEADER
