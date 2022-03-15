using namespace std;
#include <iostream>
#include <cstring>
#include "countwords.h"


// Function to count the number of occurences of the word (in the tree) 
int countwords::count_occ(struct dictNode *node)
{
    int counter = 0;
    if (node != NULL)
    {
        if (node->isWord == true)
        {
            counter++;
        }
        for (int i = 0; i <= 26; i++)
        {
            if (node->next[i] != NULL)
            {
                counter += count_occ(node->next[i]);  //Recursive call the function to count the occurences
            }
        }
    }
    return counter;   // Returning the count value or the word
}

//Function to search a word in the dictionary (in the tree)
int countwords::wordssearch(struct dictNode *root, const char *word)
{
    struct dictNode *current = root;
    populatetree tree;

    if (current != NULL)
    {
        int n = strlen(word);  // Calculating the length of the word
        int i = 0;

        while (i < n)           // While loop until the length of the word
        {
            int index = tree.letterToIndex(word[i]);
            if (current->next[index] != NULL)
            {
                current = current->next[index];
            }
            else                //If the word not found retun 0
            {
                return 0;
            }
            i++;
        }
        if (i == n)
        {
            if (current == NULL)        // If the next node is null return 1 because only 1 word found
            {
                return 1;
            }
            else                       //Else calcualting the number of occurences 
            {
                int count = count_occ(current);
                return count;
            }
        }
    }
}

