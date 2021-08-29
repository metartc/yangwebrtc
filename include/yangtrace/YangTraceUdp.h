#ifndef TRACE_YangTraceUdp_H_
#define TRACE_YangTraceUdp_H_
#ifdef _WIN32
//#include <winsock.h>
#include <winsock2.h>

//#define SOCKADDR_IN
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
class YangTraceUdp
{
public:
    YangTraceUdp(void);
    ~YangTraceUdp(void);
    void init(const char *ip,int port);
    int sendUdp(const char* data,int len);
    void setSockSrv(const char *ip,int port);
private:
    int sockSrv;
    struct sockaddr_in m_addrClient;
};

#endif
