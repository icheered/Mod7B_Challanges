#include "client.hpp"

#ifdef _MSC_VER
#include <Winsock2.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>


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

    txpkt[0] = 0x60;	// first byte of the IPv6 header contains version number in upper nibble
    /* fill in the rest of the packet yourself...:
    txpkt[1] = .........;
    txpkt[2] = .........;
    ......
    */

    send(txpkt, 40);    // send the packet, with a length of 40 bytes

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
