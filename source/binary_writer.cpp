#include "binary_writer.h"
#include "files.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
//
BinaryWriter::BinaryWriter(Stream* stream, bool ownStream, Endian endian, int bufferSize)
    : Writer(stream, ownStream, bufferSize), m_endian(endian)
{
}

BinaryWriter::BinaryWriter(const char* filename, Endian endian, int bufferSize)
    : Writer(File::create(filename), true, bufferSize), m_endian(endian)
{
}

BinaryWriter::BinaryWriter(const string& filename, Endian endian, int bufferSize)
    : Writer(File::create(filename), true, bufferSize), m_endian(endian)
{
}

void BinaryWriter::writeInt16(short value)
{
    value = m_endian.transform16(value);
    write(&value, 0, 2);
}

void BinaryWriter::writeUInt16(ushort value)
{
    value = m_endian.transform16(value);
    write(&value, 0, 2);
}

void BinaryWriter::writeInt32(int value)
{
    value = m_endian.transform32(value);
    write(&value, 0, 4);
}

void BinaryWriter::writeUInt32(uint value)
{
    value = m_endian.transform32(value);
    write(&value, 0, 4);
}

void BinaryWriter::writeInt64(int64 value)
{
    value = m_endian.transform64(value);
    write(&value, 0, 8);
}

void BinaryWriter::writeUInt64(uint64 value)
{
    value = m_endian.transform64(value);
    write(&value, 0, 8);
}

void BinaryWriter::writeFloat(float value)
{
    write(&value, 0, 4);
}

void BinaryWriter::writeDouble(double value)
{
    write(&value, 0, 8);
}

void BinaryWriter::writeString(const char* value, int count)
{
    int size = strlen(value);

    if (count < 0) count = size;
    else if (size > count) size = count;

    write(value, 0, size);

    for (int n = size; n < count; n++) writeByte(0);
}

void BinaryWriter::writeString(const string& value, int count)
{
    int size = value.size();

    if (count < 0) count = size;
    else if (size > count) size = count;

    write(value.c_str(), 0, size);

    for (int n = size; n < count; n++) writeByte(0);
}

void BinaryWriter::write7bitEncodedInt(int value)
{
    uint num = (uint)value;

    while (num >= 0x80)
    {
        writeByte(num | 0x80);
        num = num >> 7;
    }

    writeByte(num);
}

END_NAMESPACE_LIB
