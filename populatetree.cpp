using namespace std;
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include "populatetree.h"



// Returns new node (initialized to NULLs)
struct dictNode *getNode(void)
{
    struct dictNode *newNode = new dictNode;
    newNode->isWord = false;  //Initializing the new node with the isWord as false     
   
    for (int i = 0; i < ALPHABET_SIZE; i++)
        newNode->next[i] = NULL;    
   
    return newNode;   // Return the new nodes (Initialized to NULLs) 
}

//Function to convert the character to index
int populatetree::letterToIndex(char letter) 
{
    int index;
    if (letter >= 'A' && letter <= 'Z')  //Convert the uppercase letter to lowercase
    {
        letter = letter +32;
    }

    if (letter=='\'')    //If Apostrophe asign it the 26th index
    {
        index = 26;
    }
    else
    {
        index = letter - 'a';   
    }
    return index;   // Return the index for the character
}

//Function to insert the word into the dictionary
void populatetree::insert(struct dictNode *root, const char *key)
{
    int length = strlen(key);           //Calculating the length of the word
  
    struct dictNode *word = root;
   
    for (int i = 0; i < length; i++)    // iterative loop to traverse the tree structure to insert the word
    {
        int index = letterToIndex(key[i]);
        if (word->next[index] == NULL)
            word->next[index] = getNode();
        word = word->next[index];
    }   
    word->isWord = true;                // Making the flag true for the last character of word
}