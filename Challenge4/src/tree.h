#include "treenode.h"

class Tree
{
private:
    TreeNode *rootPtr;
public:
    Tree() : rootPtr(nullptr) { /* empty body */}

    void insertNode(unsigned int ip, unsigned int remaining_prefix, int port) //  IP, Prefix Length, Port
    {
        insertNodeHelper(&rootPtr, ip, remaining_prefix, port);
    }
    void getPort(unsigned int ip)
    {
        return getPortHelper(&rootPtr, ip);
    }
};

void insertNodeHelper(TreeNode **ptr, unsigned int ip, unsigned int remaining_prefix, int port)
    {

        // If remaining_prefix == 0
            // If port > current node value
                // Set current node value to port
            // Else 
                // return
        if(remaining_prefix == 0) {
            int currentPort = (*ptr)->port;
            if(port > currentPort) {
                (**ptr).port = port;
            }
            return;
        }
        



        // Else if current IP bit == 1:
            // If current node rightptr == nulptr
                // Create new node
                // Set current node rightptr to new node pointer
            // insertNodeHelper ( current node rightptr, inverted IP rightshifted, remaining_prefix -= 1, port)
        else if (ip & (0x1<<31))
        {
            if((*ptr)->rightPtr == nullptr){
                (**ptr).rightPtr = new TreeNode();
            }
            insertNodeHelper(&((*ptr)->rightPtr), ip << 1, remaining_prefix--, port);
        }



        // Else if current IP bit == 0:
            // If current node leftptr == nulptr
                // Create new node
                // Set current node leftptr to new node pointer
            // insertNodeHelper ( current node leftptr, inverted IP rightshifted, remaining_prefix -= 1, port)
        else if (!(ip & (0x1<<31)))
        {
            if((*ptr)->leftPtr == nullptr){
                (**ptr).leftPtr = new TreeNode();
            }
            insertNodeHelper(&((*ptr)->leftPtr), ip << 1, remaining_prefix--, port);
        }
    } 

void getPortHelper(TreeNode **ptr, unsigned int inverted_ip)
{
    // If current IP bit == 0 
        // If current node leftptr == nullptr
            // Return current node port
        // Else 
            // Return getPortHelper (current node leftptr, inverted_ip rightshifted)

    if (!(inverted_ip & (0x1 << 31))) {
        if ((**ptr).leftPtr == nullptr) {
            return (**ptr).port;
        }
        else {
            getPortHelper(&((*ptr)->leftPtr), inverted_ip >> 1);
        }

    }
    // Else If current IP bit == 1 
        // If current node rightptr == nullptr
            // Return current node port
        // Else 
            // Return getPortHelper (current node rightptr, inverted_ip rightshifted)
    else if ((inverted_ip & (0x1 << 31))) {
            if ((**ptr).leftPtr == nullptr) {
                return (**ptr).port;
            }
            else {
                getPortHelper(&((*ptr)->rightPtr), inverted_ip >> 1);
            }
    }
}