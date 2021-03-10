#ifndef LIB_ERRORS_H
#define LIB_ERRORS_H

#include "types.h"

void print_trace();

BEGIN_NAMESPACE_LIB

struct Error { enum Codes
{
    None                =  0,
    General             = -1,
    Timeout             = -2,
    Denied              = -3,
    Aborted             = -4,
    NotReady            = -5,
    InProgress          = -6,
    Suspended           = -7,    
    Closed              = -8,
    NotFound            = -9,
    AlreadyExists       = -10,
    
    NotSupported        = -11,
    NotImplemented      = -12,
    InvalidArgument     = -13,
    InvalidOperation    = -14,
    NotEnoughSpace      = -15,
    OutOfMemory         = -17,

    IO                  = -20,
    EndOfStream         = -21,
    Format              = -22,
    Socket              = -23,
    Protocol            = -24,
    TooManyOpens        = -25,
    TooManyRetries      = -26,

    Hardware            = -91,
    Internal            = -92,
}; };

enum
{
    NoError = 0,
    GeneralExceptionType,
    InvalidArgumentExceptionType,
    InvalidOperationExceptionType,
    BufferOverflowExceptionType,
    IndexOutOfRangeExceptionType,
    NotSupportedExceptionType,
    NotImplementedExceptionType,
    NotFoundExceptionType,
    AbortedExceptionType,
    RuntimeExceptionType,
    FormatExceptionType,
    IOExceptionType,
    EndOfStreamExceptionType,
    FileNotFoundExceptionType,
    TimeoutExceptionType,
    SocketExceptionType,
    XmlExceptionType,
    ProtocolExceptionType,
    HardwareExceptionType,

    UserExceptionBase = 1000,
};

class Exception
{
public:
    int type () { return m_type; }
    const char* message () { return m_message; }

    Exception(int type = 0, const char* msg = 0) : m_type(type)
    {
        m_message[0] = 0;
        if (msg) strncat(m_message, msg, 127);
    }

protected:
    int m_type;
    char m_message[128];
};

// Timeout is a special exception since it is thrown all the time
class TimeoutException : public Exception
{
public:
    TimeoutException(const char* msg = 0) : Exception(TimeoutExceptionType, msg)
    {
        if (msg) log_info("\n***** Timeout Exception: %s *****\n", msg);
    }
};

class SystemException : public Exception
{
public:
    SystemException(int type = 0, const char* msg = 0) : Exception(type, msg)
    {
        if (msg) log_error("\n***** System Exception: %s *****\n", msg);
    }
};

class InvalidArgumentException : public SystemException
{
public:
    InvalidArgumentException(const char* msg = 0) : SystemException(InvalidArgumentExceptionType, msg) {}
};

class InvalidOperationException : public SystemException
{
public:
    InvalidOperationException(const char* msg = 0) : SystemException(InvalidOperationExceptionType, msg) {}
};

class IndexOutOfRangeException : SystemException
{
public:
    IndexOutOfRangeException(const char* msg = 0) : SystemException(IndexOutOfRangeExceptionType, msg) {}
};

class BufferOverflowException : SystemException
{
public:
    BufferOverflowException(const char* msg = 0) : SystemException(BufferOverflowExceptionType, msg) {}
};

class NotFoundException : public SystemException
{
public:
    NotFoundException(const char* msg = 0) : SystemException(NotFoundExceptionType, msg) {}
};

class NotSupportedException : SystemException
{
public:
    NotSupportedException(const char* msg = 0) : SystemException(NotSupportedExceptionType, msg) {}
};

class NotImplementedException : public SystemException
{
public:
    NotImplementedException(const char* msg = 0) : SystemException(NotImplementedExceptionType, msg) {}
};

class AbortedException : public SystemException
{
public:
    AbortedException(const char* msg = 0) : SystemException(AbortedExceptionType, msg) {}
};

class IOException : public SystemException
{
public:
    IOException(const char* msg = 0) : SystemException(IOExceptionType, msg) {}
};

class EndOfStreamException : public IOException
{
public:
    EndOfStreamException(const char* msg = 0) : IOException(msg) {}
};

class FileNotFoundException : IOException
{
public:
    FileNotFoundException(const char* msg = 0) : IOException(msg) {}
};

class SocketException : public SystemException
{
public:
    int m_socketError;
    SocketException(const char* msg = 0) : SystemException(SocketExceptionType, msg) {}
};

class XmlException : public SystemException
{
public:
    XmlException(const char* msg = 0) : SystemException(XmlExceptionType, msg) {}
};

class RuntimeException : public SystemException
{
public:
    RuntimeException(const char* msg = 0) : SystemException(RuntimeExceptionType, msg) {}
};

class FormatException : public SystemException
{
public:
    FormatException(const char* msg = 0) : SystemException(FormatExceptionType, msg) {}
};

class ProtocolException : public SystemException
{
public:
    ProtocolException(const char* msg = 0) : SystemException(ProtocolExceptionType, msg) {}
};

class HardwareException : public SystemException
{
public:
    HardwareException(const char* msg = 0) : SystemException(HardwareExceptionType, msg) {}
};

END_NAMESPACE_LIB

#endif//LIB_ERRORS_H