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






class TreeNode
{

public:
   TreeNode *leftPtr;      // pointer to left subtree
   int port;// port number
   TreeNode *rightPtr;     // pointer to left subtree

   // Constructor
   TreeNode()   
      : leftPtr( nullptr ), // pointer to left subtree
        port( -1 ), // tree node data
        rightPtr( nullptr ) // pointer to right substree
   {} 
};






class Tree
{
private:
    TreeNode* rootPtr;
public:
    Tree() : rootPtr(new TreeNode()) { /* empty body */ }

    void insertNode(unsigned int ip, unsigned int remaining_prefix, int port) //  IP, Prefix Length, Port
    {
        Tree::insertNodeHelper(rootPtr, ip, remaining_prefix, port);
    }
    int getPort(unsigned int ip)
    {
        return Tree::getPortHelper(rootPtr, ip);
    }


    void insertNodeHelper(TreeNode* ptr, unsigned int ip, unsigned int remaining_prefix, int port)
    {
        // std::cout << "ip: " << ip << "\n";
        // std::bitset<32> x(ip);
        // std::cout << "ip (bits): "  << x <<  "\n";
        // std::cout << "remaining_prefix: " << remaining_prefix << "\n";
        // std::cout << "port: " << port << "\n";

        // If remaining_prefix == 0
            // If port > current node value
                // Set current node value to port
            // Else 
                // return
        if (remaining_prefix == 0) {
            // std::cout << "Remaining prefix is 0 \n";
            int currentPort = (ptr)->port;
            if (port > currentPort) {
                (*ptr).port = port;
            }
            return;
        }




        // Else if current IP bit == 1:
            // If current node rightptr == nulptr
                // Create new node
                // Set current node rightptr to new node pointer
            // insertNodeHelper ( current node rightptr, inverted IP rightshifted, remaining_prefix -= 1, port)
        else if (ip & (0b1 << 31))
        {
            // std::cout << "First IP bit is 1 \n";
            if ((ptr)->rightPtr == nullptr) {

                (*ptr).rightPtr = new TreeNode();
                ptr->rightPtr->port = ptr->port;
            }
            insertNodeHelper((ptr->rightPtr), ip << 1, --remaining_prefix, port);
            return;
        }



        // Else if current IP bit == 0:
            // If current node leftptr == nulptr
                // Create new node
                // Set current node leftptr to new node pointer
            // insertNodeHelper ( current node leftptr, inverted IP rightshifted, remaining_prefix -= 1, port)
        else if (!(ip & (0x1 << 31)))
        {
            // std::cout << "First IP bit is 0 \n" ;

            if ((*ptr).leftPtr == nullptr) {
                // std::cout << "Leftptr is nullptr \n";
                (*ptr).leftPtr = new TreeNode();
                ptr->leftPtr->port = ptr->port;
            }

            // std::cout << "Traversing tree \n";
            insertNodeHelper((ptr->leftPtr), ip << 1, --remaining_prefix, port);
            return;
        }
        std::cout << std::endl;
    }

    int getPortHelper(TreeNode* ptr, unsigned int ip)
    {
        // If current IP bit == 1 
            // If current node rightptr == nullptr
                // Return current node port
            // Else 
                // Return getPortHelper (current node rightptr, inverted_ip rightshifted)
        if (ip & (0b1 << 31)) {
            if ((ptr)->rightPtr == nullptr) {
                return (*ptr).port;
            }
            else {
                return getPortHelper((ptr->rightPtr), ip << 1);
            }
        }

        // If current IP bit == 0 
            // If current node leftptr == nullptr
                // Return current node port
            // Else 
                // Return getPortHelper (current node leftptr, inverted_ip rightshifted)
        else {
            if ((ptr)->leftPtr == nullptr) {
                return (*ptr).port;
            }
            else {
                return getPortHelper((ptr->leftPtr), ip << 1);
            }
        }
    }
};




std::vector<ip_entry> ip_entrys;
int reftemp;

Tree portTree;


void init() {
    //read in the txt into data struct
   // You can use this function to initialize variables.
    ip_entrys.reserve(420972);
}

void add_route(unsigned int ip, int prefix_length, int port_number) {
    // TODO: Store route to be used for later use in lookup_ip() function
    //ip_entrys.emplace_back(prefix_length, ip, port_number);
    portTree.insertNode(ip, prefix_length, port_number);
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
    /*best_match bestie(0, 0, -1); //initiate best match
    for (const auto& iterator : ip_entrys) {
        reftemp = iterator.ip ^ ip;
        reftemp = reftemp >> (32 - iterator.pl);
        if (reftemp == 0 && bestie.pl < iterator.pl) {
                bestie.pl = iterator.pl;
                bestie.ref = reftemp;
                bestie.pn = iterator.pn;
        }
    }
    return bestie.pn;*/
    return portTree.getPort(ip);
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


