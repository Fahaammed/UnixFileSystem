#include "FileSystem.h"
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <map>
#include <set> 
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
int findBit(uint8_t byteFlag, int whichBit);
void fs_cd(char name[5]);
string convertToString(char* a, int size);
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
    int ind = 0;
    for(int i=16; i < 1024 ;){
        Inode newInode;
        newInode.name[0]= superBlock[i++];
        newInode.name[1]= superBlock[i++];
        newInode.name[2]= superBlock[i++];
        newInode.name[3]= superBlock[i++];
        newInode.name[4]= superBlock[i++];

        newInode.used_size = superBlock[i++];
        newInode.start_block = superBlock[i++];
        newInode.dir_parent =superBlock[i++];

        S1->inode[ind] = newInode;
        ind++;
    }

    // consistency check 1: Blocks that are marked free in the free-space list cannot be allocated to any file. Similarly, blocks
    //                      marked in use in the free-space list must be allocated to exactly one file.

    int pos = 0;
    map<int, int> pos_map;
    for (int i = 0; i < 16; i++){                                                           // creating the map of position value pairs
        uint8_t tempByte = S1->free_block_list[i];
        uint8_t mask = 0x80U;
        for(int j=0; j < 8; j++){
            if(mask & tempByte){
                pos_map.insert(pair<int,int>(pos,1));
            }
            else{
                pos_map.insert(pair<int,int>(pos,0));
            }
            mask = mask >> 1;
            pos++;
        }  
    }

    for(int i=0;i<126; i++){
        uint8_t startBlock = S1->inode[i].start_block;
        uint8_t inUse_byte = S1->inode[i].used_size;
        int inUse_bit = findBit(inUse_byte, 8);
        if(inUse_bit == 1){
            uint8_t size = inUse_byte << 1;
            uint8_t size = inUse_byte >> 1;
            for(int j=0; j < size; j++ ){
                int pos_bit = pos_map.find(startBlock + j)->second;
                if(pos_bit == 1){
                    pos_map.erase(startBlock + j);
                }
                if(pos_bit == 0){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 1)" << endl;
                    return;
                }
            }
        }
    }

    pos_map.erase(0);
    for(auto it = pos_map.begin(); it != pos_map.end(); it++){
        if(it->second == 1){
            cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 1)" << endl;
            break;
        }
    }
    
    // consistency check 2: The name of every file/directory must be unique in each directory.

    set<string> dir_names;                                                  // creating a set of directory names


    // consistency check 3: If the state of an inode is free, all bits in this inode must be zero. Otherwise, the name attribute stored
    //                      in the inode must have at least one bit that is not zero.

    for (int i = 0; i < 126 ;i++){
        Inode temp = S1->inode[i];
        uint8_t inUse_byte = temp.used_size;
        int inUse_bit = findBit(inUse_byte, 8);
        if (inUse_bit == 0){                                                                        // not occupied
            for(int j=0; j<5; j++){                                                                 // check for name
                if((int)temp.name[j] != 0){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
                    break;    
                }
            }
            if((temp.dir_parent != 0) || (temp.start_block != 0) || (temp.used_size != 0)){         // check for other shit
                cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
            }
        }
        if(inUse_bit == 1){                                                                         // occupied
                bool name_flag = false;
                for(int j=0; j<5; j++){                                                             // check for name
                    if((int)temp.name[j] != 0){
                        name_flag = true;
                        break;    
                    }
                }
                if (name_flag == false){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
                }
        }
    }
    
    // consistency check 4: The start block of every inode that is marked as a file must have a value between 1 and 127
    //                      inclusive.

    for (int i = 0; i < 126 ;i++){
        Inode temp = S1->inode[i];
        uint8_t inUse_byte = temp.used_size;
        int inUse_bit = findBit(inUse_byte, 8);
        if (inUse_bit == 1){
            uint8_t isFile_byte = temp.dir_parent;
            int isFile_bit = findBit(isFile_byte, 8);
            if(isFile_bit == 0){                                                    // it's a file
                uint8_t startB = temp.start_block;
                if(startB < 1 || startB > 127){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 4)" << endl;
                }
            }
        }
    }


    
    // consistency check 5: The size and start block of an inode that is marked as a directory must be zero.
    for (int i = 0; i < 126 ;i++){
        Inode temp = S1->inode[i];
        uint8_t inUse_byte = temp.used_size;
        int inUse_bit = findBit(inUse_byte, 8);
        if (inUse_bit == 1){
            uint8_t isDir_byte = temp.dir_parent;
            int isDir_bit = findBit(isDir_byte, 8);
            if(isDir_bit == 1){
                int size = inUse_byte << 1;
                if(size != 0 || temp.start_block != 0){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 5)" << endl;
                }
            }
        }
    }
    // consistency check 6: For every inode, the index of its parent inode cannot be 126. Moreover, if the index of the parent inode
    //                      is between 0 and 125 inclusive, then the parent inode must be in use and marked as a directory.

    for (int i = 0; i < 126 ;i++){
        Inode temp = S1->inode[i];
        uint8_t inUse_byte = temp.used_size;
        int inUse_bit = findBit(inUse_byte, 8);
        if (inUse_bit == 1){
            uint8_t parent_byte = temp.dir_parent;
            parent_byte = parent_byte >> 1;
            if(parent_byte == 126){
                cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 5)" << endl;
            }
            if(parent_byte => 0 && parent_byte <= 125){
                uint8_t inUse_byte2 = S1->inode[parent_byte].used_size;
                int inUse_bit2 = findBit(inUse_byte2, 8);
                uint8_t isDir_byte = S1->inode[parent_byte].dir_parent;
                int isDir_bit = findBit(isDir_byte, 8);
                if(inUse_bit2 != 1 || isDir_bit != 1){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 5)" << endl;
                }

            }

        }
    }

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

int findBit(uint8_t byteFlag, int whichBit)
{
    if (whichBit > 0 && whichBit <= 8)
        return (byteFlag & (1<<(whichBit-1)));
    else
        return 0;
}

string convertToString(char* a, int size) 
{ 
    int i; 
    string s = ""; 
    for (i = 0; i < size; i++) { 
        s = s + a[i]; 
    } 
    return s; 
} 
