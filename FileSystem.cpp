#include "FileSystem.h"
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
string convertToString(char* a, int size);
vector<string> tokenize(const string &str, const char *delim);
bool fexists(const char *filename);
void changeDisk();
Super_block superBlock;
string diskName;
bool diskSet;
uint8_t buffer[1024];
uint8_t currentDir;
fstream infile;



int main(int argc, char** argv){
    string filename = argv[1];
    fstream fp(filename);
    string input;
    int whileloop = 1;
    diskSet = false;
    while(getline(fp,input)){
        vector<string> string_input_list;
        string_input_list = tokenize(input, " ");
    
        
    // mounting a disk
        if(string_input_list[0] == "M" && string_input_list.size() == 2){
            char *convert = new char[string_input_list[1].size() + 1];
            copy(string_input_list[1].begin(),string_input_list[1].end(),convert);
            convert[string_input_list[1].size()] = '\0';
            if(fs_mountCheck(convert)){
                diskSet = true;
                diskName = convert;
                fs_mount(convert);
            }
            delete[] convert;
        }


        else if(string_input_list[0] == "C" && string_input_list.size() == 3){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int size = stoi(string_input_list[2]);
                fs_create(name, size);
                // delete[] name;
            }
            

        }


        else if(string_input_list[0] == "D" && string_input_list.size() == 2){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                fs_delete(name);
                // delete[] name;
            }
        }

        else if(string_input_list[0] == "R" && string_input_list.size() == 3){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int blockNum = stoi(string_input_list[2]);
                fs_read(name, blockNum);
                // delete[] name;
            }
        }

        else if(string_input_list[0] == "W" && string_input_list.size() == 3){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int blockNum = stoi(string_input_list[2]);
                fs_write(name, blockNum);
                // delete[] name;
            }
            
        }

        else if(string_input_list[0] == "B" && string_input_list.size() > 1){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            else{
                uint8_t buff[1024];
                memset(buff, 0, 1024);
                copy(input.begin()+input.find("B")+2,input.end(),buff);
                fs_buff(buff);
            }
        }

        else if(string_input_list[0] == "L" && string_input_list.size() == 1){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            else{
                fs_ls();
            }
        }

        else if(string_input_list[0] == "E" && string_input_list.size() == 3){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int newSize = stoi(string_input_list[2]);
                fs_resize(name, newSize);
                // delete[] name;
            }
        }

        else if(string_input_list[0] == "O" && string_input_list.size() == 1){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            else{
                fs_defrag();
            }
        }

        else if(string_input_list[0] == "Y" && string_input_list.size() == 2){
            if(diskSet == false){
                cout << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str());
                fs_cd(name);
                // delete[] name;
            }
        }

        else{
            cout << "Command Error: " << endl;
        }
        whileloop++;
    }
    return 0;
}


void fs_mount(char *new_disk_name){
    cout <<"Disk name: "<< new_disk_name << endl;
    
    infile.open(new_disk_name);
    if(infile.fail()){
        cout << "infile error" << endl;
    }

    // superBlock = new Super_block;
    infile.read(superBlock.free_block_list,16);
    for(uint8_t i =0; i<126; i++){
        infile.read(superBlock.inode[i].name,5);
        infile.read((char*)&superBlock.inode[i].used_size,1);
        infile.read((char*)&superBlock.inode[i].start_block,1);
        infile.read((char*)&superBlock.inode[i].dir_parent,1);
    }
    currentDir = 127;
}



