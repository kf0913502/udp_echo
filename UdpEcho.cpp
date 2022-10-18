//
// Created by Kariem Fahmi  on 18/10/2022.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <vector>
#include <sys/fcntl.h>
#include <cstring>
#include <algorithm>




double getCurTimeInMs() {
    struct timeval tp;
    int result = gettimeofday(&tp, NULL);
    if (result == -1)
    {
    }
    return tp.tv_sec * 1000.0 + tp.tv_usec / 1000.0;

}


int setSckTimeout(int fd, int timeoutSec)
{
    struct timeval tv;
    tv.tv_sec = timeoutSec;
    tv.tv_usec = 0;
    int result = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    return result;

}

unsigned int createSck()
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK); // make socket non-blocking
    return fd;
}

//use 0 in ifaceIp to listen to all local interfaces
int bindSck(int fd, int ifaceIp, short port)
{

    struct sockaddr_in servaddr;  /*  socket address structure  */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port        = htons(port);

    int bindResult = bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    return bindResult;
}


int sckWrite(int fd, int dstIp, short dstPort, char * data, int len)
{
    struct msghdr msg{};
    iovec vec{};
    struct sockaddr_in dstAddress{};

    bzero(&msg,sizeof(msghdr));
    bzero(&dstAddress, sizeof(dstAddress));

    dstAddress.sin_family      = AF_INET;
    dstAddress.sin_port = htons(dstPort);
    dstAddress.sin_addr.s_addr = htonl(dstIp);
    msg.msg_name = &dstAddress;
    msg.msg_namelen = sizeof(dstAddress);

    vec.iov_base = data;
    vec.iov_len = len;
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;


    int result;

    result = sendmsg(fd, &msg,0);

    return result;
}


int sckRead(int fd, char * data, int len, int * srcIp, short * srcPort)
{
    sockaddr_in srcAddress{};
    unsigned int sockAddrLen = sizeof(srcAddress);
    bzero(&srcAddress,sizeof(srcAddress));
    int result = 0;

    do {
        result = recvfrom(fd, (void *) data, len, 0, (sockaddr *) (&srcAddress),
                          (socklen_t *) &sockAddrLen);
    }while(result < 0 && (errno == 35 || errno == 11)); //35 and 11 are error codes for is Resource Not Available on MacOS and Linux respectively


    *srcIp = ntohl(srcAddress.sin_addr.s_addr);
    *srcPort = ntohs(srcAddress.sin_port);

    return result;

}
