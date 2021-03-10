#include "stream_writer.h"
#include "files.h"
#include "errors.h"
#include <stdarg.h>

BEGIN_NAMESPACE_LIB
//////////////////////////////////////////////////////////////////////////
//
StreamWriter::StreamWriter(Stream* stream, bool ownStream, int bufferSize) : Writer(stream, ownStream, bufferSize)
{
}

StreamWriter::StreamWriter(const char* fileName, int bufferSize) : Writer(File::create(fileName), true, bufferSize)
{
}

StreamWriter::StreamWriter(const string& fileName, int bufferSize) : Writer(File::create(fileName), true, bufferSize)
{
}

void StreamWriter::write(bool value)
{
    write(value ? "true" : "false");
}

void StreamWriter::write(char value)
{
    writeByte(value);
}

void StreamWriter::write(byte value)
{
    writeByte(value);
}

void StreamWriter::write(short value)
{
    char buf[64];
    int num = sprintf(buf, "%d", value);
    write(buf, 0, num);
}

void StreamWriter::write(ushort value)
{
    char buf[64];
    int num = sprintf(buf, "%u", value);
    write(buf, 0, num);
}

void StreamWriter::write(int value)
{
    char buf[64];
    int num = sprintf(buf, "%d", value);
    write(buf, 0, num);
}

void StreamWriter::write(uint value)
{
    char buf[64];
    int num = sprintf(buf, "%u", value);
    write(buf, 0, num);
}

void StreamWriter::write(int64 value)
{
    char buf[64];
    int num = sprintf(buf, "%lld", value);
    write(buf, 0, num);
}

void StreamWriter::write(uint64 value)
{
    char buf[64];
    int num = sprintf(buf, "%llu", value);
    write(buf, 0, num);
}

void StreamWriter::write(float value, int digit)
{
    char fmt[16] = "%f";
    if (digit >= 0) sprintf(fmt, "%%.%df", digit);

    char buf[64];
    int num = sprintf(buf, fmt, value);
    write(buf, 0, num);
}

void StreamWriter::write(double value, int digit)
{
    char fmt[16] = "%f";
    if (digit >= 0) sprintf(fmt, "%%.%df", digit);

    char buf[64];
    int num = sprintf(buf, fmt, value);
    write(buf, 0, num);
}

void StreamWriter::write(const char* value)
{
    write(value, 0, strlen(value));
}

void StreamWriter::write(const string& value)
{
    write(value.c_str(), 0, value.size());
}

void StreamWriter::writeFormat(const char* fmt, ...)
{
    char buffer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end (args);

    int size = strlen(buffer);
    write(buffer, 0, size);
}

void StreamWriter::writeLine(bool value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(char value)
{
    writeByte(value);
    writeLine();
}

void StreamWriter::writeLine(byte value)
{
    writeByte(value);
    writeLine();
}

void StreamWriter::writeLine(short value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(ushort value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(int value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(uint value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(int64 value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(uint64 value)
{
    write(value);
    writeLine();
}

void StreamWriter::writeLine(float value, int digit)
{
    write(value, digit);
    writeLine();
}

void StreamWriter::writeLine(double value, int digit)
{
    write(value, digit);
    writeLine();
}

void StreamWriter::writeLine(const string& value)
{
    write(value.c_str(), 0, value.size());
    writeLine();
}

#ifdef QT_CORE_LIB
void StreamWriter::write(const QString& value)
{
    string str = value.toStdString();
    write(str.c_str(), 0, str.size());
}

void StreamWriter::writeLine(const QString& value)
{
    string str = value.toStdString();
    write(str.c_str(), 0, str.size());
    writeLine();
}
#endif

void StreamWriter::writeLine(const char* value)
{
    write(value, 0, strlen(value));
    writeLine();
}

void StreamWriter::writeLine()
{
#ifdef _WIN32
    write("\r\n", 0, 2);
#else
    writeByte('\n');
#endif
}

END_NAMESPACE_LIB
