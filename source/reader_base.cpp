#include "reader_base.h"
#include "errors.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
Reader::Reader(Stream* stream, bool ownStream, int bufferSize)
    : m_buf(0), m_pos(0), m_end(0), m_mark(-1), m_size(bufferSize), m_ownBuf(true), m_stream(stream), m_ownStream(ownStream)
{
    if (m_size < 256) m_size = 256;
}

Reader::Reader(const void* data, int offset, int size)
    : m_buf((char*)data + offset), m_pos(0), m_end(size), m_mark(-1), m_size(size), m_ownBuf(false), m_stream(0), m_ownStream(false)
{
}

Reader::~Reader()
{
    close();
}

void Reader::close()
{
    Stream* s = m_stream;
    char*   b = m_buf;

    m_stream = 0;
    m_buf    = 0;

    if (s && m_ownStream)
    {
        s->close();
        delete s;
    }

    if (b && m_ownBuf)
    {
        delete b;
    }
}

void Reader::discardBufferedData()
{
    m_mark = -1;
    m_pos = m_end = 0;
}

bool Reader::eof()
{
    return (fillBuffer(1) == 0);
}

int Reader::read()
{
    if (fillBuffer(1) == 0) return -1;
    return m_buf[m_pos++];
}

int Reader::peek()
{
    if (fillBuffer(1) == 0) return -1;
    return m_buf[m_pos];
}

int Reader::peekBytes(void* data, int count)
{
    int available = fillBuffer(count);
    if (available < count) throw EndOfStreamException();

    memcpy(data, m_buf + m_pos, count);
    return count;
}

int Reader::read(void* data, int offset, int count)
{
    int available = fillBuffer(1);
    if (available == 0) return 0;

    if (count > available) count = available;
    memcpy((char*)data + offset, m_buf + m_pos, count);
    m_pos += count;

    return count;
}

int Reader::readBytes(void* data, int size)
{
    int offset = 0;

    while (size > 0)
    {
        int num = read(data, offset, size);
        if (num == 0) throw EndOfStreamException();

        offset += num;
        size   -= num;
    }

    return offset;
}

void Reader::unread(int ch)
{
    if (ch == -1) return;

    if (m_pos > 0)
    {
        if (m_ownBuf) m_buf[m_pos - 1] = ch;
        else if (m_buf[m_pos - 1] != ch) throw NotSupportedException();

        m_pos--;
    }
    else throw InvalidOperationException();
}

void Reader::mark()
{
    m_mark = m_pos;
}

void Reader::unmark()
{
    m_mark = -1;
}

void Reader::reset()
{
    if (m_mark < 0) throw InvalidOperationException();

    m_pos = m_mark;
    m_mark = -1;
}

int Reader::skip(int numBytes)
{
    int skipped = 0;

    while (numBytes > 0 && fillBuffer(1))
    {
        int numRead = min(numBytes, m_end - m_pos);

        numBytes -= numRead;
        m_pos    += numRead;
        skipped  += numRead;
    }

    return skipped;
}

int Reader::acquire(int numBytes, int timeout)
{
    if (numBytes > m_size) throw InvalidArgumentException();

    int numReady = available();

    if (numReady >= numBytes) return numReady;

    numReady = Error::Timeout; // default to return timeout
    
    if (m_stream->canReady())
    {
        while (numReady < numBytes)
        {
            if (m_stream->readyRead(timeout))
            {
                int value = fillBuffer(-1); // read one more time
                if (value == numReady) return Error::EndOfStream;

                numReady = value;
            }
            else break;
        }        
    }
    else if (m_stream->canTimeout())
    {
        AutoReadTimeout autoTimeout(m_stream, timeout);

        try
        {
            while (numReady < numBytes)
            {
                int value = fillBuffer(-1); // read one more time
                if (value == numReady) return Error::EndOfStream;

                numReady = value;
            }
        }
        catch (...) { }
    }
    else
    {
        throw NotSupportedException();
    }

    return numReady;
}

void Reader::ensure(int numBytes)
{
    if (fillBuffer(numBytes) < numBytes) throw EndOfStreamException();
}

// fills up to numBytes data into the buffer unless reachs end-of-stream.
// when numBytes <= 0 just fill any more from the stream
int Reader::fillBuffer(int numBytes)
{
    int available = m_end - m_pos;
    if (m_stream == 0 || numBytes > 0 && numBytes <= available) return available;

    // lazy create buffer
    if (m_buf == 0) m_buf = new char[m_size];

    numBytes -= available; // this could be negative

    int freeSpace = m_size - m_end;

    if (freeSpace == 0 || freeSpace < numBytes)
    {
        freeSpace = compactBuffer();

        // discard marked data if there is still no enough buffer space
        if (m_mark > -1 && (freeSpace == 0 || freeSpace < numBytes))
        {
            m_mark = -1;
            freeSpace = compactBuffer();
        }

        if (freeSpace < numBytes) throw BufferOverflowException();
    }

    do
    {
        int num = m_stream->read(m_buf, m_end, freeSpace);

        if (num < 0) throw IOException();
        if (num == 0) break;
        
        m_end     += num;
        numBytes  -= num;
        freeSpace -= num;
    }
    while (numBytes > 0);

    return m_end - m_pos;
}

// move existing data to the buffer left and increase free space on the right side
int Reader::compactBuffer()
{
    int dataBegin = m_pos;
    if (m_mark > -1 && m_mark < m_pos) dataBegin = m_mark;

    int dataSize = m_end - dataBegin;

    if (dataSize == 0)
    {
        m_pos = m_end = 0;
        if (m_mark > -1) m_mark = m_pos;
    }
    else
    {
        memmove(m_buf, m_buf + dataBegin, dataSize);

        m_mark -= dataBegin;
        m_pos  -= dataBegin;
        m_end  -= dataBegin;
    }

    return m_size - m_end;
}

END_NAMESPACE_LIB
