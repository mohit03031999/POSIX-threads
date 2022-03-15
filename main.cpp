// basic file operations
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <ctype.h> 
#include <unistd.h>
#include <sys/stat.h>

#include "countwords.h"    
using namespace std;

/* Total number of files */
#define NUMOFFILES 2
/* Index for Dictionary source file */
#define DICTSRCFILEINDEX 0
/* Index for Test file */
#define TESTFILEINDEX 1
/* default number of progress marks for representing 100% progress */
#define DEFAULT_NUMOF_MARKS 50
/* minimum number of progress marks for representing 100% progress */
#define MIN_NUMOF_MARKS 10
/* place hash marks in the progress bar every N marks */
#define DEFAULT_HASHMARKINTERVAL 10
/* default minimum number of dictionary words starting from a prefix for printing or
writing to the output */
#define DEFAULT_MINNUM_OFWORDS_WITHAPREFIX 1


//Shared Data Structure between threads */
typedef struct {

 dictNode *dictRootNode; //root node of the dictionary tree
 int numOfProgressMarks; //parameters for printing progress bar
 int hashmarkInterval;  //parameters for hash interval in progress bar
 int minNumOfWordsWithAPrefixForPrinting; //Print a word and its count to the output file only if the count is equal to or greater than this number
 const char *filePath[NUMOFFILES]; //File path for Dictionary source and text file
 long totalNumOfCharsInFile[NUMOFFILES]; //stores total number of characters in files
 long *numOfCharsProcessedFromFile[NUMOFFILES]; //to track ongoing progress of number of characters read in from files.
 long wordCountInFile[NUMOFFILES]; //Wordcount in a file
 bool taskCompleted[NUMOFFILES]; //boolean flag to indicate wther the task is completed
} EXEC_STATUS;

//Function to get the total bytes of a file
//Paramters: File Path
long byteCounter(const char* file)
{   
    struct stat buf = {0};
    lstat(file, &buf); //lstat to retrieve the number of bytes in file
    long num_bytes = buf.st_size; //Total size in bytes of the buffer
    return num_bytes; //Return the number of bytes of the file passed in
}

//Function to print the progress bar
//Paramters: Total Hypen printed, Hash Interval
void print_progress_bar(int hyphens, int interval ) 
{
    if(hyphens % interval == 0 && hyphens!=0 ) { //Print a # at hash interval
        cout << '#';
        cout.flush(); //Request that they be printed immediately
    } else {
        cout << '-';
        cout.flush(); //Request that they be printed immediately
    }
}

//Pthraed function to populate the tree
//Paratemers: Shared Data Structure of type EXEC_STATUS
void *populatetree_func( void * status_p )
	{
		EXEC_STATUS *status = (EXEC_STATUS *)status_p; // casting the parameter back to the struct from (void *)
	  	
		long total_words = 0;   // Variable to store total word count of a file
		long curr = 0;          //Current amount of bytes that you have read in
		long *current_bytes = &curr;
		
		const char *delimiters = "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~";
		status->numOfCharsProcessedFromFile[DICTSRCFILEINDEX] = current_bytes;     //Storing Current bytes processed in shared data structure

		populatetree *dic = new populatetree;        
		
		std::string s = status->filePath[DICTSRCFILEINDEX];
		std::ifstream dictstream(s);                 // Opening the dictionary source file to store the word in the tree
		std::string line;                            // Traversing through each line of the file

		if(dictstream.fail())
		{
        	cout << "Unable to open <<" << status->filePath[DICTSRCFILEINDEX] << ">>" <<endl;    //Error if the file cannot open
			exit(0);
    	}
		while (std::getline(dictstream, line))
		{
			char *line_c = strdup(line.c_str());
			const char *word = strtok(line_c, delimiters);              // Extracting the words from each line of the file
			while (word != nullptr)        
			{ 
				curr = curr + (strlen(word));                        //Storing the bytes processed of the current word in the variable defined above
				curr = curr + 1;                                  	 // +1 for the null pointer
				dic->insert(status->dictRootNode, word);             //Passing the extracted word to insert function to insert the word in the tree
				total_words = total_words + 1;               		//Increasing the counter of total word by 1
				word = strtok(NULL, delimiters);  
			}
		}
		status->wordCountInFile[DICTSRCFILEINDEX] = total_words;    //Storing the total words processed ina file in shared data structure.
		status->taskCompleted[DICTSRCFILEINDEX] = true;				// Making the flag of task completed for populatree pthread to TRUE.
	  	pthread_exit(0);
	}