void fs_create(char name[5], int size){
    cout << "inside fs create, filename: "<< name << ",  file size: " << size << endl;
    for(int i =0; i < 126; i++){
        uint8_t parentDir = superBlock.inode[i].dir_parent;
        parentDir = parentDir << 1;
        parentDir = parentDir >> 1;
        if(currentDir == parentDir){ 
            string inodeName = superBlock.inode[i].name;
            if(!inodeName.compare(name)){
                cout<< "Error: File or directory "<< name << " already exists" << endl;
                return;
            }        
        }    
    }
    
    int inodeIndex = -1;
    for(int i =0; i<126; i++){
        if(superBlock.inode[i].used_size == 0){
            inodeIndex = i;
            break;
        }
    }
    cout << "inode index: " << inodeIndex << endl;
    if(inodeIndex == -1){                                                                                   // no inode is free
        cout << "Error: Superblock in disk "<< diskName << " is full, cannot create "<< name << endl;
        return;
    }


    if(size == 0){
        
        strncpy( superBlock.inode[inodeIndex].name, name, 5);
        superBlock.inode[inodeIndex].used_size |= (uint8_t)0x80U; // in use = 1
        superBlock.inode[inodeIndex].dir_parent |= (uint8_t)0x80U; // is dir = 1
        superBlock.inode[inodeIndex].dir_parent |= currentDir;
        
        changeDisk();
        // uint8_t inodeBuffer[8];
        // memset(inodeBuffer, 0, 8);
        // memcpy(inodeBuffer,(char *) & superBlock->inode[inodeIndex],8);
        


    }
    else if(size > 0 && size < 128){
        int freeListIndex = findBlock(size);
        if(freeListIndex < 0){
            cout << "Error: Cannot allocate "<< size << " on " << diskName << endl;
        return;
        }
        for(int i = 0; i < 5; i++){
            superBlock.inode[inodeIndex].name[i] = name[i];
            cout<< superBlock.inode[inodeIndex].name[i]<< endl;
        }
        // strncpy(name, superBlock->inode[inodeIndex].name, 5);                                                           // set the name
        superBlock.inode[inodeIndex].used_size = (uint8_t)size;                                              // set the startBlock
        superBlock.inode[inodeIndex].used_size |= (uint8_t)0x80U;                                                      // in use = 1
        superBlock.inode[inodeIndex].dir_parent = currentDir;
        superBlock.inode[inodeIndex].start_block = (uint8_t)freeListIndex+1;

        cout << "name " << (char *)superBlock.inode[inodeIndex].name << endl;
        cout << "used_size " << unsigned(superBlock.inode[inodeIndex].used_size) << endl;        
        cout << "start Block " << unsigned(superBlock.inode[inodeIndex].start_block) << endl;
        cout << "dirparent " << unsigned(superBlock.inode[inodeIndex].dir_parent) << endl;

        for(int i = freeListIndex+1; i < freeListIndex+1+size; i++){
            int byte_index = i / 8;
            int bit = i % 8;
            uint8_t byte = superBlock.free_block_list[byte_index];
            uint8_t mask_shift = 7 - bit;
            uint8_t mask = 1 << mask_shift;
            uint8_t new_byte = byte | mask;
            superBlock.free_block_list[byte_index] = new_byte;
        }
        
    changeDisk();
    }
    return;
}


void fs_delete(char name[5]){
    // createmap();
    return;
    cout << "inside fs delete, filename: "<< name << endl;
    int index = -1;                                                             // if an actual dir name is given then go into that dir
    for (int i = 0;i<126; i++){
        if(strncmp(name,superBlock.inode[i].name,5)){
            uint8_t tempIndex = superBlock.inode[i].dir_parent;
            int isDir = tempIndex >> 7;
            tempIndex = tempIndex << 1;
            tempIndex = tempIndex >> 1;
            if((isDir == 0) && (tempIndex == currentDir)){                      // it's a file
                // delete that file and re write it's datablocks to 0, flip the bits in free blocklis and then return 
                index = i;
            }
            if((isDir == 1) && (tempIndex == currentDir)){                      // set the index to i and break out of the loop
                index = i;
                break;
            }                       
        }
    }

    if(index < 0){
        cout << "Error: File or directory " << name << " does not exist" << endl;
    }
    // else{
    //     vector<file> files;
    //     vector<directory> directories;
    //     char currentParent[5];
    //     strncpy(superBlock.inode[index].name,currentParent,5);
    //     for (int i=0; i<126; i++){
    //         uint8_t parentDir = superBlock->inode[i].dir_parent;
    //         parentDir = parentDir << 1;
    //         parentDir = parentDir >> 1;
    //         char parentName[5];
    //         strncpy(superBlock.inode[parentDir].name,parentName,5);
    //         if(strncmp(parentDir,currentParent)){
    //             uint8_t tempIndex = superBlock->inode[i].dir_parent;
    //             int isDir = tempIndex >> 7;
    //             if(isDir == 0){

    //             }
    //         }
    //     }



    // }







    return;
}


