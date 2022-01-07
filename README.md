# Basic-File-System
Includes C++ file, make-disk executable and makefile

Program takes a 128KB file partitioned using an executable file and reads in another file with the following prefixes. C for create, D for delete, L for list,
R for read, and W for write.

Example of input file:

C file1.cpp 2 // add an empty named file1.cpp and allocate 2 blocks for the file
D file1.cpp // searches for a file named file1.cpp and "deletes" it (deallocates blocks so they can be used by another file)
R file1.cpp 1 // reads in block #1 from file1.cpp and prints it
W file1.cpp 0 // writes to the first block (block 0) of the specified file (file1.cpp)
L // lists all the files currently in the directory

Before all of these commands in the file, the first line of the file is the name of the file acting as a disk
e.x. mydisk0 is the first line, means that the 128KB file being used for the file system named mydisk0

E.x. of file that would be passed in

mydisk0
C file1 2
C file2 5
C file3 6
L
D file1
W file2 3
R file2 3
D file2 5

make-disk is the executable that creates a 128KB partition
Note: make-disk is not mine, was executable that was provided along with assignment description
