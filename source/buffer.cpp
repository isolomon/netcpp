#include "buffer.h"
#include <string.h>

BEGIN_NAMESPACE_LIB

char Buffer::m_null[0];

Buffer::Buffer (const Endian& endian)
{
    init(m_null, 0, 0, 0, false, false, false, endian);
}

Buffer::Buffer (int capacity, const Endian& endian)
{
    init(m_null, 0, 0, 0, false, false, false, endian);
    if (capacity > 0) reserve(capacity);
}

Buffer::Buffer(const void* data, int size, const Endian& endian)
{
    init((char*)data, size, 0, size, true, true, false, endian);
}

Buffer::Buffer (void* data, int size, bool ro, const Endian& endian)
{
    init((char*)data, size, 0, size, ro, true, false, endian);
}

Buffer::Buffer (const Buffer& other) : Stream()
{
    init(other.m_data, other.m_size, other.m_pos, other.m_len, false, false, true, other.m_endian);
}

void Buffer::init(char* data, int size, int pos, int len, bool ro, bool fix, bool own, const Endian& endian)
{
    m_data   = data;
    m_size   = size;
    m_pos    = pos;
    m_len    = len;
    m_own    = own;
    m_rdonly = ro;
    m_fixed  = fix;
    m_endian = endian;

    if (own && size)
    {
        m_data = (char*)malloc(m_size);
        memcpy(m_data, data, m_len);
    }
}

Buffer::~Buffer()
{
    close();
}

void Buffer::clear(bool withZeros)
{
    m_pos = 0;
    m_len = 0;

    if (withZeros)
    {
        if (m_rdonly) throw InvalidOperationException("Buffer is readonly");
        memset(m_data, 0, m_size);
    }
}

void Buffer::compact()
{
    if (m_pos == 0) return;
    if (m_pos >= m_len) { clear(); return; }

    if (m_rdonly) throw InvalidOperationException("Buffer is readonly");
    
    int count = m_len - m_pos;
    memmove(m_data, m_data + m_pos, count);

    m_len = count;
    m_pos = 0;
}

void Buffer::reserve(int size)
{
    if (m_size < size)
    {
        if (m_rdonly) throw InvalidOperationException("Buffer is readonly");
        if (m_fixed)  throw InvalidOperationException("Buffer size is fixed");

        if (size < (m_size << 1)) size = (m_size << 1);
        if (size < 64) size = 64;

        char* p = (char*)malloc(size);
        memcpy(p, m_data, m_size);
        if (m_own) ::free(m_data);

        m_data = p;
        m_size = size;
        m_own  = true;
    }
}

void Buffer::setData(const void* data, int size)
{
    reserve(size);
    memcpy(m_data, data, size);

    m_pos = 0;
    m_len = size;
}

void Buffer::close ()
{
    char* p = m_data;

    m_data = 0;
    m_len = m_pos = m_size = 0;

    if (m_own && p)
    {
        m_own = false;
        ::free(p);
    }
}

int Buffer::find (int index, char ch) const
{
    for (int n = index; n < m_len; ++n)
    {
        if (m_data[n] == ch) return n;
    }

    return -1;
}

int Buffer::findAny(int index, const char* chars) const
{
    return findAny(index, chars, chars + strlen(chars));
}

int Buffer::findAny(int index, const char* beg, const char* end) const
{
    for (int n = index; n < m_len; ++n)
    {
        for (const char* p = beg; p != end; ++p)
        {
            if (*p == m_data[n]) return n;
        }
    }

    return -1;
}

int Buffer::copyTo(Buffer& dest, int offset, int pos, int size) const
{
    size = min((size < 0 ? m_len - pos : size), m_len);
    dest.reserve(offset + size);

    memcpy(dest.m_data + offset, m_data + pos, size);
    return size;
}

int Buffer::copyTo(void* dest, int offset, int pos, int size) const
{
    size = min((size < 0 ? m_len - pos : size), m_len);

    memcpy((char*)dest + offset, m_data + pos, size);
    return size;
}

