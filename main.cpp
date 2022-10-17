#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <vector>
#include <sys/fcntl.h>


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
    }while(result < 0 && errno == 35);


    *srcIp = ntohl(srcAddress.sin_addr.s_addr);
    *srcPort = ntohs(srcAddress.sin_port);

    return result;

}


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
    if (strcmp(argv[1],"-c") == 0)
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
