#ifndef LIB_UTILS_H
#define LIB_UTILS_H

#include <math.h>
#include <algorithm>

#include "convert.h"

int     tickcount      ();

int     tickcount_s    ();

int64   tickcount_us   ();

int64   tickcount_ns   ();

int     msleep         (int ms);

int     benchmark      (const char* fmt = 0);

int     benchmark      (int& mark, const char* fmt = 0);

int     alignToSeconds (int& lastTick);
                       
int     alignToPeriod  (int& lastTick, int period);

                       
string  localDate      ();      //2018-09-12

string  localCompactDate (bool withYear = true);    // [18]0912
                       
string  localTime      ();      // 18:02:08
                       
string  localDateTime  ();      // 2018-09-12 18:02:08

//////////////////////////////////////////////////////////////////////////
string  format      (const char* fmt, ...);

strings split       (const string& value, const char* delims = "\r\n\t ");

string  toLower     (const string& value);

string& toLower     (string& value);

string  toUpper     (const string& value);

string& toUpper     (string& value);

string  upperWords  (const string& value);

string& upperWords  (string& value);

bool    equalNoCase (const string& str1, const string& str2);

bool    startWith   (const string& value, const string& leading);

bool    startWith   (const string& value, const char* leading);

bool    startWith   (const char*   value, const char* leading);


bool    endWith     (const string& value, const string& ending);

bool    endWith     (const string& value, const char* ending);

bool    endWith     (const char*   value, const char* ending);


bool    contains    (const string& value, const char* str);

bool    contains    (const char*   value, const char* str);

bool    contains    (const string& value, char ch);

bool    contains    (const char*   value, char ch);


string  trimStart   (const string& value, const char* delims = "\r\n\t ");

string  trimEnd     (const string& value, const char* delims = "\r\n\t ");

string  trim        (const string& value, const char* delims = "\r\n\t ");

string  replace     (const string& value, const string& from, const string& to);

string  tail        (const string& value, int count);


string& trimStart   (string& value, const char* delims = "\r\n\t ");

string& trimEnd     (string& value, const char* delims = "\r\n\t ");

string& trim        (string& value, const char* delims = "\r\n\t ");

string& replace     (string& value, const string& from, const string& to);


string  removeStart (const string& value, const string& ends);

string  removeEnd   (const string& value, const string& ends);

string& removeStart (string& value, const string& ends);

string& removeEnd   (string& value, const string& ends);

string  groupDigits (const string& value, int groupLen = 3, char groupSep = ',', char decimalMark = '.');

string  md5sum      (const void* data, int size);

string  md5sum      (const string& data);

template <class Pred>
int strncpy_if  (char* dest, const char* source, int num, Pred cond)
{
    char* begin = dest;
    char* end = dest + num - 1;

    for (; dest < end && *source; ++source) if (cond(*source)) *dest++ = *source;

    if (num > 0) *dest = 0;
    return dest - begin;
}

template <class Container, class T>
inline int indexOf (const Container& items, const T& elem)
{
    typename Container::const_iterator it = std::find(items.begin(), items.end(), elem);
    return it == items.end() ? -1 : it - items.begin();
}

template <class Container, class T>
inline int removeFrom (Container& items, const T& elem)
{
    typename Container::iterator it = std::find(items.begin(), items.end(), elem);
    if (it != items.end()) { items.erase(it); return true; } else return false;
}

template <class Container, class T>
inline int binarySearch (const Container& items, const T& value)
{
    typename Container::const_iterator it = std::lower_bound(items.begin(), items.end(), value);
    return (it != items.end() && *it == value) ? it - items.begin() : ~(it - items.begin());
}

template <class Container, class T, class Comp>
inline int binarySearch (const Container& items, const T& value, Comp comp)
{
    typename Container::const_iterator it = std::lower_bound(items.begin(), items.end(), value, comp); 
    return (it != items.end() && !comp(*it, value)) ? it - items.begin() : ~(it - items.begin());
}

template <class I, class T>
inline void offsetValues (I first, I last, const T& offset)
{
    for (I it = first; it != last; ++it) *it += offset;
}

template <class I, class T>
inline void resetValues (I first, I last, const T& value)
{
    for (I it = first; it != last; ++it) *it = value;
}

template <class I, class T>
inline T accumulate (I first, I last, T init)
{
    for (I it = first; it != last; ++it) init += *it;
    return init;
}

template <class I>
inline void deleteItems (I first, I last)
{
    for (I it = first; it != last; ++it) delete *it;
}

template <class Container>
inline void deleteItems (Container items)
{
    deleteItems(items.begin(), items.end());
}

template <class I>
inline void releaseItems (I first, I last)
{
    for (I it = first; it != last; ++it) (*it)->release();
}

template <class Container>
inline void releaseItems (Container items)
{
    releaseItems(items.begin(), items.end());
}

template <class I>
inline void closeItems (I first, I last)
{
    for (I it = first; it != last; ++it) (*it)->close();
}

template <class Container>
inline void closeItems (Container items)
{
    closeItems(items.begin(), items.end());
}

//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
inline int round(double num)
{
    return num > 0 ? (int)(num + 0.5) : (int)(num - 0.5);
}
#endif

#ifdef __linux__
char* itoa(int value, char* result, int base);
#endif

#endif//LIB_UTILS_H