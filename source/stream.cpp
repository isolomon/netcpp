#include "stream.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

Stream::Stream()
{
}

Stream::~Stream()
{
}

bool Stream::canRead()
{
    return false;
}

bool Stream::canWrite()
{
    return false;
}

bool Stream::canTimeout()
{
    return false;
}

bool Stream::canReady()
{
    return false;
}

bool Stream::canSeek()
{
    return false;
}

int Stream::readTimeout ()
{
    throw NotSupportedException("Timeouts Not Supported");
}

void Stream::setReadTimeout (int milliseconds)
{
    throw NotSupportedException("Timeouts Not Supported");
}

int Stream::writeTimeout()
{
    throw NotSupportedException("Timeouts Not Supported");
}

void Stream::setWriteTimeout (int milliseconds)
{
    throw NotSupportedException("Timeouts Not Supported");
}

bool Stream::readyRead(int timeout)
{
    throw NotSupportedException("readyRead Not Supported");
}

bool Stream::readyWrite(int timeout)
{
    throw NotSupportedException("readyWrite Not Supported");
}

int Stream::read(void* data, int offset, int size)
{
    throw NotSupportedException("read Not Supported");
}

int Stream::write(const void* data, int offset, int size)
{
    throw NotSupportedException("write Not Supported");
}

void Stream::flush()
{
}

void Stream::close()
{
}

int Stream::seek (int offset, int origin)
{
    throw NotSupportedException("Seeking Not Supported");
}

int Stream::position ()
{
    throw NotSupportedException("Seeking Not Supported");
}

int Stream::length ()
{
    throw NotSupportedException("Seeking Not Supported");
}

void Stream::setLength (int value)
{
    throw NotSupportedException("Seeking Not Supported");
}

int64 Stream::seek64(int64 offset, int origin)
{
    return seek(offset, origin);
}

int64 Stream::position64()
{
    return position();
}

int64 Stream::length64()
{
    return length();
}

void Stream::setLength64(int64 value)
{
    setLength(value);
}

int Stream::read ()
{
    byte value;

    int num = read(&value, 0, 1);
    if (num < 0) throw IOException();

    return num ? value : -1;
}

void Stream::write (byte value)
{
    int num = write(&value, 0, 1);
    if (num <= 0) throw IOException();
}

void Stream::readBytes(void* data, int size)
{
    int offset = 0;

    while (size > 0)
    {
        int num = read(data, offset, size);

        if (num == 0) throw EndOfStreamException();
        if (num < 0)  throw IOException();

        size   -= num;
        offset += num;
    }
}

void Stream::writeBytes(const void* data, int size)
{
    int offset = 0;

    while (size > 0)
    {
        int num = write(data, offset, size);
        if (num < 0) throw IOException();

        size -= num;
        offset += num;
    }
}

END_NAMESPACE_LIB
