#include "populatetree.h"
using namespace std;
 
#define ALPHABET_SIZE 27
class countwords{

public:
    // Function to count the number of occurences of the word (in the tree) 
    //Parameter: Node Pointer
    //Return: Integer value of number of occurences of word
    int count_occ(struct dictNode *node);  

    //Function to search a word in the dictionary (in the tree) 
    //Parameters:Node Pointer, word (to be searched)
    //Return: Integer value of number of occurences of word
    int wordssearch(struct dictNode *root, const char *word);  
};