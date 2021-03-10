#ifndef LIB_BINARY_WRITER_H
#define LIB_BINARY_WRITER_H

#include "writer_base.h"
#include "binary.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
//
class BinaryWriter : public Writer
{
public:
    BinaryWriter (Stream* steam, bool ownStream = false, Endian endian = Endian::Default, int bufferSize = WRITER_BUFSIZE);

    BinaryWriter (const char* filename, Endian endian = Endian::Default, int bufferSize = WRITER_BUFSIZE);

    BinaryWriter (const string& filename, Endian endian = Endian::Default, int bufferSize = WRITER_BUFSIZE);

public:

    virtual void    writeInt16  (short value);

    virtual void    writeUInt16 (ushort value);

    virtual void    writeInt32  (int value);

    virtual void    writeUInt32 (uint value);

    virtual void    writeInt64  (int64 value);

    virtual void    writeUInt64 (uint64 value);

    virtual void    writeFloat  (float value);

    virtual void    writeDouble (double value);

    virtual void    writeString (const string& value, int count = -1);

    virtual void    writeString (const char* value, int count = -1);

    virtual void    write7bitEncodedInt (int value);

protected:
    Endian  m_endian;
};

END_NAMESPACE_LIB

#endif //LIB_BINARY_WRITER_H
