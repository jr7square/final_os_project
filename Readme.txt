Final Group Project: Synchronization
COP4600-01: Operating Systems

Authors: Anthony Brancucci, Junior Recinos, Benjamin Culpepper, Juan Carrillo

To compile this code, open the console with the gcc compiler and type the following:

gcc group_project.c linked_queue.c -lpthread -lrt -o gp

The execution of the code requires 2 parameters:

   1. An input file with the a sequence of L and R, separated by commas.
   2. An integer value that represents, in seconds, how long it takes a baboon to cross the rope.

To run the code, type the following into the console after the code has been compiled:

./gp input_file.txt 2

Thus, "input_file.txt" is the data file with a string of "L,R" sequence, and 2 seconds is how fast it takes a baboon to cross the rope. Please ensure that the data file is located in the same folder as the executable file.