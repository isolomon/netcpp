#ifndef LIB_VERSION_H
#define LIB_VERSION_H

#include "types.h"

#undef major
#undef minor

BEGIN_NAMESPACE_LIB

class Version
{
public:
    static const uint NotPresent = (uint)-1;

    Version(uint major = 0, uint minor = 0, uint patch = NotPresent, uint build = NotPresent, const char* extension = 0);
    Version(const char* text);
    Version(const string& text);
    Version(const Version& other);

public:
    inline uint     major       () const  { return m_major == NotPresent ? 0 : m_major; }
    inline uint     minor       () const  { return m_minor == NotPresent ? 0 : m_minor; }
    inline uint     patch       () const  { return m_patch == NotPresent ? 0 : m_patch; }
    inline uint     build       () const  { return m_build == NotPresent ? 0 : m_build; }
    inline const string& extension () const { return m_extension; }

    string          text        () const;

    void assign (const string& text);

    void assign (const Version& other);

    void assign (uint major, uint minor, uint revision = NotPresent, uint build = NotPresent, const char* extension = 0);

    Version& operator = (const Version& other);

    bool operator < (const Version& other) const;
    bool operator > (const Version& other) const;
    bool operator != (const Version& other) const;
    bool operator <= (const Version& other) const { return ! operator > (other); }
    bool operator >= (const Version& other) const { return ! operator < (other); }
    bool operator == (const Version& other) const { return ! operator != (other); }

    int  extensionCompare (const string& left, const string& right) const;

protected:
    uint    m_major;
    uint    m_minor;
    uint    m_patch;
    uint    m_build;
    string  m_extension;
};

END_NAMESPACE_LIB

#endif //LIB_VERSION_H
