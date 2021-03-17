#include "client.hpp"

#ifdef _MSC_VER
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

typedef int SOCKET;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <cstring>

SOCKET sockfd;
int32_t received_length;

void connect(void) {
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(1234);

    int result = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    if (result < 0) {
        printf("Error connecting to local proxy; did you start it?\n");
        exit(0);
    }
}

int32_t get_received_length(void) {
    return received_length;
}

#ifndef _MSC_VER
void receive_timeout(int signo) {
    return;
}
#endif

unsigned char *receive(unsigned int timeout) {
    int32_t got_so_far = 0;
    int result;

#ifndef _MSC_VER
    struct sigaction sa;
    sa.sa_handler = receive_timeout;
    sa.sa_flags = 0;

    sigaction(SIGALRM, &sa, NULL);
    alarm(1 + (timeout - 1) / 1000);    // needed for CygWin since it doesn't support socket timeout yet

    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
#else
    DWORD t = timeout;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(DWORD));
#endif

    result = recv(sockfd, reinterpret_cast<char*>(&received_length), sizeof(decltype(received_length)), 0);

#ifndef _MSC_VER
    alarm(0); /* cancel alarm */
#endif

    if (result < 0) {
#ifdef _MSC_VER
        int err = WSAGetLastError();
        if (err == WSAETIMEDOUT) {
#else
        if (errno == EAGAIN || errno == EINTR) {
#endif
            received_length = 0;
            printf("Receive timeout expired\n");
            return NULL;
        }

        printf("Receive error%i\n", result);
        exit(0);
    }

    received_length = ntohl(received_length);
    unsigned char *packet =
        (unsigned char *)malloc(sizeof(char) * received_length);

    while (got_so_far < received_length) {
        result = recv(sockfd, (char*)(packet + got_so_far), received_length - got_so_far, 0);

        if (result < 0) {
            printf("Receive error\n");
            exit(0);
        }

        got_so_far += result;
    }

    return packet;
}

void send(unsigned char *data, int32_t length) {
    int result;

    int32_t plength = htonl(length);

    result = send(sockfd, reinterpret_cast<char*>(&plength), sizeof(decltype(plength)), 0);

    if (result < 0) {
        printf("Send error\n");
        exit(0);
    }

    result = send(sockfd, (char*)data, length, 0);

    if (result < 0) {
        printf("Send error\n");
        exit(0);
    }
}
