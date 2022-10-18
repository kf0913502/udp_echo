#include "UdpEcho.h"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int client(int dstIp, short dstPort, unsigned int nPackets)
{
    int fd = createSck();
    if (fd < 0)
    {
        std::cerr << "Failed to create socket, reason: " << strerror(errno);
        return -1;
    }

    char data[1];
    int srcIp;
    short srcPort;

    std::vector<double> delays;

    for(int i=0; i< nPackets; i++)
    {
        auto sendTime = getCurTimeInMs();
        int result = sckWrite(fd, dstIp, dstPort, data, 1);

        if (result < 0)
        {
            std::cerr << "Failed to write to socket, reason: " << strerror(errno);
            return -1;
        }

        result = sckRead(fd, data, 1,&srcIp, &srcPort);
        if (result < 0)
        {
            std::cerr << "Failed to read from socket, reason: " << strerror(errno);
            return -1;
        }

        double delay = getCurTimeInMs() - sendTime;
        delays.push_back(delay);
        std::cout << "Packet " << i << " RTT " << delays[i] << " ms, max RTT " << *std::max_element(delays.begin(),delays.end()) << std::endl;

    }

    return 0;



}


int server(unsigned short port)
{
    int fd = createSck();
    if (fd < 0)
    {
        std::cerr << "Failed to create socket, reason: " << strerror(errno);
        return -1;
    }

    int result = bindSck(fd, 0, port);

    if (result < 0)
    {
        std::cerr << "Failed to bind socket, reason: " << strerror(errno);
        return -1;
    }
    char data[1];
    int srcIp;
    short srcPort;
    while(true)
    {
        result = sckRead(fd, data, 1, &srcIp, &srcPort);
        if (result < 0)
        {
            std::cerr << "Failed to read from socket, reason: " << strerror(errno);
            return -1;
        }

        sckWrite(fd,srcIp,srcPort,data,1);

    }

}
int main(int argc, char *argv[]) {

    if (argc == 1)
    {
        std::cerr << "Invalid arguments! Valid arguments are -c <dest_ip> <dest_port> <n_packets> for client mode OR -s <listen_port> for server mode";
    }
   else  if (strcmp(argv[1],"-c") == 0)
    {
        std::cout << "client mode" << std::endl;
        if (argc != 5)
        {
            std::cerr << "Invalid argument for client mode. -c <dest_ip> <dest_port> <n_packets>";
            return -1;
        }

        struct sockaddr_in sa;
        inet_pton(AF_INET, argv[2], &(sa.sin_addr));

        int dstIp = ntohl(sa.sin_addr.s_addr);
        short dstPort = atoi(argv[3]);
        unsigned int nPackets = atoi(argv[4]);
        return client(dstIp, dstPort, nPackets);
    }
    else if (strcmp(argv[1],"-s") == 0)
    {
        std::cout << "server mode" << std::endl;

        if (argc != 3)
        {
            std::cerr << "Invalid argument for server mode. -s <listen_port> ";
            return -1;
        }
        short listenPort = atoi(argv[2]);

        return server(listenPort);
    }
    else
    {
        std::cerr << "Invalid first argument. Use -c for client and -s for server";
        return -1;
    }



    return 0;
}
