#ifndef LIB_BUFFER_H
#define LIB_BUFFER_H

#include "stream.h"
#include "binary.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

class Buffer : public Stream
{
public:
    Buffer (const Endian& endian = Endian::Default);

    Buffer (int size, const Endian& endian = Endian::Default);

    Buffer (const void* data, int size, const Endian& endian = Endian::Default);    
    
    Buffer (void* data, int size, bool rdonly = false, const Endian& endian = Endian::Default);

    Buffer (const Buffer& other);

    virtual ~Buffer();

public:
    ByteOrder   order       ()  const       { return m_endian.order();  }
                
    void        setOrder    (ByteOrder val) { m_endian.setOrder(val);   }

    bool        readonly    () const        { return m_rdonly; }

    bool        fixed       () const        { return m_fixed;  }

    const char* begin       () const        { return m_data;   }

    const char* current     () const        { return m_data + m_pos; }

    const char* end         () const        { return m_data + m_len; }

    char*       begin       ()              { return m_data; }

    char*       current     ()              { return m_data + m_pos; }

    char*       end         ()              { return m_data + m_len; }

    int         size        () const        { return m_size; }

    int         position    () const        { return m_pos; }

    void        setPosition (int pos)       { assertLen(pos); m_pos = pos; }

    int         length      () const        { return m_len; }

    void        setLength   (int len)       { reserve(len); m_len = len; }

    void        extendLen   (int len)       { reserve(m_len + len); m_len += len; }

    int         remaining   () const        { return m_len - m_pos;  }

    int         freeSpace   () const        { return m_size - m_len; }

    void        forward     (int numBytes)  { assertLen (m_pos + numBytes); m_pos += numBytes; }

    void        backward    (int numBytes)  { assertLen (m_pos - numBytes); m_pos -= numBytes; }

    void        rewind      ()              { m_pos = 0; }

    void        flip        ()              { m_len = m_pos; m_pos = 0; }

    void        compact     ();

    void        reserve     (int size);

    void        clear       (bool withZeros = false);

    void        setData     (const void* data, int size);


    char        at          (int index) const   { return m_data[index]; }

    char&       at          (int index)         { return m_data[index]; }  

    char        operator [] (int index) const   { return m_data[index]; }

    char&       operator [] (int index)         { return m_data[index]; }


    int         find        (int index, char ch) const;

    int         findAny     (int index, const char* chars) const;

    int         findAny     (int index, const char* first, const char* last) const;

    int         copyTo      (Buffer& dest, int offset = 0, int pos = 0, int size = -1) const;

    int         copyTo      (void* dest, int offset = 0, int pos = 0, int size = -1) const;


    operator    const void* () const    { return (const void*)m_data; }

    operator    char*       ()          { return m_data;    }
    
public: // stream methods

    virtual bool canRead    ()  { return true; }

    virtual bool canWrite   ()  { return !m_rdonly; }

    virtual bool canSeek    ()  { return true; }

    virtual int  read       ();

    virtual void write      (byte value);
    
    virtual int  write      (const void* data, int offset, int size);

    virtual int  read       (void* data, int offset, int size);

    virtual int  seek       (int offset, int origin);

    virtual void close      ();

public: // binary read / write method
    virtual char    readChar    ();

    virtual byte    readByte    ();

    virtual short   readInt16   ();

    virtual ushort  readUInt16  ();

    virtual int     readInt32   ();

    virtual uint    readUInt32  ();

    virtual int64   readInt64   ();

    virtual uint64  readUInt64  ();

    virtual float   readFloat   ();

    virtual double  readDouble  ();

    virtual string  readString  (int size, bool trimNull = true);
    

    virtual void    writeChar   (char value, int count = 1);

    virtual void    writeByte   (byte value, int count = 1);

    virtual void    writeInt16  (short value);

    virtual void    writeUInt16 (ushort value);

    virtual void    writeInt32  (int value);

    virtual void    writeUInt32 (uint value);

    virtual void    writeInt64  (int64 value);

    virtual void    writeUInt64 (uint64 value);

    virtual void    writeFloat  (float value);

    virtual void    writeDouble (double value);

    virtual void    writeString (const string& value, int count = -1);


    virtual byte    peekByte    (int offset = 0) const;

    virtual char    peekChar    (int offset = 0) const;

    virtual short   peekInt16   (int offset = 0) const;

    virtual ushort  peekUInt16  (int offset = 0) const;

    virtual int     peekInt32   (int offset = 0) const;

    virtual uint    peekUInt32  (int offset = 0) const;

    virtual int64   peekInt64   (int offset = 0) const;

    virtual uint64  peekUInt64  (int offset = 0) const;

    virtual float   peekFloat   (int offset = 0) const;

    virtual double  peekDouble  (int offset = 0) const;

    // write data to the end of the buffer without setting position
    virtual void    append       (const void* data, int offset, int size);

    virtual void    append       (byte value);

    virtual void    appendBytes  (const void* data, int size);

    virtual void    appendChar   (char value, int count = 1);

    virtual void    appendByte   (byte value, int count = 1);

    virtual void    appendInt16  (short value);

    virtual void    appendUInt16 (ushort value);

    virtual void    appendInt32  (int value);

    virtual void    appendUInt32 (uint value);

    virtual void    appendInt64  (int64 value);

    virtual void    appendUInt64 (uint64 value);

    virtual void    appendFloat  (float value);

    virtual void    appendDouble (double value);

    virtual void    appendString (const string& value, int count = -1);

protected:
    void  init      (char* data, int size, int pos, int len, bool ro, bool fixed, bool own, const Endian& endian);

    void  assertLen (int len) const { if ( len < 0 || len > m_len) throw IndexOutOfRangeException(); }

protected:
    char*   m_data;
    int     m_size;
    int     m_pos;
    int     m_len;
    bool    m_own;
    bool    m_rdonly;
    bool    m_fixed;
    Endian  m_endian;

    static char m_null[0];
};

typedef Buffer MemoryStream;

//////////////////////////////////////////////////////////////////////////
template <int TSize>
class StackBuffer : public Buffer
{
public:
    StackBuffer (const Endian& endian = Endian::Default) 
        : Buffer(m_stack, TSize, false, endian) { m_fixed = false; m_len = 0; }

protected:
    StackBuffer (const StackBuffer& other);
    char m_stack[TSize];
};

typedef StackBuffer<64>    Buffer64;
typedef StackBuffer<128>   Buffer128;
typedef StackBuffer<256>   Buffer256;
typedef StackBuffer<512>   Buffer512;
typedef StackBuffer<1024>  Buffer1K;
typedef StackBuffer<2048>  Buffer2K;
typedef StackBuffer<4096>  Buffer4K;
typedef StackBuffer<8192>  Buffer8K;
typedef StackBuffer<65536> Buffer64K;

END_NAMESPACE_LIB
#endif //LIB_BUFFER_H
