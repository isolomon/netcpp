#include "datetime.h"
#include "utils.h"
#include "errors.h"
#include <time.h>

BEGIN_NAMESPACE_LIB

TimeSpan::TimeSpan(int64 ticks)
{
    m_ticks = ticks;
}

TimeSpan::TimeSpan(int h, int m, int s)
{
    m_ticks = (int64)h * TicksPerHour + (int64)m * TicksPerMin + (int64)s * TicksPerSec;
}

TimeSpan::TimeSpan(int d, int h, int m, int s, int t)
{
    m_ticks = (int64)d * TicksPerDay + (int64)h * TicksPerHour + (int64)m * TicksPerMin + (int64)s * TicksPerSec + t;
}

TimeSpan::TimeSpan(const TimeSpan& other)
{
    m_ticks = other.m_ticks;
}

string TimeSpan::toString(int fmt)
{
    bool withMs   = hasFlag(fmt, WithMs);
    bool withDays = hasFlag(fmt, WithDays);

    int h = withDays ? hours() : abs_t(totalHours());
    string result = format("%02d:%02d:%02d", h, minutes(), seconds());

    int d  = days();
    int ms = milliseconds();

    if (withDays & d) result = format("%d:", d) + result;
    if (withMs && ms) result += format(".%03d", ms);

    if (m_ticks < 0) result.insert(0, 1, '-');

    return result;
}

TimeSpan TimeSpan::fromString(const string& text)
{
    enum State { Pending, inMil, inSec, inMin, inHou, inDay };    
    int day = 0, hou = 0, min = 0, sec = 0, mil = 0;
    
    string token;
    State  state = Pending;

    for (int n = text.size() - 1; n >= -1; --n)
    {
        int c = (n >= 0 ? (byte)text[n] : -1);

        if (c == '.' || c == ':' || c == -1)
        {
            if (state == Pending) state = (c == '.' ? inMil : inSec);

            trim(token);

            if (token.empty()) throw FormatException();
            else if (state == inMil) mil = round(Convert::toFloat('.' + token) * 1000);
            else if (state == inSec) sec = Convert::toInt(token, 10);
            else if (state == inMin) min = Convert::toInt(token, 10);
            else if (state == inHou) hou = Convert::toInt(token, 10);
            else if (state == inDay) day = Convert::toInt(token, 10);

            if (c == -1) break;

            token.clear();

            if (c == '.')
            {
                if      (state == inMil) state = inSec;
                else if (state == inHou) state = inDay;
                else throw FormatException();
            }
            else if (c == ':')
            {
                if      (state == inSec) state = inMin;
                else if (state == inMin) state = inHou;
                else if (state == inHou) state = inDay;
                else throw FormatException();
            }
        }
        else
        {
            token.insert(token.begin(), c);
        }
    }

    if (state > inSec)
    {
        if (sec > 59) throw FormatException();
        if (min > 59) throw FormatException();
    }

    if (state > inHou && hou > 23) throw FormatException();

    return TimeSpan(day, hou, min, sec, mil);
}

//////////////////////////////////////////////////////////////////////////
DateTime::DateTime(int y, int m, int d, int h, int n, int s)
{
    m_kind = LocalTime;
    setDateTime(y, m, d, h, n, s);
}

DateTime::DateTime(int y, int m, int d)
{
    m_hour = 0;
    m_minute = 0;
    m_second = 0;

    m_kind = LocalTime;
    setDate(y, m, d);
}

DateTime::DateTime(int comps[], int count)
{
    m_kind = LocalTime;
    setDateTime(comps, count);
}

DateTime::DateTime(int64 seconds, DateTimeKind kind)
{
    m_epoch = seconds;
    m_kind  = kind;

    updateComponents();
}

void DateTime::setYear(int value)
{
    if (m_year != value)
    {
        m_year = value;
        updateEpochSeconds();
    }
}

void DateTime::setMonth(int value)
{
    if (m_month != value)
    {
        m_month = value;
        updateEpochSeconds();
    }
}

void DateTime::setDay(int value)
{
    if (m_day != value)
    {
        m_day = value;
        updateEpochSeconds();
    }
}

void DateTime::setHour(int value)
{
    if (m_hour != value)
    {
        m_hour = value;
        updateEpochSeconds();
    }
}

