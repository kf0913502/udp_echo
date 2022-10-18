//
// Created by Kariem Fahmi  on 18/10/2022.
//

#ifndef UDP_ECHO_UDPECHO_H
#define UDP_ECHO_UDPECHO_H


extern double getCurTimeInMs();


extern int setSckTimeout(int fd, int timeoutSec);

extern unsigned int createSck();

//use 0 in ifaceIp to listen to all local interfaces
extern int bindSck(int fd, int ifaceIp, short port);


extern int sckWrite(int fd, int dstIp, short dstPort, char * data, int len);


extern int sckRead(int fd, char * data, int len, int * srcIp, short * srcPort);


#endif //UDP_ECHO_UDPECHO_H