void fs_read(char name[5], int block_num){
    cout << "inside fs read, filename: "<< name << ",    block number: " << block_num << endl;

    // string name_str(name);
    // uint8_t block;
    // for (int i = 0;i<126;i++){
    //     string inode_name(SuperBlock.inode[i].name);
    //     //add a check for current directory. Check parent directory. make parent dir global
    //     if (inode_name == name_str){// && SuperBlock->inode[i].dir_parent == current dir
    //         block = SuperBlock.inode[i].start_block;
    //         int position = block + block_num;
    //         if (position <= (block + (SuperBlock.inode[i].used_size & 127))){
    //             infile.seekg(position*1024,ios::beg);
    //             infile.read(blockBuffer,1024);
    //         }else{
    //             printf("Error: <file name> does not have block <block_num>");
    //         }
    //     }
    // }
    // printf("Error: File <file name> does not exist");
    return;
}
void fs_write(char name[5], int block_num){
    cout << "inside fs write, filename: "<< name << ",   block number: " << block_num << endl;
    return;
}


void fs_buff(uint8_t buff[1024]){
    cout << "inside fs buff, buff before: " << (char *)buff << endl;
    memset(buffer, 0, 1024);
    memcpy(buffer, buff, 1024);
    cout << "inside fs buff, buffer after: " << (char *)buffer << endl;
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
    if(strcmp(name, ".") == 0){                                        // do nothing
        return;
    }
    
    if(strcmp(name, "..") == 0){                                       // got one directory back of the current directory only 
        if(currentDir != 127){                                                    // if the current directory is not root
            uint8_t tempIndex = superBlock.inode[currentDir].dir_parent;
            currentDir = tempIndex << 1;
            currentDir = currentDir >> 1;
            return;
        }
            
    }
    int index = -1;                                                             // if an actual dir name is given then go into that dir
    for (int i = 0;i<126; i++){
        if(strncmp(name,superBlock.inode[i].name,5)){
            uint8_t tempIndex = superBlock.inode[i].dir_parent;
            int isDir = tempIndex >> 7;
            tempIndex = tempIndex << 1;
            tempIndex = tempIndex >> 1;
            if((isDir == 1) && (tempIndex == currentDir)){
                index = i;
            }
        }
    }
    if(index >= 0){
        currentDir = index;
    }
    else{
        cout << "Error: Directory" << name << " does not exist" << endl;
    }
    return;
}

