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


void ip2human(unsigned int ip);


// you can declare global variables here.

class TreeNode
{

public:
   TreeNode *leftPtr;      // pointer to left subtree
   int port;// port number
   TreeNode *rightPtr;     // pointer to left subtree

   // Constructor
   TreeNode(  )   
      : leftPtr( nullptr ), // pointer to left subtree
        port( -1 ), // tree node data
        rightPtr( nullptr ) // pointer to right substree
   {} 
};




class Tree
{
private:
    TreeNode *rootPtr;
public:
    Tree() : rootPtr(new TreeNode()) { /* empty body */}

    void insertNode(unsigned int ip, unsigned int remaining_prefix, int port) //  IP, Prefix Length, Port
    {
        Tree::insertNodeHelper(rootPtr, ip, remaining_prefix, port);
    }
    int getPort(unsigned int ip)
    {
        return Tree::getPortHelper(rootPtr, ip);
    }


    void insertNodeHelper(TreeNode *ptr, unsigned int ip, unsigned int remaining_prefix, int port)
    {
        // If remaining_prefix == 0
            // If port > current node value
                // Set current node value to port
            // Else 
                // return
        if(remaining_prefix == 0) {
            ptr->port = port;
            return;
        }
        

        // Else if current IP bit == 1:
            // If current node rightptr == nulptr
                // Create new node
                // Set current node rightptr to new node pointer
            // insertNodeHelper ( current node rightptr, inverted IP rightshifted, remaining_prefix -= 1, port)
        else if (ip & (0b1<<31))
        {
            if(ptr->rightPtr == nullptr){
                ptr->rightPtr = new TreeNode();
                (ptr->rightPtr)->port = ptr->port;
            }
            insertNodeHelper((ptr->rightPtr), ip << 1, --remaining_prefix, port);
            return;
        }



        // Else if current IP bit == 0:
            // If current node leftptr == nulptr
                // Create new node
                // Set current node leftptr to new node pointer
            // insertNodeHelper ( current node leftptr, inverted IP rightshifted, remaining_prefix -= 1, port)
       
        else
        {
            if(ptr->leftPtr == nullptr){
                ptr->leftPtr = new TreeNode();
                 (ptr->leftPtr)->port = ptr->port;
            }
            insertNodeHelper((ptr->leftPtr), ip << 1, --remaining_prefix, port);
            return;
        }
    }




    int getPortHelper(TreeNode *ptr, unsigned int ip)
    {
        // If current IP bit == 1 
            // If current node rightptr == nullptr
                // Return current node port
            // Else 
                // Return getPortHelper (current node rightptr, inverted_ip rightshifted)
        if(ip & (0b1<<31)) {
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
        else 
        {
            if ((ptr)->leftPtr == nullptr) {
                return (*ptr).port;
            }
            else {
                return getPortHelper((ptr->leftPtr), ip << 1);
            }
        }
        
        
    }
};


Tree portTree[256];

void init() {
    //read in the txt into data struct
   // You can use this function to initialize variables.
}

void add_route(unsigned int ip, int prefix_length, int port_number) {
    prefix_length -= 8;
    portTree[ip>>24].insertNode(ip<<8, prefix_length, port_number);
}

// This method is called after all routes have been added.
// You don't have to use this method but can use it to sort or otherwise
// organize the routing information, if your datastructure requires this.
void finalize_routes() {
    std::cout << "Done" << std::endl;
}

int lookup_ip(unsigned int ip) {
    return portTree[ip>>24].getPort(ip<<8);
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


