#ifndef LIB_DATETIME_H
#define LIB_DATETIME_H

#include "types.h"

BEGIN_NAMESPACE_LIB

class TimeSpan
{
public:
    enum { Normal = 0, WithDays = 1, WithMs = 2, WithDaysAndMs = 3, };

public:
    TimeSpan (int64 ticks = 0);

    TimeSpan (int h, int m, int s);

    TimeSpan (int d, int h, int m, int s, int ms);

    TimeSpan (const TimeSpan& other);

    inline int      days            () const { return abs_t(m_ticks) / TicksPerDay;        }

    inline int      hours           () const { return abs_t(m_ticks) / TicksPerHour % 24;  }

    inline int      minutes         () const { return abs_t(m_ticks) / TicksPerMin % 60;   }

    inline int      seconds         () const { return abs_t(m_ticks) / TicksPerSec % 60;   }

    inline int      milliseconds    () const { return abs_t(m_ticks) % TicksPerSec;        }


    inline double   totalDays       () const { return m_ticks / (double)TicksPerDay; }

    inline double   totalHours      () const { return m_ticks / (double)TicksPerHour;}

    inline double   totalMinutes    () const { return m_ticks / (double)TicksPerMin; }

    inline double   totalSeconds    () const { return m_ticks / (double)TicksPerSec; }

    inline int64    totalTicks      () const { return m_ticks;   }

    string          toString        (int fmt = Normal);
    

    inline void     add             (int ticks)             { m_ticks += ticks; }

    inline void     subtract        (int ticks)             { m_ticks -= ticks; }

    inline void     add             (const TimeSpan& other) { m_ticks += other.m_ticks; }

    inline void     subtract        (const TimeSpan& other) { m_ticks -= other.m_ticks; }

    inline int      compare         (int tick)              const { return ::compare(m_ticks, tick); }

    inline int      compare         (const TimeSpan& other) const { return ::compare(m_ticks, other.m_ticks); }

    inline bool operator == (const TimeSpan& other) const { return m_ticks == other.m_ticks; }

    inline bool operator != (const TimeSpan& other) const { return m_ticks != other.m_ticks; }

    inline bool operator <= (const TimeSpan& other) const { return m_ticks <= other.m_ticks; }

    inline bool operator >= (const TimeSpan& other) const { return m_ticks >= other.m_ticks; }

    inline bool operator <  (const TimeSpan& other) const { return m_ticks <  other.m_ticks; }

    inline bool operator >  (const TimeSpan& other) const { return m_ticks >  other.m_ticks; }

    TimeSpan& operator = (const TimeSpan& other)  { m_ticks = other.m_ticks;  return *this;  }

    operator string ()  { return toString(); }


    static TimeSpan fromDays        (double days)  { return TimeSpan(days * TicksPerDay);   }

    static TimeSpan fromHours       (double hours) { return TimeSpan(hours * TicksPerHour); }

    static TimeSpan fromMinutes     (double mins)  { return TimeSpan(mins * TicksPerMin);   }

    static TimeSpan fromSeconds     (double secs)  { return TimeSpan(secs * TicksPerSec);   }

    static TimeSpan fromTicks       (int64 ticks)  { return TimeSpan(ticks); }

    static TimeSpan fromString      (const string& text);

public:
    static const int TicksPerDay  = 3600000 * 24;
    static const int TicksPerHour = 3600000;
    static const int TicksPerMin  = 60000;
    static const int TicksPerSec  = 1000;

protected:
    int64     m_ticks;
};

inline TimeSpan operator + (const TimeSpan& l, const TimeSpan& r)   { return TimeSpan(l.totalTicks() + r.totalTicks());}
inline TimeSpan operator + (const TimeSpan& l, int64 r)             { return TimeSpan(l.totalTicks() + r); }
inline TimeSpan operator + (int64 l, const TimeSpan& r)             { return TimeSpan(l + r.totalTicks()); }

inline TimeSpan operator - (const TimeSpan& l, const TimeSpan& r)   { return TimeSpan(l.totalTicks() - r.totalTicks());}
inline TimeSpan operator - (const TimeSpan& l, int64 r)             { return TimeSpan(l.totalTicks() - r); }
inline TimeSpan operator - (int64 l, const TimeSpan& r)             { return TimeSpan(l - r.totalTicks()); }

enum DateTimeKind
{
    UtcTime = 0,
    LocalTime = 1,
};

//////////////////////////////////////////////////////////////////////////
class DateTime
{
public:
    DateTime (int64 seconds = 0, DateTimeKind kind = LocalTime);

    DateTime (int y, int m, int d, int h, int n, int s);

    DateTime (int y, int m, int d);

    DateTime (int comps[], int count);

public:
    inline int year    () const  { return m_year;    }

    inline int month   () const  { return m_month;   }

    inline int day     () const  { return m_day;     }

    inline int hour    () const  { return m_hour;    }

    inline int minute  () const  { return m_minute;  }

    inline int second  () const  { return m_second;  }

    inline int64 epochSeconds () const { return m_epoch;   }

    inline DateTimeKind kind() const  { return (DateTimeKind) m_kind; }

    inline bool isUtcTime   () const  { return m_kind == UtcTime;  }


    void    setYear    (int value);

    void    setMonth   (int value);

    void    setDay     (int value);

    void    setHour    (int value);

    void    setMinute  (int value);

    void    setSecond  (int value);

    void    setDate    (int y, int m, int d);

    void    setTime    (int h, int n, int s);

    void    setDateTime (int y, int m, int d, int h, int n, int s);

    void    setDateTime (int comps[], int count);

    void    setEpochSeconds (int64 value);

    void    setKind     (DateTimeKind value);


    string  dateString  () const;

    string  timeString  () const;

    string  dateTimeString () const;

    string  toString    () const;

    void    toArray     (int comps[], int count) const;

    void    addSeconds  (int value);

    void    addTimeSpan (const TimeSpan& value);

    void    substractSeconds  (int value);

    void    substractTimeSpan (const TimeSpan& value);

    //////////////////////////////////////////////////////////////////////////
    bool    equal       (const DateTime& other) { return m_epoch == other.m_epoch; }

    bool    operator == (const DateTime& other) { return m_epoch == other.m_epoch; }

    bool    operator != (const DateTime& other) { return m_epoch != other.m_epoch; }

    bool    operator <  (const DateTime& other) { return m_epoch <  other.m_epoch; }

    bool    operator <= (const DateTime& other) { return m_epoch <= other.m_epoch; }

    bool    operator >  (const DateTime& other) { return m_epoch >  other.m_epoch; }

    bool    operator >= (const DateTime& other) { return m_epoch >= other.m_epoch; }

    operator string () const { return toString(); }

public:
    static DateTime now     ();

    static DateTime utcNow  ();

    static DateTime parse   (const string& text);

protected:
    void   updateEpochSeconds ();
    void   updateComponents ();
    
protected:
    int64   m_epoch;
    int     m_year;
    char    m_month;
    char    m_day;
    char    m_hour;
    char    m_minute;
    char    m_second;

    char    m_kind;
};

END_NAMESPACE_LIB

#endif //LIB_DATETIME_H
