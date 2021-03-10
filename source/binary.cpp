#include "binary.h"

BEGIN_NAMESPACE_LIB

const Endian Endian::Big(BigEndian);

const Endian Endian::Little(LittleEndian);

const Endian Endian::Default(DefaultEndian);

//////////////////////////////////////////////////////////////////////////
char Endian::readChar (const char* buffer) const
{
    return *buffer;
}

byte Endian::readByte (const char* buffer) const
{
    return (byte)*buffer;
}

short Endian::readInt16 (const char* buffer) const
{
    return transform16(*(uint16*)buffer);
}

ushort Endian::readUInt16 (const char* buffer) const
{
    return transform16(*(uint16*)buffer);
}

int Endian::readInt32 (const char* buffer) const
{
    return transform32(*(uint32*)buffer);
}

uint Endian::readUInt32 (const char* buffer) const
{
    return transform32(*(uint32*)buffer);
}

int64 Endian::readInt64 (const char* buffer) const
{
    return transform64(*(uint64*)buffer);
}

uint64 Endian::readUInt64 (const char* buffer) const
{
    return transform64(*(uint64*)buffer);
}

float Endian::readFloat (const char* buffer) const
{
    return *(float*)buffer;
}

double Endian::readDouble (const char* buffer) const
{
    return *(double*)buffer;
}

void Endian::writeChar (char* buffer, char value) const
{
    *buffer = value;
}

void Endian::writeByte (char* buffer, byte value) const
{
    *buffer = (char)value;
}

void Endian::writeInt16 (char* buffer, short value) const
{
    *(uint16*)buffer = transform16(value);
}

void Endian::writeUInt16 (char* buffer, ushort value) const
{
    *(uint16*)buffer = transform16(value);
}

void Endian::writeInt32 (char* buffer, int value) const
{
    *(uint32*)buffer = transform32(value);
}

void Endian::writeUInt32 (char* buffer, uint value) const
{
    *(uint32*)buffer = transform32(value);
}

void Endian::writeInt64 (char* buffer, int64 value) const
{
    *(uint64*)buffer = transform64(value);
}

void Endian::writeUInt64 (char* buffer, uint64 value) const
{
   *(uint64*)buffer = transform64(value);
}

void Endian::writeFloat (char* buffer, float value) const
{
    *(float*)buffer = value;
}

void Endian::writeDouble (char* buffer, double value) const
{
    *(double*)buffer = value;
}

//////////////////////////////////////////////////////////////////////////
string Hex::toString(const void* data, int size, const char* join)
{
    string result;
    const char* dict = "0123456789ABCDEF";
    const byte* ch = (const byte*)data;

    for (int n = 0; n < size; n++)
    {
        if (join && result.size()) result.append(join);

        result.push_back(dict[ch[n] >> 4]);
        result.push_back(dict[ch[n] & 0x0F]);
    }

    return result;
}

string Hex::toBytes(const char* hex)
{
    string result;
    bool good = true;
    int value = 0;

    while (hex && *hex)
    {
        int ch = toByte(*hex++);
        if (ch < 0) continue;

        if (good) { value = ch << 4; good = false; }
        else { value |= ch; result.push_back(value); good = true; }
    }

    return result;
}

int Hex::toBytes(const char* hex, void* data, int size)
{
    bool  first = true;
    char* dest = (char*)data;
    char* destEnd = dest + size;

    while (hex && *hex && dest < destEnd)
    {
        int ch = toByte(*hex++);
        if (ch < 0) continue;

        if (first) { *dest = ch << 4; first = false; }
        else { *dest |= ch; first = true; dest++;}
    }

    return dest - (char*)data;
}

int Hex::toByte(int hex)
{
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;

    return -1;
}

END_NAMESPACE_LIB
