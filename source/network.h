#ifndef LIB_NETWORK_H
#define LIB_NETWORK_H

#include "socket.h"
#include "thread.h"
#include "buffer.h"
#include "reader.h"

BEGIN_NAMESPACE_LIB

class Network
{
public:
    static string getLocalAddress(const char* devName);
};

END_NAMESPACE_LIB

#endif//LIB_NETWORK_H

