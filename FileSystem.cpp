#include "FileSystem.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
using namespace std;


void fs_mount(char *new_disk_name);
void fs_create(char name[5], int size);
void fs_delete(char name[5]);
void fs_read(char name[5], int block_num);
void fs_write(char name[5], int block_num);
void fs_buff(uint8_t buff[1024]);
void fs_ls(void);
void fs_resize(char name[5], int new_size);
void fs_defrag(void);
void fs_cd(char name[5]);
vector<string> tokenize(const string &str, const char *delim);
bool fexists(const char *filename);



int main(void){

    string input;
    // string inter;
    vector<string> string_input_list;

    getline(cin, input);

    string_input_list = tokenize(input, " ");
    
    
    
    if(string_input_list[0] == "M" && string_input_list.size() == 2){
        char *convert = new char[string_input_list[1].size() + 1];
        copy(string_input_list[1].begin(),string_input_list[1].end(),convert);
        convert[string_input_list[1].size()] = '\0';
        fs_mount(convert);
        delete[] convert;
    }


    else if(string_input_list[0] == "C" && string_input_list.size() == 3){
        if(string_input_list[1].size() < 6){
            char name[5];
            strcpy(name,string_input_list[1].c_str() );
            int size = stoi(string_input_list[2]);
            fs_create(name, size);
            delete[] name;
        }
        
    }


    else if(string_input_list[0] == "D" && string_input_list.size() == 2){
        if(string_input_list[1].size() < 6){
            char name[5];
            strcpy(name,string_input_list[1].c_str() );
            fs_delete(name);
            delete[] name;
        }
    }

    else if(string_input_list[0] == "R" && string_input_list.size() == 3){
         if(string_input_list[1].size() < 6){
            char name[5];
            strcpy(name,string_input_list[1].c_str() );
            int blockNum = stoi(string_input_list[2]);
            fs_read(name, blockNum);
            delete[] name;
        }
    }

    else if(string_input_list[0] == "W" && string_input_list.size() == 3){
        if(string_input_list[1].size() < 6){
            char name[5];
            strcpy(name,string_input_list[1].c_str() );
            int blockNum = stoi(string_input_list[2]);
            fs_write(name, blockNum);
            delete[] name;
        }
    }

    else if(string_input_list[0] == "B" && string_input_list.size() == 2){
        uint8_t buff[1024];
        copy(string_input_list[1].begin(),string_input_list[1].end(),buff);
        fs_buff(buff);
        delete[] buff;
    }

    else if(string_input_list[0] == "L" && string_input_list.size() == 1){
        fs_ls();
    }

    else if(string_input_list[0] == "E" && string_input_list.size() == 3){

        if(string_input_list[1].size() < 6){
            char name[5];
            strcpy(name,string_input_list[1].c_str() );
            int newSize = stoi(string_input_list[2]);
            fs_resize(name, newSize);
            delete[] name;
        }
    }

    else if(string_input_list[0] == "O" && string_input_list.size() == 1){
        fs_defrag();
    }

    else if(string_input_list[0] == "Y" && string_input_list.size() == 2){
        if(string_input_list[1].size() < 6){
            char name[5];
            strcpy(name,string_input_list[1].c_str() );
            fs_cd(name);
            delete[] name;
        }
    }

    else{
        cout << "Command Error: " << endl;
    }

    return 0;
}


void fs_mount(char *new_disk_name){
    cout<<"Disk name: "<<new_disk_name<<endl;

    int fp = open(new_disk_name, O_RDWR);
    if(fp < 0){
        cerr << "Cannot find disk " << new_disk_name << endl;
        return;
    }
    // Super_block superBlock;
    uint8_t superBlock[1024];
    ssize_t superBlock_size = 1024;

    if(lseek(fp,0,SEEK_SET)<0){
        cout << "failed to lseek" << endl;
    }
    if(read(fp,superBlock,superBlock_size) != superBlock_size){
        cout << "failed to read" << endl;
    }
    else{
        cout << "success" << endl;
    }

    Super_block *S1 = new Super_block; 
    // insert first 16 bytes into the free block list
    for (int j=0 ; j < 16; j++) {
        S1->free_block_list[j] = superBlock[j];

        cout << "in the superBlock " << unsigned(superBlock[j]) << endl;
        cout << "in the structure " << (int)(S1->free_block_list[j]) << endl;
    }
    // insert the inodes in the inodes list in the Super_block structure
    for(int i=16; i < 1024 ;){
        Inode *newInode = new Inode;
        newInode->name[0]= superBlock[i++];
        newInode->name[1]= superBlock[i++];
        newInode->name[2]= superBlock[i++];
        newInode->name[3]= superBlock[i++];
        newInode->name[4]= superBlock[i++];

        newInode->used_size = superBlock[i++];
        newInode->start_block = superBlock[i++];
        newInode->dir_parent =superBlock[i++];

        // S1->inode[(i-16)/8] = &newInode;

    }
    // int inodes_list[126] = {0};
    // int inode_index = 0;
    // for (int i=17; i < 1024; i+=8){
    //     inodes_list[inode_index] = i;
    //     inode_index++;
    //     cout<< inodes_list[inode_index-1] <<"       count: "<< inode_index << "        i value "<< i << endl;
    // }
}

void fs_create(char name[5], int size){
    cout << "inside fs create, filename: "<< name << ",  file size: " << size << endl;
    return;
}
void fs_delete(char name[5]){
    cout << "inside fs delete, filename: "<< name << endl;
    return;
}
void fs_read(char name[5], int block_num){
    cout << "inside fs read, filename: "<< name << ",    block number: " << block_num << endl;
    return;
}
void fs_write(char name[5], int block_num){
    cout << "inside fs write, filename: "<< name << ",   block number: " << block_num << endl;
    return;
}
void fs_buff(uint8_t buff[1024]){
    cout << "inside fs buff, filename: "<< buff << endl;
    return;
}

void fs_ls(void){
    cout << "inside fs ls " << endl;
    return;
}

void fs_resize(char name[5], int new_size){
    cout << "inside fs resize, filename: " << name <<  ",    new size: " << new_size << endl;
    return;
}

void fs_defrag(void){
    cout << "inside fs defrag" << endl;
    return;
}

void fs_cd(char name[5]){
    cout << "inside fs cd, filename: " << name << endl;
}


/**
 * @brief Tokenize a string 
 * 
 * @param str - The string to tokenize
 * @param delim - The string containing delimiter character(s)
 * @return std::vector<std::string> - The list of tokenized strings. Can be empty
 */
vector<string> tokenize(const string &str, const char *delim) {
  char* cstr = new char[str.size() + 1];
  strcpy(cstr, str.c_str());

  char* tokenized_string = strtok(cstr, delim);

  vector<string> tokens;
  while (tokenized_string != NULL)
  {
    tokens.push_back(string(tokenized_string));
    tokenized_string = strtok(NULL, delim);
  }
  delete[] cstr;
  return tokens;
}
