#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <map>
#include <set> 
#include <fstream>
#include <bitset>
typedef struct {
	char name[5];        // Name of the file or directory
	uint8_t used_size;   // Inode state and the size of the file or directory
	uint8_t start_block; // Index of the start file block
	uint8_t dir_parent;  // Inode mode and the index of the parent inode
} Inode;

typedef struct {
	char free_block_list[16];
	Inode inode[126];
} Super_block;

using namespace std;
void fs_mount(char *new_disk_name);
bool fs_mountCheck(char *new_disk_name);
void fs_create(char name[5], int size);
void fs_delete(char name[5]);
void fs_read(char name[5], int block_num);
void fs_write(char name[5], int block_num);
void fs_buff(uint8_t buff[1024]);
void fs_ls(void);
void fs_resize(char name[5], int new_size);
void fs_defrag(void);
int findBit(uint8_t byteFlag, int whichBit);
int findBlock(int requiredSize);
void fs_cd(char name[5]);
void deleteInode(int index);
string convertToString(char* a, int size);
vector<string> tokenize(const string &str, const char *delim);
bool fexists(const char *filename);
void changeDisk();
vector<int> fileDirectory(int directory);