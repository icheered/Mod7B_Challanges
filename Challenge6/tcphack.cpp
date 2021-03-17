#include "client.hpp"

#ifdef _MSC_VER
#include <Winsock2.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>


int main(void) {
#ifdef _MSC_VER
    // Initialize Winsock
    WSADATA wsaDataUnused;
    WSAStartup(/*version*/2, &wsaDataUnused);
#endif

    connect();

    bool done = false;

    // array of bytes in which we're going to build our packet:
    unsigned char txpkt[1500];
    //parts of the ipv6 header
    unsigned char Version3TrafficClass12FlowLabel[4];
    unsigned char payloadLength[2];
    unsigned char nextHeader[1] = { 0 };
    unsigned char HopLimit[1] = { 0 };
    unsigned char sourceAddr[16];
    unsigned char destAddr[16];
    //parts of the tcp header
    unsigned int TCPoffset = 40; //length of the ipv6 header in bytes
    unsigned char sourcePort[2];
    unsigned char destinationPort[2];
    unsigned char seqNumber[4];
    unsigned char ackNumber[4];
    unsigned char headerLengthAndFlags[2];
    unsigned char advertisedWindow[2];
    unsigned char TCPchecksum[2];
    unsigned char urgentPointer[2];
    
    //initializing ipv6 arrays, default to zero
    memset(Version3TrafficClass12FlowLabel, 0, sizeof(Version3TrafficClass12FlowLabel));
    Version3TrafficClass12FlowLabel[0] = 0x60; // first byte of the IPv6 header contains version number in upper nibble
    memset(payloadLength, 0, sizeof(payloadLength));
    payloadLength[0] = 0x0008; //at least 8 byte tcp header
    nextHeader[0] = 0x06; //6 for TCP
    HopLimit[0] = 0x0a; //arbitrary value of 10 for the hop limit
    //assigning my own ip as source addr
    sourceAddr[0] = 0x20;
    sourceAddr[1] = 0x03;
    sourceAddr[2]  = 0x00;
    sourceAddr[3] = 0xdf;
    sourceAddr[4] = 0xff;
    sourceAddr[5] = 0x13;
    sourceAddr[6]  = 0x73;
    sourceAddr[7] =  0x00;
    sourceAddr[8]  = 0x75;
    sourceAddr[9] = 0xfc;
    sourceAddr[10]  = 0x19;
    sourceAddr[11] = 0xf0;
    sourceAddr[12]  = 0x9a;
    sourceAddr[13] = 0x80;
    sourceAddr[14] = 0x77;
    sourceAddr[15] = 0x7f;
    //assigning destination addr 
    destAddr[0] = 0x20;
    destAddr[1] = 0x01;
    destAddr[2] = 0x06;
    destAddr[3] = 0x10;
    destAddr[4] = 0x19;
    destAddr[5] = 0x08;
    destAddr[6] = 0xff;
    destAddr[7] = 0x02;
    destAddr[8] = 0xe7;
    destAddr[9] = 0xb3;
    destAddr[10] = 0xe3;
    destAddr[11] = 0x75;
    destAddr[12] = 0xfd;
    destAddr[13] = 0xff;
    destAddr[14] = 0x67;
    destAddr[15] = 0x9f;


    //setting all TCP arrays to zero initially
    memset(txpkt, 0, sizeof(txpkt));
    memset(headerLengthAndFlags, 0, sizeof(headerLengthAndFlags));
    memset(sourcePort, 0, sizeof(sourcePort));
    memset(destinationPort, 0, sizeof(destinationPort));
    memset(seqNumber, 0, sizeof(seqNumber));
    memset(ackNumber, 0, sizeof(ackNumber));
    memset(advertisedWindow, 0, sizeof(advertisedWindow));
    memset(TCPchecksum, 0, sizeof(TCPchecksum));
    memset(urgentPointer, 0, sizeof(urgentPointer));

    //assign ipv6 values
    payloadLength[0] = 0x14;
    //assigning TCP values
    sourcePort[0] = 0x4d;
    sourcePort[1] = 0x20;
    destinationPort[0] = 0x1e;
    destinationPort[1] = 0x1e;
    headerLengthAndFlags[0] = 0x50;
    headerLengthAndFlags[0] = 0x02; //set syn flag
    advertisedWindow[0] = 0x01; //arbitrary chosen
    //assigning additional data

    //fill the packet
    //section ipv6 header
    memcpy(txpkt, Version3TrafficClass12FlowLabel, sizeof(Version3TrafficClass12FlowLabel));
    memcpy(txpkt + 4, payloadLength, sizeof(payloadLength));
    memcpy(txpkt + 6, nextHeader, sizeof(nextHeader));
    memcpy(txpkt + 7, HopLimit, sizeof(HopLimit));
    memcpy(txpkt + 8, sourceAddr, sizeof(sourceAddr));
    memcpy(txpkt + 24, destAddr, sizeof(destAddr));
    //section TCP header
    memcpy(txpkt + TCPoffset, sourcePort, sizeof(sourcePort));
    memcpy(txpkt + TCPoffset + 2, destinationPort, sizeof(destinationPort));
    memcpy(txpkt + TCPoffset + 4, seqNumber, sizeof(seqNumber));
    memcpy(txpkt + TCPoffset + 8, ackNumber, sizeof(ackNumber));
    memcpy(txpkt + TCPoffset + 12, headerLengthAndFlags, sizeof(headerLengthAndFlags));
    memcpy(txpkt + TCPoffset + 14, advertisedWindow, sizeof(advertisedWindow));
    memcpy(txpkt + TCPoffset + 16, TCPchecksum, sizeof(TCPchecksum));
    memcpy(txpkt + TCPoffset + 18, urgentPointer, sizeof(urgentPointer));
    //maybe section for more packet stuff?
    int HeaderOffset = 60;




    /* fill in the rest of the packet yourself...:
    txpkt[1] = .........;
    txpkt[2] = .........;
    ......
    */

    send(txpkt, 60);    // send the packet, with a length of 80 bytes

    while (!done) {
        unsigned char *rxpkt = receive(500); // check for reception of a packet, but wait at most 500 ms

        if (!rxpkt) {
            // if NULL, nothing has been received yet
            printf("Nothing...\n");
            continue;
        }

        // something has been received; rxpkt now points to the received data bytes, including the IPv6 header
        int len = get_received_length();    // ask for the length of the most recent packet

        // print the received bytes:
        int i;
        printf("Received %i bytes: ", len);
        for (i = 0; i < len; i++) printf("%02x ", rxpkt[i]);
        printf("\n");

    }

#ifdef _MSC_VER
    // De-initialize Winsock
    WSACleanup();
#endif
}
