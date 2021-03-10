#ifndef LIB_CONVERT_H
#define LIB_CONVERT_H

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include "types.h"
#include "stream.h"

BEGIN_NAMESPACE_LIB

class Convert
{
public:
    static int      toInt       (const char* value, int base = 0);

    static int      toInt       (const string& value, int base = 0) { return toInt(value.c_str(), base); }

    static int64    toInt64     (const char* value, int base = 0);

    static int64    toInt64     (const string& value, int base = 0) { return toInt64(value.c_str(), base); }

    static uint     toUInt      (const char* value, int base = 0);

    static uint     toUInt      (const string& value, int base = 0) { return toUInt(value.c_str(), base); }

    static bool     toBool      (const char* value);

    static bool     toBool      (const string& value);

    static double   toFloat     (const char* value);

    static double   toFloat     (const string& value);

    #ifdef QT_CORE_LIB
    static int      toInt       (const QString& value, int base = 0) { return toInt(value.toStdString(), base); }

    static int64    toInt64     (const QString& value, int base = 0) { return toInt64(value.toStdString(), base); }

    static uint     toUInt      (const QString& value, int base = 0) { return toUInt(value.toStdString(), base); }

    static bool     toBool      (const QString& value)  { return toBool(value.toStdString()); }

    static double   toFloat     (const QString& value)  { return toFloat(value.toStdString()); }
    #endif

    static string   toHex       (byte* data, int count, char* join = 0);

    static string   toString    (bool value);

    static string   toString    (char value)    { return toString((long)value); }

    static string   toString    (byte value)    { return toString((ulong)value); }

    static string   toString    (short value)   { return toString((long)value); }

    static string   toString    (ushort value)  { return toString((ulong)value); }

    static string   toString    (int value)     { return toString((long)value); }

    static string   toString    (uint value)    { return toString((ulong)value); }

    static string   toString    (long value);

    static string   toString    (ulong value);

    static string   toString    (int64 value);

    static string   toString    (uint64 value);

    static string   toString    (double value, int digit = 2, char fmt = 'f');

    template <class InputIt>
    static string toStrings (InputIt first, InputIt last)
    {
        string result; result.reserve(4096);
        for (InputIt it = first; it != last; it++)
        {
            if (it != first) result.append(", ");
            result.append(Convert::toString(*it));
        }
        return result;
    }

    template <class InputIt, typename ConvFn>
    static string toStrings (InputIt first, InputIt last, ConvFn conv)
    {
        string result; result.reserve(4096);
        for (InputIt it = first; it != last; it++)
        {
            if (it != first) result.append(", ");
            result.append(conv(*it));
        }
        return result;
    }

    template <class I, class ConvFn>
    static I toArray (const string& values, I begin, I end, ConvFn conv)
    {
        string result; I it = begin;
        for (uint pos = 0; pos != string::npos && it != end; it++)
        {
            uint token = values.find(',', pos);
            *it = conv(values.substr(pos, token - pos));
            pos = token;
        }
        return it;
    }

    template <class I, class ConvFn>
    static I toArray (const string& values, I inserter, ConvFn conv)
    {
        string result; I it = inserter;
        for (uint pos = 0; pos != string::npos; it++)
        {
            uint token = values.find(',', pos);
            *it = conv(values.substr(pos, token - pos));
            pos = token;
        }
        return it;
    }
};

//////////////////////////////////////////////////////////////////////////

class StringValue
{
public:
    StringValue ();

    StringValue (const char* s)     : m_value(s) {}

    StringValue (const string& s)   : m_value(s) {}

    #ifdef QT_CORE_LIB
    StringValue (const QString& s)  : m_value(s.toStdString()) {}
    #endif

    StringValue (bool s)            : m_value(Convert::toString(s)) {}

    StringValue (char s)            : m_value(Convert::toString(s)) {}

    StringValue (byte s)            : m_value(Convert::toString(s)) {}

    StringValue (short s)           : m_value(Convert::toString(s)) {}

    StringValue (ushort s)          : m_value(Convert::toString(s)) {}

    StringValue (int s)             : m_value(Convert::toString(s)) {}

    StringValue (uint s)            : m_value(Convert::toString(s)) {}

    StringValue (long s)            : m_value(Convert::toString(s)) {}

    StringValue (ulong s)           : m_value(Convert::toString(s)) {}

    StringValue (int64 s)           : m_value(Convert::toString(s)) {}

    StringValue (double s, int digit = -1) : m_value(Convert::toString(s, digit)) {}

public:
    const char*     c_str   () const { return m_value.c_str(); }

    const string&   str     () const { return m_value; }

    string&         str     ()       { return m_value; }

    operator const char*    () const { return m_value.c_str(); }

    operator const string&  () const { return m_value; }

    operator string&        ()       { return m_value; }

protected:
    string m_value;
};

//////////////////////////////////////////////////////////////////////////

class StringStream : public Stream
{
public:
    StringStream (const string& source);
    StringStream (string& source);

    operator const string&      ()  { return m_str; }

    const string&   str         ()  { return m_str; }

    virtual bool    canRead     ()  { return true;  }

    virtual bool    canWrite    ()  { return !m_readonly;  }


    virtual int     seek        (int offset, int origin);

    virtual int     position    ()  { return m_pos; }

    virtual int     length      ()  { return m_str.length();  }

    virtual void    setLength   (int value);


    virtual int     read        (void* data, int offset, int size);

    virtual int     write       (const void* data, int offset, int size);

    virtual int     read        ();

    virtual void    write       (byte value);

protected:
    string&  m_str;
    int      m_pos;
    bool     m_readonly;
};

END_NAMESPACE_LIB

#endif//LIB_CONVERT_H
