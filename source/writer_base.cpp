#include "writer_base.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
Writer::Writer(Stream* stream, bool ownStream, int bufferSize)
    : m_buf(0), m_end(0), m_size(bufferSize), m_stream(stream), m_ownStream(ownStream), m_autoFlush(false)
{
    if (m_size < 256) m_size = 256;
}

Writer::~Writer()
{
    close();
}

void Writer::flush()
{
    if (m_stream && m_buf && m_end > 0)
    {
        m_stream->writeBytes(m_buf, m_end);
        m_stream->flush();
        m_end = 0;
    }
}

void Writer::close()
{
    bool soError = false;
    bool ioError = false;

    try { flush(); }
    catch (SocketException&) { soError = true; }
    catch (...) { ioError = true; }
    
    Stream* s = m_stream;
    char*   b = m_buf;

    m_stream = 0;
    m_buf = 0;
    m_end = 0;

    if (s && m_ownStream) delete s;
    if (b) delete b;
    
    if (soError) throw SocketException();
    if (ioError) throw IOException();
}

void Writer::writeChar(char value)
{
    writeByte(value);
}

void Writer::writeByte(byte value)
{
    if (m_autoFlush)
    {
        m_stream->write(value);
        m_stream->flush();

        return;
    }

    ensureBuffer(1);
    m_buf[m_end++] = value;

    if (m_end == m_size) flush();
}

void Writer::write(const void* data, int offset, int size)
{
    if (m_autoFlush)
    {
        m_stream->writeBytes((const char*)data + offset, size);
        m_stream->flush();

        return;
    }

    const char* source = (const char*)data + offset;
    if (m_buf == 0) m_buf = new char[m_size];

    while (size > 0)
    {
        int num = min(size, m_size - m_end);
        memcpy(m_buf + m_end, source, num);

        size   -= num;
        source += num;
        m_end  += num;

        if (m_end == m_size) flush();
    }
}

void Writer::ensureBuffer(int numBytes)
{
    if (m_buf == 0) m_buf = new char[m_size];

    if (m_size - m_end < numBytes)
    {
        flush();

        if (m_size - m_end < numBytes) throw BufferOverflowException();
    }
}

END_NAMESPACE_LIB
