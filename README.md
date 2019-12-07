# UnixFileSystem
This program is intended to give experience in writing a simple UNIX-like file system.

Design choices:
    * Creating reusablse  and helper functions to modularise the code
        - findBit()             - to find the value of a specific bit in a byte
        - findBlock()           - to find the contagious blocks
        - deleteInode()         - delete everything associatiated with an inode
        - convertToString()     - convert a char array to a string
        - tokenize()            - to tokenize a string by a delimiter
        - changeDisk()          - write the superblock into the disc
        - fileDirectory()       - create a vector which contains the list of inode                                  indexes of the childs of a given directory
    * besides these other important choices were made to recursively delete files in fs_delete, made the for loops optimized by having multiple conditions so that it does less work



System Calls:
    - getline() --> to read input
    - memset() --> fill buffre with a constant byte. usually to clear the buffer
    - memcpy() --> to copy oe buffer to another
    - open() --> to open the files
    - read() --> to read from the files 
    - write() --> to write to the files

Testing:
    - tested using the testcases given on the eclass
    - valgring to minimize the memory leaks
    - and some made some test cases on my own to test the program and edgecases

Things didn't implement:
    - defrag
    - resize
    even though I structured the code enough to make them easy to implement but did not have enough time to do them.
    ** if it does not run comment out fs ls"