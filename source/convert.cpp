#include "convert.h"
#include "utils.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

struct ConvertFilter { bool operator () (char c) { return c != ' ' && c != ','; } };

int Convert::toInt(const char* value, int base)
{
    char data[64];
    strncpy_if(data, value, 64, ConvertFilter());
    return strtol(data, 0, base);
}

int64 Convert::toInt64(const char* value, int base)
{
    char data[64];
    strncpy_if(data, value, 64, ConvertFilter());

    #if defined(_MSC_VER)
    return _strtoi64(data, 0, base);
    #else
    return strtoll(data, 0, base);
    #endif
}

uint Convert::toUInt(const char* value, int base)
{
    char data[64];
    strncpy_if(data, value, 64, ConvertFilter());
    return strtoul(data, 0, base);
}

string Convert::toString(double value, int digit, char fch)
{
    char fmt[] = "%.*f";
    char buffer[64];
    fmt[3] = fch;

    sprintf(buffer, fmt, digit, value);

    string result(buffer);

    if (fch == 'e')
    {
        replace(result, "e+0", "e+");
        replace(result, "e-0", "e-");
    }

    return result;
}

string Convert::toString(ulong value)
{
    char buf[64];
    sprintf(buf, "%lu", value);
    return string(buf);
}

string Convert::toString(long value)
{
    char buf[64];
    sprintf(buf, "%ld", value);
    return string(buf);
}

string Convert::toString(int64 value)
{
    char buf[64];
    sprintf(buf, "%lld", value);
    return string(buf);
}

string Convert::toString(uint64 value)
{
    char buf[64];
    sprintf(buf, "%llu", value);
    return string(buf);
}

string Convert::toString(bool value)
{
    return value ? "true" : "false";
}

bool Convert::toBool(const char* value)
{
    string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return (lower == "true" || lower == "yes" || lower == "on" || lower == "1");
}

bool Convert::toBool(const string& value)
{
    string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return (lower == "true" || lower == "yes" || lower == "on" || lower == "1");
}

double Convert::toFloat(const string& value)
{
    return atof(value.c_str());
}

double Convert::toFloat(const char* value)
{
    return atof(value);
}

//////////////////////////////////////////////////////////////////////////

StringStream::StringStream(string& source) : m_str(source), m_readonly(false), m_pos(0)
{
}

StringStream::StringStream(const string& source) : m_str((string&)source), m_readonly(true), m_pos(0)
{
}

int StringStream::seek(int offset, int origin)
{
    int pos = m_pos;

    if (origin == SeekBegin)    pos = offset;
    if (origin == SeekEnd)      pos = m_str.size() + offset;
    if (origin == SeekCurrent)  pos += offset;

    if (pos < 0 || pos > m_str.size()) throw IndexOutOfRangeException();

    return m_pos = pos;
}

void StringStream::setLength(int value)
{
    if (m_readonly) throw InvalidOperationException();

    m_str.resize(value);
}

int StringStream::read(void* data, int offset, int size)
{
    const char* pos = m_str.c_str() + m_pos;

    if (m_pos + size > m_str.size()) size = m_str.size() - m_pos;

    memcpy((char*)data + offset, pos, size);

    m_pos += size;

    return size;
}

int StringStream::write(const void* data, int offset, int size)
{
    if (m_readonly) throw InvalidOperationException();

    int newSize = m_pos + size;
    if (newSize > m_str.capacity()) m_str.reserve(newSize * 3 / 2);
    if (newSize > m_str.size()) m_str.resize(newSize);

    const char* src = (const char*)data + offset;
    for (int n = 0; n < size; n++) m_str[m_pos++] = *src++;

    return size;
}

void StringStream::write(byte value)
{
    if (m_readonly) throw InvalidOperationException();

    int newSize = m_pos + 1;
    if (newSize > m_str.capacity()) m_str.reserve(newSize * 3 / 2);
    if (newSize > m_str.size()) m_str.resize(newSize);

    m_str[m_pos++] = value;
}

int StringStream::read()
{
    return (m_pos < m_str.size()) ? m_str[m_pos++] : -1;
}

END_NAMESPACE_LIB