int Buffer::read ()
{
    return (m_pos < m_len) ? (byte)m_data[m_pos++] : -1;
}

void Buffer::write (byte value)
{
    reserve(m_pos + 1);
    m_data[m_pos++] = value;

    if (m_len < m_pos) m_len = m_pos;
}

int Buffer::read (void* data, int offset, int size)
{
    if (size <= 0) return 0;

    char* p = m_data + m_pos; 

    int available = m_len - m_pos;
    if (size > available) size = available;

    m_pos += size;
    memcpy((char*)data + offset, p, size);

    return size;
}

int Buffer::seek (int offset, int origin)
{
    if (origin == SeekCurrent)  setPosition(m_pos + offset);
    if (origin == SeekEnd)      setPosition(m_len + offset);
    if (origin == SeekBegin)    setPosition(offset);

    return m_pos;
}

int Buffer::write (const void* data, int offset, int size)
{
    if (size <= 0) return 0;

    reserve(m_pos + size);
    memcpy(m_data + m_pos, (const char*)data + offset, size);

    m_pos += size;
    if (m_len < m_pos) m_len = m_pos;

    return size;
}

//////////////////////////////////////////////////////////////////////////

char Buffer::readChar()
{
    char* p = current(); forward(1);
    return *p;
}

byte Buffer::readByte()
{
    char* p = current(); forward(1);
    return *p;
}

short Buffer::readInt16()
{
    char* p = current(); forward(2);
    return m_endian.readInt16(p);
}

ushort Buffer::readUInt16()
{
    char* p = current(); forward(2);
    return m_endian.readUInt16(p);
}

int Buffer::readInt32()
{
    char* p = current(); forward(4);
    return m_endian.readInt32(p);
}

uint Buffer::readUInt32()
{
    char* p = current(); forward(4);
    return m_endian.readUInt32(p);
}

int64 Buffer::readInt64()
{
    char* p = current(); forward(8);
    return m_endian.readInt64(p);
}

uint64 Buffer::readUInt64()
{
    char* p = current(); forward(8);
    return m_endian.readUInt64(p);
}

float Buffer::readFloat()
{
    char* p = current(); forward(4);
    return m_endian.readFloat(p);
}

double Buffer::readDouble()
{
    char* p = current(); forward(8);
    return m_endian.readDouble(p);
}

string Buffer::readString(int size, bool trimNull)
{
    string result;
    bool hasNull = false;

    for (int n = 0; n < size; n++)
    {
        char ch = readChar();

        if (ch == 0) hasNull = true;
        if (trimNull && hasNull) continue;

        result.push_back(ch);
    }

    return result;
}

void Buffer::writeChar(char value, int count)
{
    writeByte(value, count);
}

void Buffer::writeByte(byte value, int count)
{
    if (count == 1)
    {
        write(value);
    }
    else if (count > 1)
    {
        reserve(m_pos + count);
        memset(m_data + m_pos, value, count);

        m_pos += count;
        if (m_len < m_pos) m_len = m_pos;
    }
}

void Buffer::writeInt16(short value)
{
    value = m_endian.transform16(value);
    write(&value, 0, 2);
}

void Buffer::writeUInt16(ushort value)
{
    value = m_endian.transform16(value);
    write(&value, 0, 2);
}

void Buffer::writeInt32(int value)
{
    value = m_endian.transform32(value);
    write(&value, 0, 4);
}

void Buffer::writeUInt32(uint value)
{
    value = m_endian.transform32(value);
    write(&value, 0, 4);
}

void Buffer::writeInt64(int64 value)
{
    value = m_endian.transform64(value);
    write(&value, 0, 8);
}

void Buffer::writeUInt64(uint64 value)
{
    value = m_endian.transform64(value);
    write(&value, 0, 8);
}

void Buffer::writeFloat(float value)
{
    write(&value, 0, 4);
}

