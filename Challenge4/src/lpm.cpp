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


//==============================================================
// Filename :   lpm.cpp
// Authors :    Tjeerd Bakker and Soenke van Loh
// Version :    v1.0
// License :    Copyright by University of Twente
// Description : Longest Prefix Matching
//==============================================================


#include <stdio.h>
#include <iostream>
#include "lpm.h"


class TreeNode
{

public:
   TreeNode *leftPtr;       // pointer to left subtree
   int port;                // Interface number
   TreeNode *rightPtr;      // pointer to left subtree

   // Constructor
   TreeNode(  )   
      : leftPtr(nullptr),
        port(-1),           // Default node value is -1
        rightPtr(nullptr){} 
};




class Tree
{
private:
    TreeNode *rootPtr;
public:
    Tree() : rootPtr(new TreeNode()) {}

    // Insert a new entry into the binary tree
    void insertNode(unsigned int ip, unsigned int remaining_prefix, int port) 
    {
        Tree::insertNodeHelper(rootPtr, ip, remaining_prefix, port);
    }

    // Look up an entry in the binary tree
    int getPort(unsigned int ip)
    {
        return Tree::getPortHelper(rootPtr, ip);
    }


    void insertNodeHelper(TreeNode *ptr, unsigned int ip, unsigned int remaining_prefix, int port)
    {
        // This function is called recursively
        // With every recursive call, the remaining_prefix is decremented and the ip is leftshifted by 1
        // If the remaining_prefix is 0 it stops (anything deeper is matched by this route)
        // If the 'new' IP route starts with a 1, create a subtree at rightPointer
        // If the 'new' IP route starts with a 0, create a subtree at leftPointer


        if(remaining_prefix == 0) {
            ptr->port = port;
            // Set the current node's port to the passed port
            return;
        }
        

        else if (ip & (0b1<<31))
        // The first bit of the IP starts with a 1
        {
            if(ptr->rightPtr == nullptr){
                // If the right subtree does not exist, create it
                ptr->rightPtr = new TreeNode();

                // Set the new subtree's interface to the parent's interface
                (ptr->rightPtr)->port = ptr->port;
            }
            insertNodeHelper((ptr->rightPtr), ip << 1, --remaining_prefix, port);
            return;
        }


        else
        // The first bit of the IP starts with a 0
        {
            if(ptr->leftPtr == nullptr){
                // If the left subtree does not exist, create it
                ptr->leftPtr = new TreeNode();

                // Set the new subtree's interface to the parent's interface
                (ptr->leftPtr)->port = ptr->port;
            }
            
            // Recursively call the insertNodeHelper with decremented remaining_prefix and leftshifted IP
            insertNodeHelper((ptr->leftPtr), ip << 1, --remaining_prefix, port);
            return;
        }
    }




    int getPortHelper(TreeNode *ptr, unsigned int ip)
    {             
        if(ip & (0b1<<31)) {
            // First bit of the IP address == 1
            if (ptr->rightPtr == nullptr) {
                // Current node rightptr is a nullptr, there is no longer prefix
                return ptr->port;
                // Return current node port
            }
            else {
                // Return getPortHelper (current node rightptr, ip leftshifted by 1)
                // Recursively call getPortHelper and every time shift the ip 1 to the left
                return getPortHelper(ptr->rightPtr, ip << 1);
            }
        }


        else 
        // First bit of the IP address == 0 
        {
            if ((ptr)->leftPtr == nullptr) {
                // Current node rightptr is a nullptr, there is no longer prefix
                return ptr->port;
                // Return current node port
            }
            else {
                // Return getPortHelper (current node rightptr, ip leftshifted by 1)
                // Recursively call getPortHelper and every time shift the ip 1 to the left
                return getPortHelper(ptr->leftPtr, ip << 1);
            }
        }
    }
};




std::vector<ip_entry> ip_entrys;
int reftemp;


std::vector<ip_entry> ip_entrys;
int reftemp;

Tree portTree;


void init() {
}

// Add route to binary tree lookup table
void add_route(unsigned int ip, int prefix_length, int port_number) {
    // Ignore the first 8 prefix bits
    prefix_length -= 8;

    // Location of binary tree in LUT is just the first 8 bits = 32 bits shifted 24 times right
    portTree[ip>>24].insertNode(ip<<8, prefix_length, port_number);
}

void finalize_routes() {
    std::cout << "Done" << std::endl;
    // TODO: (optionally) use this to finalize your routes.
}
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
                bestie.pn = iterator.pn;
        }
    }
    return bestie.pn;
}

int lookup_ip(unsigned int ip) {
    // Location of binary tree in LUT is just the first 8 bits = 32 bits shifted 24 times right
    return portTree[ip>>24].getPort(ip<<8);
}