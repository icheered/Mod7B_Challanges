#include "treenode.h"

class Tree
{
private:
    TreeNode *rootPtr;
public:
    Tree() : rootPtr(nullptr) { /* empty body */}

    void insertNode() //  IP, Prefix Length, Port
    {
        // Invert IP
        // remaining_prefix = Prefix Length
        // insertNodeHelper(rootptr, inverted_ip, remaining_prefix, port)
    }
    void getPort()
    {
        // Invert IP
        // Return getPortHelper(rootptr, inverted_ip)
    }
};

void insertNodeHelper(TreeNode **ptr, unsigned int inverted_ip, unsigned int remaining_prefix, unsigned int port)
    {
        // If remaining_prefix == 0
            // If port > current node value
                // Set current node value to port
            // Else 
                // return
        
        // Else if current IP bit == 1:
            // If current node rightptr == nulptr
                // Create new node
                // Set current node rightptr to new node pointer
            // insertNodeHelper ( current node rightptr, inverted IP rightshifted, remaining_prefix -= 1, port)
        
        // Else if current IP bit == 0:
            // If current node leftptr == nulptr
                // Create new node
                // Set current node leftptr to new node pointer
            // insertNodeHelper ( current node leftptr, inverted IP rightshifted, remaining_prefix -= 1, port)
    } 

void getPortHelper(TreeNode **ptr, unsigned int inverted_ip)
{
    // If current IP bit == 0 
        // If current node leftptr == nullptr
            // Return current node port
        // Else 
            // Return getPortHelper (current node leftptr, inverted_ip rightshifted)
    // Else If current IP bit == 1 
        // If current node rightptr == nullptr
            // Return current node port
        // Else 
            // Return getPortHelper (current node rightptr, inverted_ip rightshifted)
}