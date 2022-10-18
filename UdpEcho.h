//
// Created by Kariem Fahmi  on 18/10/2022.
//

#ifndef UDP_ECHO_UDPECHO_H
#define UDP_ECHO_UDPECHO_H


extern "C" int createSck();



extern "C"  double getCurTimeInMs();




extern "C" int setSckTimeout(int fd, int timeoutSec);

//use 0 in ifaceIp to listen to all local interfaces
extern "C" int bindSck(int fd, int ifaceIp, short port);



extern "C" int sckWrite(int fd, int dstIp, short dstPort, char * data, int len);




extern "C"  int sckRead(int fd, char * data, int len, int * srcIp, short * srcPort);


#endif //UDP_ECHO_UDPECHO_H
