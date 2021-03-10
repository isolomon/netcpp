#include "network.h"
#include "process.h"
#include "utils.h"

BEGIN_NAMESPACE_LIB


string Network::getLocalAddress(const char* devName)
{
    string cmdline = format("ip -4 addr show %s", devName);
    string output;

    int code = Process::popen(cmdline.c_str(), output);

    if (code == 0)
    {
        int pos = output.find("inet ");
        int end = output.find('/', pos);

        if (pos > 0 && end > pos)
        {
            pos += 5;
            return output.substr(pos, end - pos);
        }
    }

    return string();
}

END_NAMESPACE_LIB