void DateTime::setMinute(int value)
{
    if (m_minute != value)
    {
        m_minute = value;
        updateEpochSeconds();
    }
}

void DateTime::setSecond(int value)
{
    if (m_second != value)
    {
        m_second = value;
        updateEpochSeconds();
    }
}

void DateTime::setDate(int y, int m, int d)
{
    m_year = y;
    m_month = m;
    m_day = d;

    updateEpochSeconds();
}

void DateTime::setTime(int h, int n, int s)
{
    m_hour = h;
    m_minute = n;
    m_second = s;

    updateEpochSeconds();
}

void DateTime::setDateTime(int y, int m, int d, int h, int n, int s)
{
    m_year = y;
    m_month = m;
    m_day = d;
    m_hour = h;
    m_minute = n;
    m_second = s;

    updateEpochSeconds();
}

void DateTime::setDateTime(int comps[], int count)
{
    m_year   = count > 0 ? comps[0] : 0;
    m_month  = count > 1 ? comps[1] : 0;
    m_day    = count > 2 ? comps[2] : 0;
    m_hour   = count > 3 ? comps[3] : 0;
    m_minute = count > 4 ? comps[4] : 0;
    m_second = count > 5 ? comps[5] : 0;

    updateEpochSeconds();
}

void DateTime::setEpochSeconds(int64 value)
{
    m_epoch = value;
    updateComponents();
}

void DateTime::setKind(DateTimeKind value)
{
    if (m_kind != value)
    {
        m_kind = value;
        updateComponents();
    }
}

string DateTime::dateString() const
{
    return format("%04d-%02d-%02d", m_year, m_month, m_day);
}

string DateTime::timeString() const
{
    return format("%02d:%02d:%02d", m_hour, m_minute, m_second);
}

string DateTime::dateTimeString() const
{
    return format("%04d-%02d-%02d %02d:%02d", m_year, m_month, m_day, m_hour, m_minute);
}

string DateTime::toString() const
{
    return format("%04d-%02d-%02d %02d:%02d:%02d", m_year, m_month, m_day, m_hour, m_minute, m_second);
}

void DateTime::toArray(int comps[], int count) const
{
    if (count > 0) comps[0] = m_year;
    if (count > 1) comps[1] = m_month;
    if (count > 2) comps[2] = m_day;
    if (count > 3) comps[3] = m_hour;
    if (count > 4) comps[4] = m_minute;
    if (count > 5) comps[5] = m_second;
}

void DateTime::addSeconds(int value)
{
    m_epoch += value;
    updateComponents();
}

void DateTime::addTimeSpan(const TimeSpan& value)
{
    m_epoch += value.totalSeconds();
    updateComponents();
}

void DateTime::substractSeconds(int value)
{
    m_epoch -= value;
    updateComponents();
}

void DateTime::substractTimeSpan(const TimeSpan& value)
{
    m_epoch -= value.totalSeconds();
    updateComponents();
}

DateTime DateTime::now()
{
    return DateTime(time(0), LocalTime);
}

DateTime DateTime::utcNow()
{
    return DateTime(time(0), UtcTime);
}

DateTime DateTime::parse(const string& text)
{
    throw NotImplementedException();
}

void DateTime::updateComponents()
{
    time_t t = m_epoch;
    struct tm tp;

#ifdef WIN32
    if (isUtcTime()) tp = *gmtime(&t);
    else tp = *localtime(&t);
#else
    if (isUtcTime()) gmtime_r(&t, &tp);
    else localtime_r(&t, &tp);
#endif

    m_year  = tp.tm_year + 1900;
    m_month = tp.tm_mon + 1;
    m_day   = tp.tm_mday;
    m_hour  = tp.tm_hour;
    m_minute = tp.tm_min;
    m_second = tp.tm_sec;
}

void DateTime::updateEpochSeconds()
{
    struct tm tp;

    tp.tm_year = m_year - 1900;
    tp.tm_mon  = m_month - 1;
    tp.tm_mday = m_day;
    tp.tm_hour = m_hour;
    tp.tm_min  = m_minute;
    tp.tm_sec  = m_second;
    tp.tm_isdst = 0;

#ifdef WIN32
    m_epoch = isUtcTime() ? _mkgmtime(&tp) : mktime(&tp);
#else
    m_epoch = isUtcTime() ? timegm(&tp) : mktime(&tp);
#endif
}

END_NAMESPACE_LIB

