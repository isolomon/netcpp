#include "utils.h"
#include "files.h"
#include "errors.h"
#include "md5.h"
#include "binary.h"
#include "datetime.h"

#include <stdarg.h>
#include <time.h>
#ifdef __linux__
#include <unistd.h>
#endif

using namespace lib;

int tickcount()
{
    #ifdef __linux__
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return tm.tv_sec * 1000 + tm.tv_nsec / 1000000;
    #else
    return 0;
    #endif
}

int tickcount_s()
{
#ifdef __linux__
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return tm.tv_sec;
#else
    return 0;
#endif
}


int64 tickcount_us()
{
#ifdef __linux__
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return (int64)tm.tv_sec * 1000000 + tm.tv_nsec / 1000;
#else
    return 0;
#endif
}

int64 tickcount_ns()
{
#ifdef __linux__
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return (int64)tm.tv_sec * 1000000000 + tm.tv_nsec;
#else
    return 0;
#endif
}

int benchmark (int& mark, const char* fmt)
{
    int tick = tickcount();
    int used = mark ? tick - mark : 0;
    mark = tick;

    if (fmt)
    {
        if (strstr(fmt, "%s"))
        {
            string text;

            if (used < 10000)  text = format("%d ms", used); // < 10 sec
            else if (used < 600000) text = format("%0.3f seconds", used / 1000.0); // < 10 min
            else text = TimeSpan::fromTicks(used).toString(TimeSpan::WithMs);

            logmsg(fmt, text.c_str());
        }
        else
        {
            logmsg(fmt, used);
        }
    }

    return used;
}

int benchmark(const char* fmt)
{
    static int mark = 0;
    return benchmark(mark, fmt);
}

int msleep(int ms)
{
#ifdef __linux__
    usleep(ms * 1000);
#else
    return 0;
#endif
}

int alignToSeconds(int& lastTick)
{
    int tick = tickcount(), remain = 0;

    if (lastTick == 0 || tick < lastTick + 1000)
    {
        remain = 1000 - tick % 1000;
        logmsg("  sleep %d ms to align seconds\n", remain);
        msleep(remain);
    }

    lastTick = tick + remain;
    return remain;
}

int alignToPeriod(int& lastTick, int period)
{
    int tick = tickcount(), remain = 0;

    if (tick < lastTick + period)
    {
        remain = lastTick + period - tick;
        //log_verb("  sleep %d ms to align period\n", remain);
        msleep(remain);
    }

    lastTick = tick + remain;
    return remain;
}

