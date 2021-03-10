#include "version.h"
#include "utils.h"

#undef major
#undef minor

BEGIN_NAMESPACE_LIB

Version::Version(uint major, uint minor, uint patch, uint build, const char* extension)
{
    m_major = major;
    m_minor = minor;
    m_patch = patch;
    m_build = build;

    if (extension) m_extension = extension;
}

Version::Version(const char* text)
{
    assign(string(text));
}

Version::Version(const string& text)
{
    assign(text);
}

Version::Version(const Version& other)
{
    assign(other);
}

Version& Version::operator = (const Version& other)
{
    assign(other);
    return *this;
}

void Version::assign(const Version& other)
{
    m_major = other.m_major;
    m_minor = other.m_minor;
    m_patch = other.m_patch;
    m_build = other.m_build;
    m_extension = other.m_extension;
}

void Version::assign(uint major, uint minor, uint patch, uint build, const char* extension)
{
    m_major = major;
    m_minor = minor;
    m_patch = patch;
    m_build = build;
    m_extension = extension ? extension : "";
}

void Version::assign(const string& text)
{
    m_major = m_minor = m_patch = m_build = NotPresent;
    m_extension.clear();

    uint* comp = &m_major;
    uint  curpos = 0;

    for (int n = 0; n < 4; n++)
    {
        uint pos = text.find_first_of(".-", curpos);
        string token = text.substr(curpos, pos - curpos);
        comp[n] = Convert::toUInt(token);

        if (pos == string::npos) return;

        curpos = pos + 1;
        if (text[pos] == '-') break;        
    }

    m_extension = text.substr(curpos);
}

string Version::text() const
{
    string result = format("%u.%u", major(), minor());

    if (m_patch != NotPresent)
    {
        result += format(".%u", m_patch);
        if (m_build != NotPresent) result += format(".%u", m_build);
    }
    
    if (m_extension.size()) result += format("-%s", m_extension.c_str());

    return result;
}

bool Version::operator < (const Version& other) const
{
    if (major() < other.major()) return true;
    else if (major() > other.major()) return false;

    if (minor() < other.minor()) return true;
    else if (minor() > other.minor()) return false;

    if (patch() < other.patch()) return true;
    else if (patch() > other.patch()) return false;

    if (build() < other.build()) return true;
    else if (build() > other.build()) return false;

    return extensionCompare(m_extension, other.extension()) < 0;
}

bool Version::operator > (const Version& other) const
{
    if (major() > other.major()) return true;
    else if (major() < other.major()) return false;

    if (minor() > other.minor()) return true;
    else if (minor() < other.minor()) return false;

    if (patch() > other.patch()) return true;
    else if (patch() < other.patch()) return false;

    if (build() > other.build()) return true;
    else if (build() < other.build()) return false;

    return extensionCompare(m_extension, other.extension()) > 0;
}

bool Version::operator != (const Version& other) const
{
    if (major() != other.major()) return true;
    if (minor() != other.minor()) return true;
    if (patch() != other.patch()) return true;
    if (build() != other.build()) return true;

    return extensionCompare(m_extension, other.extension()) != 0;
}

int Version::extensionCompare(const string& left, const string& right) const
{
    int leftValue = 0, rightValue = 0;

    if (left.size() > 0)
    {
        leftValue = isalpha(left[0]) ? -1 : Convert::toUInt(left);
    }

    if (right.size() > 0)
    {
        rightValue = isalpha(right[0]) ? -1 : Convert::toUInt(right);
    }

    if (leftValue > -1 || rightValue > -1)
    {
        return leftValue == rightValue ? 0 : (leftValue < rightValue ? -1 : 1);
    }

    // all -1 means they are both start with alpha, compare with string
    return left.compare(right); 
}

END_NAMESPACE_LIB

