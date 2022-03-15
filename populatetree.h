using namespace std;
 
#define ALPHABET_SIZE 27

struct dictNode
{
    // Collection of nodes that represent subsequent characters in 
    // words. Each node in the next dictNode*  
    // array corresponds to a particular character that continues  
    // the word if the node at the array index is NOT NULL: 
    // e.g., if next[0] is NOT NULL, it means ‘a’ would be  
    // one of the next characters that continues the current word; 
    // while next[0] being NULL means there are no further words  
    // with the next letter as ‘a’ along this path. 
    struct dictNode *next[ALPHABET_SIZE];
    
    // isWord is true if the node represents the  
    // last character of a word 
    bool isWord=false;
};

class populatetree{

public:
    //Function to insert the word into the dictionary
    //Parameters: Node Pointer, word (to be insterted)
    void insert(struct dictNode *root, const char *key); 

    //Function to convert the character to index
    //Parameter:Char (for which index needs to be found)
    //Return: Index value of type integer
    int letterToIndex(char letter);
};


