#include "stream_reader.h"
#include "files.h"
#include "utils.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
//
StreamReader::StreamReader(Stream* stream, bool ownStream, int bufferSize) : Reader(stream, ownStream, bufferSize)
{
}

StreamReader::StreamReader(const char* filename, int bufferSize) : Reader(File::openRead(filename), true, bufferSize)
{
}

StreamReader::StreamReader(const string& filename, int bufferSize) : Reader(File::openRead(filename), true, bufferSize)
{
}

StreamReader::StreamReader(const void* data, int offset, int size) : Reader(data, offset, size)
{
}

string StreamReader::readLine()
{
    if (eof()) throw EndOfStreamException();

    string result;
    int c = 0;

    while ((c = read()) >= 0)
    {
        if (c == '\r')
        {
            if (peek() == '\n') read();
            break;
        }
        else if (c == '\n') break;

        result.push_back(c);
    }

    return result;
}

string StreamReader::readToEnd()
{
    string result;

    if (stream()->canSeek())
    {
        int len = stream()->length();
        result.reserve(len > 0 ? len + 64: 0);
    }

    while (fillBuffer(1))
    {
        result.append(m_buf + m_pos, m_end - m_pos);
        m_pos = m_end;
    }

    return result;
}


bool StreamReader::startWith(const char* value, int len, bool caseSensitive)
{
    if (fillBuffer(len) < len) return false;

    if (caseSensitive) return memcmp(buffer(), value, len) == 0;

    #ifdef WIN32
    return strnicmp(buffer(), value, len) == 0;
    #else
    return strncasecmp(buffer(), value, len) == 0;
    #endif
}

int StreamReader::skipCharacters(const char* chars)
{
    int c, skipped = 0;

    while ((c = read()) >= 0)
    {
        if (contains(chars, c)) skipped++;
        else { unread(c); break; }
    }

    return skipped;
}

bool StreamReader::moveToNextLine(int lines)
{
    int c;

    while (lines > 0 && (c = read()) >= 0)
    {
        if (c == '\r')
        {
            if (peek() == '\n') read();
            lines--;
        }
        else if (c == '\n') lines--;
    }

    return lines == 0;
}

bool StreamReader::moveToLine(const char* line)
{
    do 
    {
        skipBlanks();
        if (startWith(line)) return true;
    }
    while (moveToNextLine());

    return false;
}

bool StreamReader::readTo(const char* value, bool stepOver, string* result)
{
    int length = strlen(value);
    if (length == 0) return false;

    while (length && fillBuffer(length) >= length)
    {
        if (memcmp(buffer(), value, length) == 0)
        {
            if (stepOver) skip(length);
            return true;
        }

        int c = read();
        if (result) result->push_back(c);
    }

    return false;
}

bool StreamReader::readTo(char value, bool stepOver, string* result)
{
    int c;

    while ((c = read()) >= 0)
    {
        if (c == value)
        {
            if (!stepOver) unread(c);
            return true;
        }

        if (result) result->push_back(c);
    }

    return false;
}

bool StreamReader::readToFirstOf(const char* chars, bool stepOver, string* result)
{
    int c;

    while((c = read()) >= 0)
    {
        if (contains(chars, c))
        {
            if (!stepOver) unread(c);
            return true;
        }

        if (result) result->push_back(c);
    }

    return false;
}

bool StreamReader::readEscapedTo(char value, bool stepOver, char escape, string* result)
{
    for (;;)
    {
        int c = read();

        if (c == value)
        {
            if (!stepOver) unread(c);
            return true;
        }

        if (c == escape)
        {
            if (result) result->push_back(c);
            c = read();
        }

        if (c < 0) break;
        if (result) result->push_back(c);
    }

    return false;
}

string StreamReader::readQuoted(const char* marks, char escape)
{
    char openMark = marks[0];
    if (openMark == 0) throw InvalidArgumentException();

    char closeMark = marks[1];
    if (closeMark == 0) closeMark = openMark;

    if (!moveTo(openMark)) throw FormatException();

    string result;
    if (readEscapedTo(closeMark, true, escape, &result)) return result;

    throw FormatException();
}

int StreamReader::readToken(const char* extra, bool stepOver, char* dest, int size)
{
    char* begin = dest;
    char* end = dest + size - 1;

    skipWhitespace();

    mark();

    while (dest < end)
    {
        int c = read();

        if (isToken(c, extra))
        {
            *dest++ = c;
            continue;
        }

        if (stepOver)
        {
            if (c == ' ' || c == '\t') { skipCharacters(" \t"); c = read(); }
            if (isCrLf(c) || isToken(c, extra)) unread(c);
        }
        else
        {
            unread(c); // keep the delimiter unread
        }

        unmark();

        if (size > 0) *dest = 0;
        return dest - begin;
    }

    reset();
    return -1;
}

string StreamReader::readToken(const char* extra, bool stepOver)
{
    string result;

    skipWhitespace();
    
    for (;;)
    {
        int c = read();

        if (isToken(c, extra))
        {
            result.push_back((char)c);
            continue;
        }

        if (stepOver)
        {
            if (c == ' ' || c == '\t') { skipCharacters(" \t"); c = read(); }
            if (isCrLf(c) || isToken(c, extra)) unread(c);
        }
        else
        {
            unread(c); // keep the delimiter unread
        }

        break;
    }

    return result;
}

bool StreamReader::readTokenAsBool(const char* extra, bool stepOver)
{
    char token[64];
    int num = readToken(extra, stepOver, token, 64);
    if (num < 0) throw FormatException();

    return Convert::toBool(token);
}

double StreamReader::readTokenAsFloat(const char* extra, bool stepOver)
{
    char token[64];
    int num = readToken(extra, stepOver, token, 64);
    if (num < 0) throw FormatException();

    return Convert::toFloat(token);
}

uint StreamReader::readTokenAsUInt(const char* extra, bool stepOver)
{
    char token[64];
    int num = readToken(extra, stepOver, token, 64);
    if (num < 0) throw FormatException();

    return Convert::toUInt(token);
}

int64 StreamReader::readTokenAsInt64(const char* extra, bool stepOver)
{
    char token[64];
    int num = readToken(extra, stepOver, token, 64);
    if (num < 0) throw FormatException();

    return Convert::toInt64(token);
}

int StreamReader::readTokenAsInt(const char* extra, bool stepOver)
{
    char token[64];
    int num = readToken(extra, stepOver, token, 64);
    if (num < 0) throw FormatException();

    return Convert::toInt(token);
}

bool StreamReader::isToken(int c, const char* extra)
{
    bool valid = false;

    if (isalnum(c) || c == '_') // all alpha and digits are valid
    {
        valid = true;
    }
    else if (c == '+' || c == '-' || c == '.')  // digit signs are also valid
    {
        valid = isdigit(peek());
    }
    
    if (!valid && extra) // otherwise check the additional chars 
    {
        valid = contains(extra, c);
    }

    return valid;
}

END_NAMESPACE_LIB
