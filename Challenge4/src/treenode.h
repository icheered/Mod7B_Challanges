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