#include "binary_reader.h"
#include "files.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
//
BinaryReader::BinaryReader(Stream* stream, bool ownStream, Endian endian, int bufferSize)
    : Reader(stream, ownStream, bufferSize), m_endian(endian)
{
}

BinaryReader::BinaryReader(const char* filename, Endian endian, int bufferSize)
    : Reader(File::openRead(filename), true, bufferSize), m_endian(endian)
{
}

BinaryReader::BinaryReader(const string& filename, Endian endian, int bufferSize)
    : Reader(File::openRead(filename), true, bufferSize), m_endian(endian)
{
}

BinaryReader::BinaryReader(const void* data, int size, Endian endian)
    : Reader(data, 0, size), m_endian(endian)
{
}

char BinaryReader::readChar()
{
    char* p = m_buf + m_pos;
    ensure(1); m_pos++;
    return *p;
}

byte BinaryReader::readByte()
{
    char* p = m_buf + m_pos;
    ensure(1); m_pos++;
    return *p;
}

short BinaryReader::readInt16()
{
    char* p = m_buf + m_pos;
    ensure(2); m_pos += 2;
    return m_endian.readInt16(p);
}

ushort BinaryReader::readUInt16()
{
    char* p = m_buf + m_pos;
    ensure(2); m_pos += 2;
    return m_endian.readUInt16(p);
}

int BinaryReader::readInt32()
{
    char* p = m_buf + m_pos;
    ensure(4); m_pos += 4;
    return m_endian.readInt32(p);
}

uint BinaryReader::readUInt32()
{
    char* p = m_buf + m_pos;
    ensure(4); m_pos += 4;
    return m_endian.readUInt32(p);
}

int64 BinaryReader::readInt64()
{
    char* p = m_buf + m_pos;
    ensure(8); m_pos += 8;
    return m_endian.readInt64(p);
}

uint64 BinaryReader::readUInt64()
{
    char* p = m_buf + m_pos;
    ensure(8); m_pos += 8;
    return m_endian.readUInt64(p);
}

float BinaryReader::readFloat()
{
    char* p = m_buf + m_pos;
    ensure(4); m_pos += 4;
    return m_endian.readFloat(p);
}

double BinaryReader::readDouble()
{
    char* p = m_buf + m_pos;
    ensure(8); m_pos += 8;
    return m_endian.readDouble(p);
}

string BinaryReader::readString(int size)
{
    string result;

    for (int n = 0; n < size; n++)
    {
        char ch = readChar();
        result.push_back(ch);
    }

    return result;
}

int BinaryReader::read7BitEncodedInt()
{
    char ch;
    int result = 0, shift = 0;

    do
    {
        if (shift == 35) throw FormatException("Invalid 7bit encoded int");

        ch = readChar();
        result |= (ch & 0x7f) << shift;
        shift += 7;
    }
    while ((ch & 0x80) != 0);

    return result;
}

char BinaryReader::peekChar(int offset)
{
    ensure(offset + 1);
    return m_buf[m_pos + offset];
}

byte BinaryReader::peekByte(int offset)
{
    ensure(offset + 1);
    return m_buf[m_pos + offset];
}

short BinaryReader::peekInt16(int offset)
{
    ensure(offset + 2);
    return m_endian.readInt16(m_buf + m_pos + offset);
}

ushort BinaryReader::peekUInt16(int offset)
{
    ensure(offset + 2);
    return m_endian.readUInt16(m_buf + m_pos + offset);
}

int BinaryReader::peekInt32(int offset)
{
    ensure(offset + 4);
    return m_endian.readInt32(m_buf + m_pos + offset);
}

uint BinaryReader::peekUInt32(int offset)
{
    ensure(offset + 4);
    return m_endian.readUInt32(m_buf + m_pos + offset);
}

int64 BinaryReader::peekInt64(int offset)
{
    ensure(offset + 8);
    return m_endian.readInt64(m_buf + m_pos + offset);
}

uint64 BinaryReader::peekUInt64(int offset)
{
    ensure(offset + 8);
    return m_endian.readUInt64(m_buf + m_pos + offset);
}

float BinaryReader::peekFloat(int offset)
{
    ensure(offset + 4);
    return m_endian.readFloat(m_buf + m_pos + offset);
}

double BinaryReader::peekDouble(int offset)
{
    ensure(offset + 8);
    return m_endian.readDouble(m_buf + m_pos + offset);
}

END_NAMESPACE_LIB
