# POSIX threading to accomplish a dictionary tree
Program starts from the main thread, which reads and processes command line 
arguments, creates and initializes a shared data structure to be shared between the main 
thread and worker threads, then spawns two worker threads to carry out the following 
sequence of logic in that order:

1. populatetree thread for reading words from the dictionary source file and 
populating a dictionary tree.

2. countwords thread for reading words from a test file, and for every word read in 
from the test file:
