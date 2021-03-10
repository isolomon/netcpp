#include "socket.h"
#include "errors.h"
#include "utils.h"

#ifdef WIN32
#include <WS2tcpip.h>
#include <mstcpip.h>

#define socket_ioctl ioctlsocket
#define socket_close closesocket
#define socket_poll  WSAPoll

inline bool socket_error   () { return WSAGetLastError(); }
inline bool is_in_progress (int err) { return err == WSAEINPROGRESS; }
inline bool is_would_block (int err) { return err == WSAEWOULDBLOCK; }
inline bool is_interrupted (int err) { return err == WSAEINTR; }
    
int socket_set_blocking(socket_t sock, bool value)
{
    u_long code = value ? 0 : 1;
    return ioctlsocket(sock, FIONBIO, &code);
}

void socket_init()
{
    static bool initialized = false;

    if (!initialized)
    {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;

        int err = WSAStartup(wVersionRequested, &wsaData);
        if (err) throw SocketException();

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
            WSACleanup( );
            throw SocketException();
        }

        initialized = true;
    }
}
#else // for linux
#include <errno.h>

#define socket_ioctl ::ioctl
#define socket_close ::close
#define socket_poll  ::poll

inline void socket_init    () { }
inline int  socket_error   () { return errno; }
inline bool is_in_progress (int err) { return err == EINPROGRESS; }
inline bool is_would_block (int err) { return err == EWOULDBLOCK; }
inline bool is_interrupted (int err) { return err == EINTR; }

int socket_set_blocking(socket_t sock, bool value)
{
    int flags = fcntl(sock, F_GETFL, 0);
    flags = value ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
    return fcntl(sock, F_SETFL, flags);
}
#endif

//////////////////////////////////////////////////////////////////////////
BEGIN_NAMESPACE_LIB

IpAddress IpAddress::Any = IpAddress((uint)0);  // 0.0.0.0

IpAddress IpAddress::None = IpAddress(0xFFFFFFFF); // 255.255.255.255

IpAddress IpAddress::Loopback = IpAddress(0x7F000001); // 127.0.0.1

IpAddress::IpAddress (uint address)
{
    m_address = htonl(address);
}

IpAddress::IpAddress (byte address[], int count)
{
    if (count != 4) throw InvalidArgumentException();
    m_address = ((((address[3] << 0x18) | (address[2] << 0x10)) | (address[1] << 8)) | address[0]);
}

IpAddress::IpAddress (const char* address)
{
    m_address = inet_addr(address);
}

IpAddress::IpAddress (const string& address)
{
    m_address = inet_addr(address.c_str());
}

IpAddress::IpAddress (const SocketAddress& address)
{
    sockaddr_in* addr = (sockaddr_in*)address.m_data;
    m_address = addr->sin_addr.s_addr;
}

string IpAddress::toString () const
{
    byte* addr = (byte*)&m_address;
    string value = format("%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
    return value;
}

bool IpAddress::operator == (const IpAddress& other) const
{
    return (m_address == other.m_address);
}

bool IpAddress::operator != (const IpAddress& other) const
{
    return (m_address != other.m_address);
}

IpAddress IpAddress::parse (const char* ipstr)
{
    return IpAddress(ipstr);
}

IpAddress IpAddress::parse(const string& ipstr)
{
    return IpAddress(ipstr);
}

//////////////////////////////////////////////////////////////////////////
SocketAddress::SocketAddress(const IpAddress& address, int port)
{
    sockaddr_in* addr = (sockaddr_in*)&m_data;
    memset(addr, 0, sizeof(sockaddr_in));

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = address.m_address;
    addr->sin_port = htons(port);
}


SocketAddress::SocketAddress()
{
    memset(m_data, 0, sizeof(m_data));
}

IpAddress SocketAddress::ipAddress ()
{
    return IpAddress(*this);
}

int SocketAddress::port ()
{
    sockaddr_in* addr = (sockaddr_in*)&m_data;
    return htons(addr->sin_port);
}

//////////////////////////////////////////////////////////////////////////
Socket::Socket (AddressFamily family, SocketType type)
{
    socket_init();

    m_socket = ::socket(family, type, (type == UDP ? IPPROTO_UDP : IPPROTO_TCP));
    if (m_socket == InvalidSocket) throw SocketException();

    m_family    = family;
    m_type      = type;
    m_blocking  = true;
    m_connected = false;
}

Socket::Socket(socket_t sock, byte family, byte type, bool block, bool conn)
    : m_socket(sock), m_family(family), m_type(type), m_blocking(block), m_connected(conn)
{
}

Socket::~Socket ()
{
    close();
}

void Socket::connect (const IpAddress& host, int port, int timeout)
{
    SocketAddress addr(host, port);
       
    if (timeout > 0) socket_set_blocking(m_socket, false);

    int err = ::connect(m_socket, (sockaddr*)&addr.m_data, sizeof(sockaddr));
    if (err < 0) err = socket_error();

    if (timeout > 0) socket_set_blocking(m_socket, m_blocking);

    if (is_in_progress(err) || is_would_block(err) || is_interrupted(err))
    {
        bool writable = poll(timeout, SelectWrite);
        m_connected = writable && solError() == 0;
        
        if (m_connected) return;
    }

    if (err) throw SocketException();

    m_connected = true;
}

void Socket::bind (const IpAddress& address, int port)
{
    SocketAddress addr(address, port);

    int err = ::bind(m_socket, (sockaddr*)&addr.m_data, sizeof(sockaddr));
    if (err) throw SocketException();
}

void Socket::listen (int backlog)
{
    int err = ::listen(m_socket, backlog);
    if (err) throw SocketException();
}

Socket* Socket::accept ()
{
    for (;;)
    {
        socket_t sock = ::accept(m_socket, NULL, NULL);

        if (sock < 0)
        {
            int err = socket_error();
            if (is_interrupted(err)) continue;

            throw SocketException();
        }

        return new Socket(sock, m_family, TCP, false, true);
    }    
}

bool Socket::poll (int milliseconds, SelectMode mode)
{
    pollfd pfd = { m_socket, 0, 0 };

    if (hasFlag(mode, Socket::SelectRead )) pfd.events |= POLLIN;
    if (hasFlag(mode, Socket::SelectWrite)) pfd.events |= POLLOUT;
    if (hasFlag(mode, Socket::SelectError)) pfd.events |= POLLERR;

    for (;;)
    {
        int result = socket_poll(&pfd, 1, milliseconds);

        if (result < 0)
        {
            int err = socket_error();
            if (is_interrupted(err)) continue;

            throw SocketException();
        }

        return result > 0;
    }
}

int Socket::send (const void* data, int offset, int size, int flags)
{
    flags |= MSG_NOSIGNAL; // do not generate SIGPIPE.

    for (;;)
    {
        int num = ::send(m_socket, (const char*)data + offset, size, flags);

        if (num < 0)
        {
            int err = socket_error();

            if (is_interrupted(err)) continue;
            if (is_would_block(err)) return WouldBlock;
            
            throw SocketException();
        }

        return num;
    }
}

int Socket::sendTo (const void* data, int offset, int size, const IpAddress& host, int port, int flags)
{
    SocketAddress addr(host, port);
    socklen_t length = addr.size();

    for (;;)
    {
        int num = ::sendto(m_socket, (const char*)data + offset, size, flags, (sockaddr*)&addr.m_data, length);
    
        if (num < 0)
        {
            int err = socket_error();

            if (is_interrupted(err)) continue;
            if (is_would_block(err)) return WouldBlock;

            throw SocketException();
        }

        return num;
    }
}

int Socket::receive (void* data, int offset, int size, int flags)
{
    for (;;)
    {
        int num = recv(m_socket, (char*)data + offset, size, flags);
    
        if (num < 0)
        {
            int err = socket_error();

            if (is_interrupted(err)) continue;
            if (is_would_block(err)) return WouldBlock;

            throw SocketException();
        }

        return num;
    }
}

int Socket::receiveFrom (void* data, int offset, int size, IpAddress* host, int* port, int flags)
{
    SocketAddress addr;
    socklen_t length = addr.size();

    for (;;)
    {
        int num = recvfrom(m_socket, (char*)data + offset, size, flags, (sockaddr*)&addr.m_data, &length);
    
        if (num < 0)
        {
            int err = socket_error();

            if (is_interrupted(err)) continue;
            if (is_would_block(err)) return WouldBlock;

            throw SocketException();
        }
        else
        {
            if (host) *host = addr.ipAddress();
            if (port) *port = addr.port();
        }

        return num;
    }
}

void Socket::shutdown (ShutdownMode shutdownMode)
{
    if (m_socket != InvalidSocket) ::shutdown(m_socket, shutdownMode);

    m_connected = false;
}

void Socket::close ()
{
    if (m_socket != InvalidSocket)
    {
        if (m_connected) ::shutdown(m_socket, ShutdownBoth);

        while (socket_close(m_socket) < 0)
        {
            int err = socket_error();
            if (!is_interrupted(err)) break;
        }

        m_socket = InvalidSocket;
        m_connected = false;
    }
}

int Socket::solError()
{
    return getOption(SOL_SOCKET, SO_ERROR);
}

int Socket::error()
{
    return socket_error();
}

void Socket::setBlocking (bool value)
{
    int err = socket_set_blocking(m_socket, value);
    if (err) throw SocketException();

    m_blocking = value;
}

int Socket::available ()
{
    u_long size = 0;
    int err = socket_ioctl(m_socket, FIONREAD, &size);
    if (err) throw SocketException();

    return (int)size;
}

IpAddress Socket::localAddress ()
{
    SocketAddress addr(IpAddress::Any, 0);
    int length = sizeof(addr.m_data);
    getsockname(m_socket, (sockaddr*)&addr, (socklen_t*)&length);
    return IpAddress(addr);
}

IpAddress Socket::remoteAddress ()
{
    SocketAddress addr(IpAddress::Any, 0);
    int length = sizeof(addr.m_data);
    getpeername(m_socket, (sockaddr*)addr.m_data, (socklen_t*)&length);
    return IpAddress(addr);
}

int Socket::localPort ()
{
    SocketAddress addr(IpAddress::Any, 0);
    int length = sizeof(addr.m_data);
    getsockname(m_socket, (sockaddr*)addr.m_data, (socklen_t*)&length);
    return addr.port();
}

int Socket::remotePort ()
{
    SocketAddress addr(IpAddress::Any, 0);
    int length = sizeof(addr.m_data);
    getpeername(m_socket, (sockaddr*)addr.m_data, (socklen_t*)&length);
    return addr.port();
}

string Socket::remoteName()
{
    string ip = remoteAddress().toString();
    return format("%s:%d", ip.c_str(), remotePort());
}

string Socket::localName()
{
    string ip = localAddress().toString();
    return format("%s:%d", ip.c_str(), localPort());
}

void Socket::setOption (int level, int name, const void* value, int length)
{
    int err = setsockopt(m_socket, level, name, (const char*)value, length);
    if (err) throw SocketException();
}

void Socket::getOption (int level, int name, void* value, int* length)
{
    int err = getsockopt(m_socket, level, name, (char*)value, (socklen_t*)length);
    if (err) throw SocketException();
}

void Socket::setOption (int level, int name, int value)
{
    int err = setsockopt(m_socket, level, name, (const char*)&value, sizeof(int));
    if (err) throw SocketException();
}

int Socket::getOption (int level, int name)
{
    int value = 0, length = sizeof(int);
    int err = getsockopt(m_socket, level, name, (char*)&value, (socklen_t*)&length);
    if (err) throw SocketException();
    return value;
}

bool Socket::lingerOption(int* time)
{
    linger opt; int len = sizeof(opt);
    getOption(SOL_SOCKET, SO_LINGER, &opt, &len);
    if (time) *time = opt.l_linger;
    return opt.l_onoff;
}

void Socket::setLingerOption(bool onoff, int time)
{
    linger opt = { onoff, time };
    setOption(SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
}

int Socket::sendBufferSize ()
{
    return getOption(SOL_SOCKET, SO_SNDBUF);
}

void Socket::setSendBufferSize (int value)
{
    setOption(SOL_SOCKET, SO_SNDBUF, value);
}

int Socket::receiveBufferSize ()
{
    return getOption(SOL_SOCKET, SO_RCVBUF);
}

void Socket::setReceiveBufferSize (int value)
{
    setOption(SOL_SOCKET, SO_RCVBUF, value);
}

int Socket::sendTimeout ()
{
    timeval tm; int len = sizeof(tm);
    getOption(SOL_SOCKET, SO_SNDTIMEO, &tm, &len);
    return tm.tv_sec * 1000 + tm.tv_usec / 1000;
}

void Socket::setSendTimeout (int milliseconds)
{
    timeval tm = { milliseconds / 1000, (milliseconds % 1000) * 1000 };
    setOption(SOL_SOCKET, SO_SNDTIMEO, (const char*)&tm, sizeof(tm));
}

int Socket::receiveTimeout ()
{
    timeval tm; int len = sizeof(tm);
    getOption(SOL_SOCKET, SO_RCVTIMEO, &tm, &len);
    return tm.tv_sec * 1000 + tm.tv_usec / 1000;
}

void Socket::setReceiveTimeout (int milliseconds)
{
    timeval tm = { milliseconds / 1000, (milliseconds % 1000) * 1000 };
    setOption(SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm));
}

bool Socket::reuseAddress ()
{
    return getOption(SOL_SOCKET, SO_REUSEADDR) != 0;
}

void Socket::setReuseAddress (bool value)
{
    setOption(SOL_SOCKET, SO_REUSEADDR, value ? 1 : 0);
}

bool Socket::tcpNoDelay ()
{
    return getOption(IPPROTO_TCP, TCP_NODELAY) != 0;
}

void Socket::setTcpNoDelay (bool value)
{
    setOption(IPPROTO_TCP, TCP_NODELAY, value ? 1 : 0);
}

bool Socket::keepAlive ()
{
    return getOption(SOL_SOCKET, SO_KEEPALIVE) != 0;
}

void Socket::setKeepAlive (bool onoff, int idle, int interval, int count)
{
#ifdef WIN32
    WORD bytesReturned; 
    tcp_keepalive settings = { onoff ? 1 : 0, idle * 1000, interval * 1000 }; // in milleseconds

    int err = WSAIoctl(m_socket, SIO_KEEPALIVE_VALS, &settings, sizeof(settings), NULL, 0, &bytesReturned, NULL, NULL);
    if (err) throw SocketException(); 
#else
    int opt = onoff ? 1 : 0;
    int err = setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(int));
    if (err) throw SocketException();

    setsockopt(m_socket, SOL_TCP, TCP_KEEPIDLE,  &idle,     sizeof(int));
    setsockopt(m_socket, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(int));
    setsockopt(m_socket, SOL_TCP, TCP_KEEPCNT,   &count,    sizeof(int));
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////
NetworkStream::NetworkStream (Socket* socket, bool own)
  : m_socket(socket), m_ownSocket(own), m_sendTimeout(-1), m_recvTimeout(-1)
{
}

NetworkStream::NetworkStream (const IpAddress& host, int port, int connTimeout)
  : m_socket(0), m_ownSocket(true), m_sendTimeout(-1), m_recvTimeout(-1)
{
    m_socket = new Socket();

    try
    {
        m_socket->connect(host, port, connTimeout);
    }
    catch (...)
    {
        delete m_socket;
        throw;
    }
}

NetworkStream::~NetworkStream ()
{
    close();
}

bool NetworkStream::canRead ()
{
    return true;
}

bool NetworkStream::canWrite ()
{
    return true;
}

bool NetworkStream::canTimeout ()
{
    return true;
}

bool NetworkStream::canReady()
{
    return true;
}

int NetworkStream::readTimeout ()
{
    return m_recvTimeout;
}

void NetworkStream::setReadTimeout (int timeout)
{
    m_recvTimeout = timeout;
}

int NetworkStream::writeTimeout ()
{
    return m_sendTimeout;
}

void NetworkStream::setWriteTimeout (int timeout)
{
    m_sendTimeout = timeout;
}

bool NetworkStream::readyRead(int timeout)
{
    return m_socket->poll(timeout, Socket::SelectRead);
}

bool NetworkStream::readyWrite(int timeout)
{
    return m_socket->poll(m_sendTimeout, Socket::SelectWrite);
}

int NetworkStream::read (void* data, int offset, int size)
{
    if (size <= 0) return 0;

    if (m_recvTimeout >= 0 && !m_socket->poll(m_recvTimeout, Socket::SelectRead))
    {
        throw TimeoutException("Timeout while read network stream");
    }

    int num = m_socket->receive(data, offset, size);
    if (num < 0) throw SocketException();

    return num;
}

int NetworkStream::write (const void* data, int offset, int size)
{
    if (size <= 0) return 0;

    if (m_sendTimeout >= 0 && !m_socket->poll(m_sendTimeout, Socket::SelectWrite))
    {
        throw TimeoutException("Timeout while write network stream");
    }

    int num = m_socket->send(data, offset, size);
    if (num < 0) throw SocketException();

    return num;
}

void NetworkStream::flush ()
{
}

void NetworkStream::close ()
{
    Socket* s = m_socket;
    m_socket = NULL;

    if (s && m_ownSocket)
    {
        s->shutdown(Socket::ShutdownBoth);
        s->close();
        delete s;
    }
}

END_NAMESPACE_LIB
