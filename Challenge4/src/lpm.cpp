/**
 * lpm.cpp
 *
 *   Version: 2020-03-02
 * Copyright: University of Twente, 2015-2019
 *
 **************************************************************************
 *                            Copyright notice                            *
 *                                                                        *
 *             This file may ONLY be distributed UNMODIFIED.              *
 * In particular, a correct solution to the challenge must NOT be posted  *
 * in public places, to preserve the learning effect for future students. *
 **************************************************************************
 */


#include <stdio.h>

void ip2human(unsigned int ip);


// you can declare global variables here.

void init() {
   // You can use this function to initialize variables.
}

void add_route(unsigned int ip, int prefix_length, int port_number) {
    // TODO: Store route to be used for later use in lookup_ip() function
}

// This method is called after all routes have been added.
// You don't have to use this method but can use it to sort or otherwise
// organize the routing information, if your datastructure requires this.
void finalize_routes() {
    // TODO: (optionally) use this to finalize your routes.
}

int lookup_ip(unsigned int ip) {
    // TODO: Lookup IP in stored data from add_route function,
    //       returns port number (or -1 for no route found).
  return -1;
}


// convenience function to print IP addresses in human readable form for debugging
void ip2human(unsigned int ip) {
    unsigned int a, b, c, d;

    a = (ip >> 24) & 0xff;
    b = (ip >> 16) & 0xff;
    c = (ip >>  8) & 0xff;
    d =  ip        & 0xff;

    printf("%i.%i.%i.%i\n", a, b, c, d);
}