string localDate()
{
    time_t t;
    time(&t);

    struct tm *p = localtime(&t);
    return format("%04d-%02d-%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
}

string localCompactDate(bool withYear)
{
    time_t t;
    time(&t);

    struct tm *p = localtime(&t);

    if (withYear) return format("%02d%02d%02d", (p->tm_year + 1900) % 100, p->tm_mon + 1, p->tm_mday);
    return format("%02d%02d", p->tm_mon + 1, p->tm_mday);
}

string localTime()
{
    time_t t;
    time(&t);

    struct tm *p = localtime(&t);
    return format("%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);
}

string localDateTime()
{
    time_t t;
    time(&t);

    struct tm *p = localtime(&t);
    return format("%04d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

//////////////////////////////////////////////////////////////////////////
//
// string utilities
//
//////////////////////////////////////////////////////////////////////////
string format(const char* fmt, ...)
{
    int   size = 4096;
    char  stack [4096];
    char* buffer = stack;

    va_list args;
    va_start (args, fmt);

    int num = vsnprintf(buffer, size, fmt, args);
    if (num >= size)
    {
        size = num + 1;
        buffer = new char[size];
        num = vsnprintf(buffer, size, fmt, args);
    }

    va_end (args);
    
    if (num < 0) throw FormatException();

    string result(buffer, num);

    if (buffer != stack) delete[] buffer;

    return result;
}

string& toUpper(string& value)
{
    std::transform(value.begin(), value.end(), value.begin(), ::toupper);
    return value;
}

string toUpper(const string& value)
{
    string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

string upperWords(const string& value)
{
    string result = value;
    return upperWords(result);
}

string& upperWords(string& value)
{
    bool first = true;

    for (int n = 0; n < value.size(); n++)
    {
        char& ch = value[n];

        if (first && ::isalpha(ch)) ch = ::toupper(ch);
        first = !::isalpha(ch);
    }

    return value;
}

bool equalNoCase(const string& str1, const string& str2)
{
#ifdef WIN32
    return stricmp(str1.c_str(), str2.c_str()) == 0;
#else
    return strcasecmp(str1.c_str(), str2.c_str()) == 0;
#endif
}

string& toLower(string& value)
{
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value;
}

string toLower(const string& value)
{
    string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool startWith(const string& value, const string& leading)
{
    if (leading.size() > value.size()) return false;
    return std::equal(leading.begin(), leading.end(), value.begin());
}

bool startWith(const string& value, const char* leading)
{
    int len = strlen(leading);
    if (len > value.size()) return false;
    return std::equal(leading, leading + len, value.begin());
}

bool startWith(const char* value, const char* leading)
{
    int len1 = strlen(value);
    int len2 = strlen(leading);

    if (len2 > len1) return false;
    return std::equal(leading, leading + len2, value);
}

bool endWith(const string& value, const string& ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool endWith(const string& value, const char* ending)
{
    int len = strlen(ending);
    if (len > value.size()) return false;
    return std::equal(ending, ending + len, value.end() - len);
}

bool endWith(const char* value, const char* ending)
{
    int len1 = strlen(value);
    int len2 = strlen(ending);

    if (len2 > len1) return false;
    return std::equal(ending, ending + len2, value + len1 - len2);
}

bool contains(const string& value, const char* str)
{
    return value.find(str) != string::npos;
}

bool contains(const string& value, char ch)
{
    return value.find(ch) != string::npos;
}

bool contains(const char* value, char ch)
{
   while (*value) if (*value++ == ch) return true;
   return false;
}

bool contains(const char* value, const char* str)
{
    return strstr(value, str);
}

string trim(const string& value, const char* delims)
{
    if (value.empty()) return value;

    size_t from = value.find_first_not_of(delims);
    if (from == string::npos) return string();

    size_t to   = value.find_last_not_of(delims) + 1;
    return value.substr(from, to - from);
}

string& trim(string& value, const char* delims)
{
    if (value.empty()) return value;

    value.erase(0, value.find_first_not_of(delims));
    value.erase(value.find_last_not_of(delims) + 1);

    return value;
}

string trimStart(const string& value, const char* delims)
{
    if (value.empty()) return value;
    return value.substr(value.find_first_not_of(delims));
}

string& trimStart(string& value, const char* delims)
{
    if (value.empty()) return value;
    value.erase(0, value.find_first_not_of(delims));
    return value;
}

string trimEnd(const string& value, const char* delims)
{
    if (value.empty()) return value;
    return value.substr(0, value.find_last_not_of(delims) + 1);
}

string& trimEnd(string& value, const char* delims)
{
    if (value.empty()) return value;
    value.erase(value.find_last_not_of(delims) + 1);
    return value;
}

string& replace(string& value, const string& from, const string& to)
{
    for (size_t pos = 0; pos != string::npos; pos += to.length())
    {   
        if ((pos = value.find(from, pos)) != string::npos)
            value.replace(pos, from.length(), to);
        else break;   
    }   
    return value;
}

string replace(const string& value, const string& from, const string& to)
{
    string result = value;

    for (size_t pos = 0; pos != string::npos; pos += to.length())
    {   
        if ((pos = result.find(from, pos)) != string::npos)
            result.replace(pos, from.length(), to);
        else break;   
    }   
    return result;
}

string tail(const string& value, int length)
{
    if (length >= value.size()) return value;
    return value.substr(value.size() - length);
}

string removeStart(const string& value, const string& head)
{
    if (value.size() >= head.size() && value.compare(0, head.size(), head) == 0) return value.substr(head.size());
    return value;
}

string& removeStart(string& value, const string& head)
{
    if (value.size() >= head.size() && value.compare(0, head.size(), head) == 0) return value.erase(0, head.size());
    return value;
}

string removeEnd(const string& value, const string& ends)
{
    int pos = value.size() - ends.size();
    if (pos >= 0 && value.compare(pos, ends.size(), ends) == 0) return value.substr(0, pos);
    return value;
}

string& removeEnd(string& value, const string& ends)
{
    int pos = value.size() - ends.size();
    if (pos >= 0 && value.compare(pos, ends.size(), ends) == 0) value.erase(pos);
    return value;
}

string groupDigits(const string& value, int groupLen, char groupSep, char decimalMark)
{
    string result = value;

    uint end = value.find(decimalMark);
    if (end == string::npos) end = value.length();

    for (int pos = end - groupLen; pos > 0; pos -= groupLen)
    {
        if (isdigit(result[pos - 1]) == false) break;
        result.insert(pos, 1, groupSep);
    }

    return result;
}

string md5sum(const void* data, int size)
{
    byte digits[16];
    MD5((unsigned char*)data, size, digits);
    return Hex::toString(digits, 16);
}

string md5sum(const string& data)
{
    byte digits[16];
    MD5((unsigned char*)data.c_str(), data.size(), digits);
    return Hex::toString(digits, 16);
}

strings split(const string& value, const char* delims)
{
    strings result;
    
    for (uint pos = 0, end = 0; end != string::npos ; pos++)
    {
        end = value.find_first_of(delims, pos);
        result.push_back(value.substr(pos, end - pos));
        pos = end;
    }

    return result;
}

#ifdef __linux__
char* itoa(int value, char* result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }
    
    const char* digits = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = digits[35 + (tmp_value - value * base)];
    } while ( value );

    // apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

#endif