void Buffer::writeDouble(double value)
{
    write(&value, 0, 8);
}

void Buffer::writeString(const string& value, int count)
{
    int size = value.size();

    if (count < 0) count = size;
    else if (size > count) size = count;

    write(value.c_str(), 0, size);

    writeByte(0, count - size);
}

//////////////////////////////////////////////////////////////////////////

char Buffer::peekChar (int offset) const
{
    assertLen(m_pos + offset + 1);
    return m_data[m_pos + offset];
}

byte Buffer::peekByte (int offset) const
{
    assertLen(m_pos + offset + 1);
    return m_data[m_pos + offset];
}

short Buffer::peekInt16 (int offset) const
{
    assertLen(m_pos + offset + 2);
    return m_endian.readInt16(m_data + m_pos + offset);
}

ushort Buffer::peekUInt16 (int offset) const
{
    assertLen(m_pos + offset + 2);
    return m_endian.readUInt16(m_data + m_pos + offset);
}

int Buffer::peekInt32 (int offset) const
{
    assertLen(m_pos + offset + 4);
    return m_endian.readInt32(m_data + m_pos + offset);
}

uint Buffer::peekUInt32 (int offset) const
{
    assertLen(m_pos + offset + 4);
    return m_endian.readUInt32(m_data + m_pos + offset);
}

int64 Buffer::peekInt64 (int offset) const
{
    assertLen(m_pos + offset + 8);
    return m_endian.readInt64(m_data + m_pos + offset);
}

uint64 Buffer::peekUInt64 (int offset) const
{
    assertLen(m_pos + offset + 8);
    return m_endian.readUInt64(m_data + m_pos + offset);
}

float Buffer::peekFloat (int offset) const
{
    assertLen(m_pos + offset + 4);
    return m_endian.readFloat(m_data + m_pos + offset);
}

double Buffer::peekDouble (int offset) const
{
    assertLen(m_pos + offset + 8);
    return m_endian.readDouble(m_data + m_pos + offset);
}

//////////////////////////////////////////////////////////////////////////
void Buffer::append(const void* data, int offset, int size)
{
    if (size <= 0) return;

    reserve(m_len + size);
    memcpy(m_data + m_len, (char*)data + offset, size);

    m_len += size;
}

void Buffer::append(byte value)
{
    reserve(m_len + 1);
    m_data[m_len++] = value;
}

void Buffer::appendBytes(const void* data, int size)
{
    append(data, 0, size);
}

void Buffer::appendChar(char value, int count)
{
    appendByte(value, count);
}

void Buffer::appendByte(byte value, int count)
{
    if (count == 1)
    {
        append(value);
    }
    else if (count > 1)
    {
        reserve(m_len + count);
        memset(m_data + m_len, value, count);

        m_len += count;
    }
}

void Buffer::appendInt16(short value)
{
    value = m_endian.transform16(value);
    append(&value, 0, 2);
}

void Buffer::appendUInt16(ushort value)
{
    value = m_endian.transform16(value);
    append(&value, 0, 2);
}

void Buffer::appendInt32(int value)
{
    value = m_endian.transform32(value);
    append(&value, 0, 4);
}

void Buffer::appendUInt32(uint value)
{
    value = m_endian.transform32(value);
    append(&value, 0, 4);
}

void Buffer::appendInt64(int64 value)
{
    value = m_endian.transform64(value);
    append(&value, 0, 8);
}

void Buffer::appendUInt64(uint64 value)
{
    value = m_endian.transform64(value);
    append(&value, 0, 8);
}

void Buffer::appendFloat(float value)
{
    append(&value, 0, 4);
}

void Buffer::appendDouble(double value)
{
    append(&value, 0, 8);
}

void Buffer::appendString(const string& value, int count)
{
    int size = value.size();

    if (count < 0) count = size;
    else if (size > count) size = count;

    append(value.c_str(), 0, size);

    appendByte(0, count - size);
}

END_NAMESPACE_LIB
