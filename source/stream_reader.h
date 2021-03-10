#ifndef LIB_STREAM_READER_H
#define LIB_STREAM_READER_H

#include "reader_base.h"

BEGIN_NAMESPACE_LIB

//////////////////////////////////////////////////////////////////////////
//
class StreamReader : public Reader
{
public:
    StreamReader (Stream* stream, bool ownStream = false, int bufferSize = READER_BUFSIZE);

    StreamReader (const char* filename, int bufferSize = READER_BUFSIZE);

    StreamReader (const string& filename, int bufferSize = READER_BUFSIZE);

    StreamReader (const void* data, int offset, int size);
    
public:
    bool        startWith       (const char* value, int len, bool caseSensitive);

    bool        startWith       (const char* value, bool caseSensitive = true) { return startWith(value, strlen(value), caseSensitive); }


    int         skipBlanks      ()  { return skipCharacters(" \t"); }

    int         skipWhitespace  ()  { return skipCharacters(" \t\r\n\v\f"); }

    int         skipCharacters  (const char* chars);

    bool        moveToNextLine  (int lines = 1);

    bool        moveToLine      (const char* line);
    
    string      readLine        ();

    string      readToEnd       ();


    bool        readTo          (char value, bool stepOver, string* result);

    bool        readTo          (const char* value, bool stepOver, string* result);

    bool        moveTo          (char value, bool stepOver = true)          { return readTo(value, stepOver, 0); }

    bool        moveTo          (const char* value, bool stepOver = true)   { return readTo(value, stepOver, 0); }

    string      readTo          (char value, bool stepOver = true)          { string r; readTo(value, stepOver, &r); return r; }

    string      readTo          (const char* value, bool stepOver = true)   { string r; readTo(value, stepOver, &r); return r; }


    bool        readToFirstOf   (const char* chars, bool stepOver, string* result);

    bool        moveToFirstOf   (const char* chars, bool stepOver = true)  { return readToFirstOf(chars, stepOver, 0); }

    string      readToFirstOf   (const char* chars, bool stepOver = true)  { string r; readToFirstOf(chars, stepOver, &r); return r; }


    bool        readEscapedTo   (char value, bool stepOver, char escape, string* result);
    
    string      readEscapedTo   (char value, bool stepOver = true, char escape = '\\') { string r; readEscapedTo(value, stepOver, escape, &r); return r; }

    
    string      readQuoted      (const char* marks= "\"", char escape = '\\');

    string      readToken       (const char* extra = 0, bool stepOver = true);

    int         readToken       (const char* extra, bool stepOver, char* buffer, int size);


    int         readTokenAsInt   (const char* extra = 0, bool stepOver = true);

    uint        readTokenAsUInt  (const char* extra = 0, bool stepOver = true);

    int64       readTokenAsInt64 (const char* extra = 0, bool stepOver = true);

    double      readTokenAsFloat (const char* extra = 0, bool stepOver = true);

    bool        readTokenAsBool  (const char* extra = 0, bool stepOver = true);

protected:
    bool        isToken         (int c, const char* extra);

    inline bool isCrLf          (int c) { return c == '\r' || c == '\n'; }
};

//////////////////////////////////////////////////////////////////////////
class StringReader : public StreamReader
{
public:
    StringReader(const string& data) : StreamReader(data.c_str(), 0, data.size()) {}

    StringReader(const char* data) : StreamReader(data, 0, strlen(data)) {}
};

END_NAMESPACE_LIB

#endif //LIB_STREAM_READER_H
