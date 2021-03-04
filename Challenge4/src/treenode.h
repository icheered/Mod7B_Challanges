class TreeNode
{
private:
   TreeNode *leftPtr;      // pointer to left subtree
   unsigned short int port;// port number
   TreeNode *rightPtr;     // pointer to left subtree
public:
// constructor
    TreeNode(  )   
      : leftPtr( nullptr ), // pointer to left subtree
        port( -1 ), // tree node data
        rightPtr( nullptr ) // pointer to right substree
   { 
      // empty body 
   } // end TreeNode constructor
};