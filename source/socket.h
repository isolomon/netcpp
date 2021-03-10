#ifndef LIB_SOCKET_H
#define LIB_SOCKET_H

#include "stream.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0600
#include <WinSock2.h>
typedef SOCKET socket_t;
#else
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <netinet/tcp.h>
#include <poll.h>
typedef int socket_t;
#endif

BEGIN_NAMESPACE_LIB

class SocketAddress;

//////////////////////////////////////////////////////////////////////////
class IpAddress
{
public:
    static IpAddress Any;
    static IpAddress Loopback;
    static IpAddress None;

public:
    IpAddress (uint address = 0);
    IpAddress (byte address[], int count);
    IpAddress (const char* address);
    IpAddress (const string& address);
    IpAddress (const SocketAddress& address);

    string toString () const;

    bool operator == (const IpAddress& other) const;
    bool operator != (const IpAddress& other) const;

    static IpAddress parse  (const char* ipstr);
    static IpAddress parse  (const string& ipstr);

private:
    uint m_address;

    friend class Socket;
    friend class SocketAddress;
};

//////////////////////////////////////////////////////////////////////////
class SocketAddress
{
public:
    SocketAddress();
    SocketAddress(const IpAddress& address, int port);

    IpAddress ipAddress();

    int port();

    int size() { return sizeof(m_data); }

private:
    char m_data[16];

    friend class Socket;
    friend class IpAddress;
};

enum SocketCodes
{
    SocketError = -1,
    WouldBlock = -2,
};

//////////////////////////////////////////////////////////////////////////
class Socket
{
public:
    enum AddressFamily  { InterNetwork = 0x02, InterNetworkV6 = 0x17 };
    enum SelectMode     { SelectNone = 0x00, SelectRead = 0x01, SelectWrite = 0x02, SelectError = 0x04 };
    enum ShutdownMode   { ShutdownRead, ShutdownWrite, ShutdownBoth, };
    enum SocketType     { TCP = 1, UDP = 2 };
    enum { InvalidSocket = -1 };

public: //operations
    Socket (AddressFamily family = InterNetwork, SocketType type = TCP);
    virtual ~Socket ();

    void    connect     (const IpAddress& host, int port, int timeout = 15000);

    void    bind        (const IpAddress& host, int port);

    void    listen      (int backlog);

    Socket* accept      ();

    bool    poll        (int milliseconds, SelectMode selectMode);

    int     send        (const void* data, int offset, int size, int flags = 0);

    int     sendTo      (const void* data, int offset, int size, const IpAddress& host, int port, int flags = 0);

    int     receive     (void* data, int offset, int size, int flags = 0);

    int     receiveFrom (void* data, int offset, int size, IpAddress* host = 0, int* port = 0, int flags = 0);

    void    shutdown    (ShutdownMode shutdownMode = ShutdownBoth);

    void    close       ();

    int     solError    ();

    static int error    ();

public: //properties
    inline socket_t handle      ()  { return m_socket;      }

    inline int  family          ()  { return m_family;      }

    inline int  type            ()  { return m_type;        }

    inline bool blocking        ()  { return m_blocking;    }

    inline bool connected       ()  { return m_connected;   }

    int         available       ();

    void        setBlocking     (bool value);    

    IpAddress   localAddress    ();

    IpAddress   remoteAddress   ();

    int         localPort       ();

    int         remotePort      ();

    string      remoteName      ();

    string      localName       ();

    //////////////////////////////////////////////////////////////////////////

    void        setOption           (int level, int name, int  value);
    void        setOption           (int level, int name, const void* value, int length);

    int         getOption           (int level, int name);
    void        getOption           (int level, int name, void* value, int* length);    
    
    bool        reuseAddress        ();
    void        setReuseAddress     (bool value);

    bool        tcpNoDelay          ();
    void        setTcpNoDelay       (bool value);

    bool        keepAlive           ();
    void        setKeepAlive        (bool onoff, int idle = 60, int interval = 10, int count = 10); // unit is in seconds

    bool        lingerOption        (int* time = 0);;
    void        setLingerOption     (bool onoff, int time);

    int         sendBufferSize      ();
    void        setSendBufferSize   (int value);

    int         receiveBufferSize   ();
    void        setReceiveBufferSize (int value);

    int         sendTimeout         ();
    void        setSendTimeout      (int milliseconds);

    int         receiveTimeout      ();
    void        setReceiveTimeout   (int milliseconds);
    
protected:
    Socket(socket_t sock, byte family, byte type, bool block, bool conn);

protected:
    socket_t    m_socket;
    byte        m_family;
    byte        m_type;
    bool        m_blocking;
    bool        m_connected;

    static bool m_initialized;
};

//////////////////////////////////////////////////////////////////////////
class UdpSocket : public Socket
{
public:
    UdpSocket(int port = 0) : Socket (InterNetwork, UDP)
    {
        if (port) bind(IpAddress::Any, port);
    }

    UdpSocket(const IpAddress& ip, int port) : Socket (InterNetwork, UDP)
    {
        bind(ip, port);
    }
};

//////////////////////////////////////////////////////////////////////////
class NetworkStream : public Stream
{
public:
    NetworkStream (Socket* socket, bool own = false);

    NetworkStream (const IpAddress& host, int port, int connectTimeout = 15000);

    virtual ~NetworkStream ();

    inline Socket* socket () { return m_socket;  }

    // properties
    virtual bool canRead         ();
                                 
    virtual bool canWrite        ();
                                 
    virtual bool canTimeout      ();
                                 
    virtual bool canReady        ();
                                 
    virtual int  readTimeout     ();
                                 
    virtual void setReadTimeout  (int timeout);
                                 
    virtual int  writeTimeout    ();
                                 
    virtual void setWriteTimeout (int timeout);


    virtual bool readyRead       (int timeout);

    virtual bool readyWrite      (int timeout);

    // operations
    virtual int  read   (void* data, int offset, int size);

    virtual int  write  (const void* data, int offset, int size);

    virtual void flush  ();

    virtual void close  ();

protected:
    Socket* m_socket;
    int     m_sendTimeout;
    int     m_recvTimeout;
    bool    m_ownSocket;
};

END_NAMESPACE_LIB

#endif //LIB_SOCKET_H
