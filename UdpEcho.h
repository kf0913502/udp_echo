//
// Created by Kariem Fahmi  on 18/10/2022.
//

#ifndef UDP_ECHO_UDPECHO_H
#define UDP_ECHO_UDPECHO_H


int createSck();

extern "C" int cCreateSck()
{
    return createSck();
}


 double getCurTimeInMs();

extern "C" double cGetCurTimeInMs()
{
    return getCurTimeInMs();
}


 int setSckTimeout(int fd, int timeoutSec);

//use 0 in ifaceIp to listen to all local interfaces
extern int bindSck(int fd, int ifaceIp, short port);

extern "C" double cBindSck(int fd, int ifaceIp, short port)
{
    return bindSck(fd,ifaceIp,port);
}

int sckWrite(int fd, int dstIp, short dstPort, char * data, int len);

extern "C" int cSckWrite(int fd, int dstIp, short dstPort, char * data, int len)
{
    return sckWrite( fd,  dstIp,  dstPort, data,  len);
}



 int sckRead(int fd, char * data, int len, int * srcIp, short * srcPort);

extern "C" int cSckRead(int fd, char * data, int len, int * srcIp, short * srcPort)
{
    return sckRead( fd,  data,  len, srcIp,  srcPort);
}
#endif //UDP_ECHO_UDPECHO_H