void changeDisk(){
    infile.seekp(0, ios::beg);
    infile.write(superBlock.free_block_list, 16);

    for(int i =0;i < 126; i++){
        infile.write(superBlock.inode[i].name, 5);
        infile.write((char *)& superBlock.inode[i].used_size, 1);
        infile.write((char *)& superBlock.inode[i].start_block,1);
        infile.write((char *)& superBlock.inode[i].dir_parent,1);
        // cout << "name " << (char *)superBlock.inode[i].name << endl;
        // cout << "used_size " << unsigned(superBlock.inode[i].used_size) << endl;        
        // cout << "start Block " << unsigned(superBlock.inode[i].start_block) << endl;
        // cout << "dirparent " << unsigned(superBlock.inode[i].dir_parent) << endl;
    }
    
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
    if (whichBit > 0 && whichBit <= 8){
        if( (byteFlag & (1<<(whichBit-1))) != 0){
            return 1;
        }
    }
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


bool fs_mountCheck(char *new_disk_name){
    fstream infile;
    infile.open(new_disk_name);
    if(infile.fail()){
        cout << "Error: Cannot find disk " << new_disk_name << endl;
    }
    Super_block *S1 = new Super_block;
    infile.read(S1->free_block_list,16);
    for(uint8_t i =0; i<126; i++){
        infile.read((char*)&(S1->inode[i].name),5);
        infile.read((char*)&(S1->inode[i].used_size),1);
        infile.read((char*)&(S1->inode[i].start_block),1);
        infile.read((char*)&(S1->inode[i].dir_parent),1);
    }

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

    for(int i=0;i<126; i++){                                                                // going through all the inodes and get their startBlocks and sizes which
        uint8_t startBlock = S1->inode[i].start_block;                                      // gives us the indexs in our free space list and then we check those bits
        uint8_t inUse_byte = S1->inode[i].used_size;                                        // in our free space list as it should be the only one's with 1's. if this check
        int inUse_bit = findBit(inUse_byte, 8);                                             // passes then we remove them from our (position , value) map. So all the positions 
        if(inUse_bit == 1){                                                                 // in our map should have their bits only as 0's. so we iterate through to do the check.
            uint8_t size = inUse_byte << 1;
            size = size >> 1;
            for(int j=0; j < size; j++ ){
                if((startBlock+j) > 127){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 4)" << endl;
                    return false;
                }
                int pos_bit = pos_map.find(startBlock + j)->second;
                if(pos_bit == 1){
                    pos_map.erase(startBlock + j);
                }
                if(pos_bit == 0){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 1)" << endl;
                    return false;
                }
            }
        }
    }

    pos_map.erase(0);                                                                           // erasing the initial bit in the free space as it will alsways be 1
    for(auto it = pos_map.begin(); it != pos_map.end(); it++){
        if(it->second == 1){
            cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 1)" << endl;
            return false;
        }
    }
    
    // consistency check 2: The name of every file/directory must be unique in each directory.

   // creating a multimap of file/dir names and index of their parent node
    multimap<string,uint8_t> name_dirMap;
    for (int i = 0; i < 126 ;i++){
        Inode temp = S1->inode[i];
        uint8_t inUse_byte = temp.used_size;
        int inUse_bit = findBit(inUse_byte, 8);
        if (inUse_bit == 1){
            char node_name[5];
            strncpy(node_name,S1->inode[i].name,5);
            uint8_t dir_index = S1->inode[i].dir_parent;
            dir_index = dir_index <<1;
            dir_index = dir_index >>1;

            auto ret = name_dirMap.equal_range(node_name);
            for(auto it = ret.first; it != ret.second; ++it){
                if(it->second == dir_index){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 2)" << endl;
                    return false;
                }
            }
            name_dirMap.insert(pair<string,uint8_t>(node_name, dir_index));
        }
    }





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
                    return false;  
                }
            }
            if((temp.dir_parent != 0) || (temp.start_block != 0) || (temp.used_size != 0)){         // check for other shit
                cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
                return false;
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
                    return false;
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
                    return false;
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
                cout << "is directory inode:" << i << endl;
                uint8_t size = inUse_byte << 1;
                size = size >> 1;
                if(size != 0 || temp.start_block != 0){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 5)" << endl;
                    return false;
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
            parent_byte = parent_byte << 1;
            parent_byte = parent_byte >> 1;
            if(parent_byte == 126){
                cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 6)" << endl;
                return false;
            }
            if(parent_byte >= 0 && parent_byte <= 125){
                uint8_t inUse_byte2 = S1->inode[parent_byte].used_size;
                int inUse_bit2 = findBit(inUse_byte2, 8);
                uint8_t isDir_byte = S1->inode[parent_byte].dir_parent;
                int isDir_bit = findBit(isDir_byte, 8);
                if(inUse_bit2 != 1 || isDir_bit != 1){
                    cout << "Error: File system in " << new_disk_name << " is inconsistent (error code: 6)" << endl;
                    return false;
                }

            }

        }
    }
    return true;

}

int findBlock(int requiredSize){
    int freeIndex = 0;
    int foundindex = -1;
    for (int i = 0; i < 16; i++){                                                           // creating the map of position value pairs
        uint8_t tempByte = superBlock.free_block_list[i];
        uint8_t mask = 0x80U;
        for(int j=0; j < 8; j++){
            if(mask & tempByte){
                freeIndex = 0;
            }
            else{
                freeIndex++;
            }
            if(freeIndex >= requiredSize){
                foundindex = (i*8)+j;
                foundindex -= requiredSize;
                return foundindex;
            }
            mask = mask >> 1;
        }  
    }
    return foundindex;

}