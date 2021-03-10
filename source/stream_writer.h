#ifndef LIB_STREAM_WRITER_H
#define LIB_STREAM_WRITER_H

#include "writer_base.h"

#ifdef QT_CORE_LIB
#include <QString>
#endif

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
class StreamWriter : public Writer
{
public:
    StreamWriter(Stream* stream, bool ownStream = false, int bufferSize = WRITER_BUFSIZE);

    StreamWriter(const char* fileName, int bufferSize = WRITER_BUFSIZE);

    StreamWriter(const string& fileName, int bufferSize = WRITER_BUFSIZE);

    using Writer::write;

    void    write       (bool value);

    void    write       (char value);

    void    write       (byte value);

    void    write       (short value);

    void    write       (ushort value);

    void    write       (int value);

    void    write       (uint value);

    void    write       (int64 value);

    void    write       (uint64 value);

    void    write       (float value, int digit = -1);

    void    write       (double value, int digit = -1);

    void    write       (const char* value);

    void    write       (const string& value);

    void    writeFormat (const char* fmt, ...);


    void    writeLine   (bool value);

    void    writeLine   (char value);

    void    writeLine   (byte value);

    void    writeLine   (short value);

    void    writeLine   (ushort value);

    void    writeLine   (int value);

    void    writeLine   (uint value);

    void    writeLine   (int64 value);

    void    writeLine   (uint64 value);

    void    writeLine   (float value, int digit = -1);

    void    writeLine   (double value, int digit = -1);

    void    writeLine   (const char* value);

    void    writeLine   (const string& value);

    void    writeLine   ();

#ifdef QT_CORE_LIB
    void    write       (const QString& value);
    void    writeLine   (const QString& value);
#endif

};

END_NAMESPACE_LIB

#endif //LIB_WRITER_H