//Pthraed function to count the number of words
//Paratemers: Shared Data Structure of type EXEC_STATUS
void * countwords_func( void * exec_status_count )
{
	EXEC_STATUS *status = (EXEC_STATUS *)exec_status_count;           // casting the parameter back to the struct from (void *)
	while(status->taskCompleted[DICTSRCFILEINDEX] != true)           //Loop for pthread to execute after the pouplate tree pthread is compltetd
	{
	}
	
	long total_words_test = 0;     									// Variable to store total word count of a file
	long curr = 0; 													//Current amount of bytes that you have read in
	long *current_bytes = &curr;

	const char *delimiters = "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~";
	status->numOfCharsProcessedFromFile[TESTFILEINDEX] = current_bytes;

	countwords *count = new countwords;
		
	std::string test_file = status->filePath[TESTFILEINDEX];
		
	ofstream outfile("countwords_output.txt");							// Opening the output file
	std::ifstream dictstream1(test_file); 								// Opening the test file  
    std::string line_test;    											// Traversing through each line of the file

	if(dictstream1.fail())
	{
        cout << "Unable to open <<" << status->filePath[TESTFILEINDEX] << ">>" <<endl;				//Error if the file cannot open
		exit(0);
    }
	while (std::getline(dictstream1,line_test))
	{
		// Conversion from str to char
		char ctostr[line_test.size() + 1];
		strcpy(ctostr, line_test.c_str());
			
		// Updating bytes processed
		int lineSize = sizeof(ctostr);
		curr += lineSize;
			
		// Counting word tokens on a line delimited by whitespace characters
		char *token = strtok(ctostr, delimiters);
		while (token != NULL)
		{
			int value = count->wordssearch(status->dictRootNode, token);
			if (value >= status->minNumOfWordsWithAPrefixForPrinting)
			{
				outfile << token << " " << value << endl;
			}
			total_words_test = total_words_test + 1;
			token = strtok(NULL, delimiters);
		}
	}
	status->wordCountInFile[TESTFILEINDEX] = total_words_test;           //Storing the total words processed ina file in shared data structure.
	status->taskCompleted[TESTFILEINDEX] = true;						// Making the flag of task completed for populatree pthread to TRUE.
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	// Declare threads 
	pthread_t countwords;
	pthread_t populatetree;

	struct dictNode *root = new dictNode;

	// declaring and initializing the default values to EXEC_STATUS struct
	EXEC_STATUS * exec_status = (EXEC_STATUS *)malloc(sizeof(EXEC_STATUS));
	
	exec_status->dictRootNode = root;
	exec_status->numOfProgressMarks = DEFAULT_NUMOF_MARKS;
	exec_status->hashmarkInterval = DEFAULT_HASHMARKINTERVAL;
	exec_status->minNumOfWordsWithAPrefixForPrinting = DEFAULT_MINNUM_OFWORDS_WITHAPREFIX ;
	exec_status->taskCompleted[DICTSRCFILEINDEX ] = false;
	exec_status->taskCompleted[TESTFILEINDEX ] = false;
	
	if(argc < 2 )
	{
		cout << "File not provided"<< "\n";
		exit(1);
	}

	else
	{
		char Option;
		while ( ( Option = getopt(argc, argv, "p:h:n:")) != -1) 
		{
			int count = 0;
			/* If the option has an argument, optarg is set to point to the 
			* argument associated with the option.
			*/
			switch (Option) 
			{
				case 'p': /* Assume this takes a number */
				/* optarg will contain the string following -p
				* -n is expected to be an integer in this case, so convert the
				* string to an integer.
				*/
					count = atoi(optarg);  
					if (count < MIN_NUMOF_MARKS)
					{
						cout << "Number of progress marks must be a number and at least 10 "<<std::endl;
						return(0) ;
					}
					else
					{
						exec_status->numOfProgressMarks = count;
						break;
					}

				case 'h': /* optarg points to whatever follows -h */
					count = atoi(optarg);  
					if (count > DEFAULT_HASHMARKINTERVAL || count <=0)
					{
						cout << "Hash mark interval for progress must be a number, greater than 0, and less than or equal to 10  "<<std::endl;
						return(0) ;
					}
					else
					{
						exec_status->hashmarkInterval = count;
						break;
					}

				case 'n':  /* optarg points to whatever follows -n */
				
					count = atoi(optarg);  
					exec_status->minNumOfWordsWithAPrefixForPrinting = count;
					break;

				default:
					return(0);
			}
		}
	}
	

	int idx = optind;

	if (idx < argc)													//Condition to check if the input files not present
	{
		if (argv[idx] == std::string("dictionarysource.txt") && argv[idx+1] == NULL )
		{
			cout << "Unable to open <<testfile1.txt>>"<< "\n";
			exit(1);
		}
		else if (argv[optind] == std::string("testfile1.txt") && argv[optind+1] == NULL)
		{
			cout << "Unable to open <<dictionarysource.txt>>"<< "\n";
			exit(1);
		}
  	}
	else
	{
		cout << "Both Files not provided" <<endl;
		return(0);
	}


	const char* dictionary_source = argv[optind];
	exec_status->filePath[DICTSRCFILEINDEX] = dictionary_source;   //Storing the Dicitionary Source file in EXEC_STATUS struct


	const char* test_text = argv[optind+1];  
	exec_status->filePath[TESTFILEINDEX ] = test_text;		//Storing the Test file in EXEC_STATUS struct

     
	long num_bytes_dic = byteCounter(exec_status->filePath[DICTSRCFILEINDEX]); //Calcualting number of bytes found from file 
    exec_status->totalNumOfCharsInFile[DICTSRCFILEINDEX] = num_bytes_dic;	//Storing the number of bytes found from Dicitionary Source file in EXEC_STATUS struct
	
	long num_bytes_test = byteCounter(exec_status->filePath[TESTFILEINDEX]); //Calcualting number of bytes found from file 
	exec_status->totalNumOfCharsInFile[TESTFILEINDEX] = num_bytes_test;		//Storing the number of bytes found from Test file in EXEC_STATUS struct

	//Creating the two Pthread
	pthread_create(&populatetree, NULL, &populatetree_func, (void *) exec_status);
	pthread_create(&countwords, NULL, &countwords_func, (void *) exec_status);
	
	int processed_hyphens = 0;  
	int total_hyphens = 0;     //Total amount of hyphens processed
	
	while (exec_status->taskCompleted[DICTSRCFILEINDEX] != true)
	{
		long *current = exec_status->numOfCharsProcessedFromFile[DICTSRCFILEINDEX];  //Extracting current bytes processed in a file
		long size = exec_status->totalNumOfCharsInFile[DICTSRCFILEINDEX];         //Extracting the total bytes of a file
		int hash_mark = exec_status->hashmarkInterval;                        //Extracting the hash mark interval
		if(total_hyphens < exec_status->numOfProgressMarks) 
			{
				float temp = (float)(*(current))/(float)size; 
				total_hyphens = (temp * (exec_status->numOfProgressMarks));
				if(total_hyphens > processed_hyphens) 
				{
					print_progress_bar(total_hyphens,hash_mark);  //Call function to print hyphens
					processed_hyphens = total_hyphens;
				}
			}

	}
	cout << '\n' << "There are " << exec_status->wordCountInFile[DICTSRCFILEINDEX] << " words in " << exec_status->filePath[DICTSRCFILEINDEX] << "." << '\n';
	
	int processed_hyphens_test = 0;
	int total_hyphens_test = 0;     //Total amount of hyphens processed
	while (exec_status->taskCompleted[TESTFILEINDEX] != true)
	{
		long *current = exec_status->numOfCharsProcessedFromFile[TESTFILEINDEX];         //Extracting current bytes processed in a file
		long size = exec_status->totalNumOfCharsInFile[TESTFILEINDEX];                   //Extracting the total bytes of a file
		int hash_mark = exec_status->hashmarkInterval;                                   //Extracting the hash mark interval
		if(total_hyphens_test < exec_status->numOfProgressMarks)
			{
				float temp = (float)(*(current))/(float)size;
				total_hyphens_test = (temp * (exec_status->numOfProgressMarks));
				if(total_hyphens_test > processed_hyphens_test) 
				{
					print_progress_bar(total_hyphens_test,hash_mark); //Call function to print hyphens
					processed_hyphens_test = total_hyphens_test;
				}
			}

	}
	cout << '\n' << "There are " << exec_status->wordCountInFile[TESTFILEINDEX] << " words in " << exec_status->filePath[TESTFILEINDEX] << "." << '\n';
	return 0;
}