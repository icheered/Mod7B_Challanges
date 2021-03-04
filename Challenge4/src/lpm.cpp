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
#include <iostream>
#include <vector>


void ip2human(unsigned int ip);


// you can declare global variables here.
struct ip_entry {
    int8_t pl;
    unsigned int ip;
    int16_t pn;

    ip_entry(int8_t pl, unsigned int ip, int16_t pn) : pl(pl), ip(ip), pn(pn){}
};

struct best_match {
    int8_t pl;
    int32_t ref;
    int16_t pn;

    best_match(int8_t pl, int32_t ref, int16_t pn) : pl(pl), ref(ref), pn(pn) {}
};

std::vector<ip_entry> ip_entrys;
int reftemp;



void init() {
    //read in the txt into data struct
   // You can use this function to initialize variables.
    ip_entrys.reserve(420972);
}

void add_route(unsigned int ip, int prefix_length, int port_number) {
    // TODO: Store route to be used for later use in lookup_ip() function
    ip_entrys.emplace_back(prefix_length, ip, port_number);
}

// This method is called after all routes have been added.
// You don't have to use this method but can use it to sort or otherwise
// organize the routing information, if your datastructure requires this.
void finalize_routes() {
    // TODO: (optionally) use this to finalize your routes.
}

int lookup_ip(unsigned int& ip) {
    // TODO: Lookup IP in stored data from add_route function,
    //       returns port number (or -1 for no route found).
    best_match bestie(0, 0, -1); //initiate best match
    for (const auto& iterator : ip_entrys) {
        reftemp = iterator.ip ^ ip;
        reftemp = reftemp >> (32 - iterator.pl);
        if (reftemp == 0 && bestie.pl < iterator.pl) {
                bestie.pl = iterator.pl;
                bestie.ref = reftemp;
                bestie.pn = iterator.pn;
        }
    }
    return bestie.pn;
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
