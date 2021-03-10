#ifndef LIB_BINARY_H
#define LIB_BINARY_H

#ifdef WIN32
#include <stdlib.h>
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __BYTE_ORDER    1234
#define bswap_16    _byteswap_ushort
#define bswap_32    _byteswap_ulong
#define bswap_64    _byteswap_uint64
#else
#include <endian.h>
#include <byteswap.h>
#endif

#include "types.h"

BEGIN_NAMESPACE_LIB

enum ByteOrder
{
    BigEndian = 0,
    LittleEndian = 1,

    #if __BYTE_ORDER == __LITTLE_ENDIAN
    DefaultEndian = LittleEndian,
    #else
    DefaultEndian = BigEndian,
    #endif
};

#if __BYTE_ORDER == __LITTLE_ENDIAN
  inline uint16 hton16(uint16 value) { return bswap_16(value); }
  inline uint32 hton32(uint32 value) { return bswap_32(value); }
  inline uint64 hton64(uint64 value) { return bswap_64(value); }
  inline uint16 ntoh16(uint16 value) { return bswap_16(value); }
  inline uint32 ntoh32(uint32 value) { return bswap_32(value); }
  inline uint64 ntoh64(uint64 value) { return bswap_64(value); }
#else
  inline uint16 hton16(uint16 value) { return value; }
  inline uint32 hton32(uint32 value) { return value; }
  inline uint64 hton64(uint64 value) { return value; }
  inline uint16 ntoh16(uint16 value) { return value; }
  inline uint32 ntoh32(uint32 value) { return value; }
  inline uint64 ntoh64(uint64 value) { return value; }
#endif

//////////////////////////////////////////////////////////////////////////
class Endian
{
public:
    Endian (ByteOrder order = DefaultEndian) : m_order(order) { }

    inline ByteOrder order      () const            { return (ByteOrder)m_order; }
                              
    inline void      setOrder   (ByteOrder value)   { m_order = value;  }


    inline bool     isBig       () const { return m_order == BigEndian;     }

    inline bool     isLittle    () const { return m_order == LittleEndian;  }

    inline bool     isDefault   () const { return m_order == DefaultEndian; }

    inline uint16   transform16 (uint16 value) const  { return isDefault() ? value : bswap_16(value); }

    inline uint32   transform32 (uint32 value) const  { return isDefault() ? value : bswap_32(value); }

    inline uint64   transform64 (uint64 value) const  { return isDefault() ? value : bswap_64(value); }


    char    readChar    (const char* buffer) const;

    byte    readByte    (const char* buffer) const;

    short   readInt16   (const char* buffer) const;

    ushort  readUInt16  (const char* buffer) const;

    int     readInt32   (const char* buffer) const;

    uint    readUInt32  (const char* buffer) const;

    int64   readInt64   (const char* buffer) const;

    uint64  readUInt64  (const char* buffer) const;

    float   readFloat   (const char* buffer) const;

    double  readDouble  (const char* buffer) const;


    void    writeChar   (char* buffer, char value) const;

    void    writeByte   (char* buffer, byte value) const;

    void    writeInt16  (char* buffer, short value) const;

    void    writeUInt16 (char* buffer, ushort value) const;

    void    writeInt32  (char* buffer, int value) const;

    void    writeUInt32 (char* buffer, uint value) const;

    void    writeInt64  (char* buffer, int64 value) const;

    void    writeUInt64 (char* buffer, uint64 value) const;

    void    writeFloat  (char* buffer, float value) const;

    void    writeDouble (char* buffer, double value) const;

protected:
    byte    m_order;

public:
    static const Endian Little;
    static const Endian Big;
    static const Endian Default;
};

//////////////////////////////////////////////////////////////////////////
struct Hex
{
    static string toString  (const void* hexData, int size, const char* join = 0);

    static string toString  (const string& hexData, const char* join = 0) { return toString(hexData.c_str(), hexData.size(), join); }

    static string toBytes   (const char* hexString);

    static string toBytes   (const string& hexString)   { return toBytes(hexString.c_str()); }

    static int    toBytes   (const char* hexString, void* hexData, int size);

    static int    toBytes   (const string& hexString, void* hexData, int size)   { return toBytes(hexString.c_str(), hexData, size); }

    static int    toByte    (int hex);
};

END_NAMESPACE_LIB

#endif //LIB_BINARY_H
