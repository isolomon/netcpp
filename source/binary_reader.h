#ifndef LIB_BINARY_READER_H
#define LIB_BINARY_READER_H

#include "reader_base.h"
#include "binary.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
//
class BinaryReader : public Reader
{
public:
    BinaryReader (Stream* stream, bool ownStream = false, Endian endian = Endian::Default, int bufferSize = READER_BUFSIZE);

    BinaryReader (const char* filename, Endian endian = Endian::Default, int bufferSize = READER_BUFSIZE);

    BinaryReader (const string& filename, Endian endian = Endian::Default, int bufferSize = READER_BUFSIZE);

    BinaryReader (const void* data, int size, Endian endian = Endian::Default);

public:
    virtual char    readChar    ();

    virtual byte    readByte    ();

    virtual short   readInt16   ();

    virtual ushort  readUInt16  ();

    virtual int     readInt32   ();

    virtual uint    readUInt32  ();

    virtual int64   readInt64   ();

    virtual uint64  readUInt64  ();

    virtual float   readFloat   ();

    virtual double  readDouble  ();

    virtual string  readString  (int size);

    virtual int     read7BitEncodedInt ();


    virtual char    peekChar    (int offset = 0);

    virtual byte    peekByte    (int offset = 0);

    virtual short   peekInt16   (int offset = 0);

    virtual ushort  peekUInt16  (int offset = 0);

    virtual int     peekInt32   (int offset = 0);

    virtual uint    peekUInt32  (int offset = 0);

    virtual int64   peekInt64   (int offset = 0);

    virtual uint64  peekUInt64  (int offset = 0);

    virtual float   peekFloat   (int offset = 0);

    virtual double  peekDouble  (int offset = 0);

protected:
    Endian  m_endian;
};

END_NAMESPACE_LIB

#endif //LIB_BINARY_READER_H
