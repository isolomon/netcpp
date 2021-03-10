#ifndef LIB_TYPES_H
#define LIB_TYPES_H

#include "config.h"

#include <string>
#include <vector>

#include <string.h>
#include <malloc.h>

#if defined(_MSC_VER) && (_MSC_VER < 1600) // <VS2010
typedef __int8              int8_t;
typedef __int16             int16_t;
typedef __int32             int32_t;
typedef __int64             int64_t;
typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;
#else
#include <stdint.h>
#endif

typedef wchar_t             wchar;
typedef unsigned char       byte;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;

typedef wchar_t             WChar;
typedef signed char         Char;
typedef unsigned char       Byte;
typedef float               Float;
typedef double              Double;

typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef int64_t             int64;
typedef uint64_t            uint64;

typedef signed char         Int8;
typedef signed short        Int16;
typedef signed int          Int32;
typedef unsigned char       UInt8;
typedef unsigned short      UInt16;
typedef unsigned int        UInt32;
typedef int64_t             Int64;
typedef uint64_t            UInt64;

typedef intptr_t            IntPtr;
typedef uintptr_t           UIntPtr;
typedef void*               Handle;

using std::string;
using std::wstring;
using std::vector;

typedef std::vector<std::string>  strings;
typedef std::vector<std::wstring> wstrings;

extern const std::string NullString;

//////////////////////////////////////////////////////////////////////////

template<class T>
struct RangeT
{
    T low; T high;

    RangeT () : low(0), high (0) {}
    RangeT (const T& from, const T& to) : low(from), high (to) {}
    RangeT (const RangeT& other) : low(other.low), high(other.high) {}

    inline bool operator == (const RangeT& other) const { return low == other.low && high == other.high; }
    inline bool operator != (const RangeT& other) const { return ! operator == (other); }
    inline T    center   () const               { return (high + low) / 2; }
    inline T    span     () const               { return high - low;       }
    inline bool contains (const T& val) const   { return val >= low && val <= high; }
};

typedef RangeT<int>     Range;
typedef RangeT<double>  RangeF;

//////////////////////////////////////////////////////////////////////////

template<class T>
struct PointT
{
    T x; T y;

    PointT () {}
    PointT (const T& x, const T& y) : x(x), y(y) {}
};

typedef PointT<int>     Point;
typedef PointT<double>  PointF;

struct Point3D
{
    double x; double y; double z;

    Point3D () {}
    Point3D (double x, double y, double z) : x(x), y(y), z(z) {}
};

//////////////////////////////////////////////////////////////////////////

template <typename T>
class AutoPtr
{
public:
    AutoPtr  (T* p = 0) : m_ptr(p) {}
    ~AutoPtr () { reset(); }

    T*   get   () const     { return  m_ptr; }
    void reset (T* p = 0)   { if (m_ptr) delete m_ptr; m_ptr = p; }

    T&  operator *  () const  { return *m_ptr; }
    T*  operator -> () const  { return  m_ptr; }

    AutoPtr& operator = (T* p)  { reset(p); return *this; }
    AutoPtr& operator = (AutoPtr& other)  { reset(other.m_ptr); other.m_ptr = 0; return *this; }

protected:
    AutoPtr (const AutoPtr& other);
    AutoPtr& operator = (const AutoPtr& other);

protected:
    T* m_ptr;
};

//////////////////////////////////////////////////////////////////////////
class AutoBool
{
public:
    AutoBool(bool& var, bool init = true, bool final = false) : m_bool(var), m_final(final) { m_bool = init; }
    ~AutoBool() { m_bool = m_final; }

protected:
    bool& m_bool;
    bool  m_final;
};

//////////////////////////////////////////////////////////////////////////

inline bool hasFlag(int value, int flag)   { return (value & flag) == flag; }

template<class T>
inline T abs_t (const T& v) { return v < 0 ? -v : v; }

template<class T1, class T2>
inline T1 min (const T1& v1, const T2& v2) { return v1 < v2 ? v1 : v2; }

template<class T1, class T2>
inline T1 max (const T1& v1, const T2& v2) { return v1 < v2 ? v2 : v1; }

template<class T1, class T2, class T3>
inline T1 clip (const T1& val, const T2& min, const T3& max) { return (val < min) ? min : (val > max ? max : val); }

template<class T1, class T2>
inline int compare (const T1& v1, const T2& v2) { return v1 == v2 ? 0 : (v1 < v2 ? -1 : 1); }

//////////////////////////////////////////////////////////////////////////

#include "debug.h"

#endif//LIB_TYPES_H
