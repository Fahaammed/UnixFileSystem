#include "FileSystem.h"


using namespace std;

Super_block superBlock;
string diskName;
bool diskSet = false;
uint8_t buffer[1024];
uint8_t currentDir;
fstream infile;


int main(int argc, char** argv){
    string filename = argv[1];
    fstream fp(filename);
    string input;
    int whileloop = 0;
    diskSet = false;
    while(getline(fp,input)){
        whileloop++;
        vector<string> string_input_list;
        string_input_list = tokenize(input, " ");
    
        
    // mounting a disk if the conditions meet
        if(string_input_list[0] == "M" && string_input_list.size() == 2){
            char *convert = new char[string_input_list[1].size() + 1];
            copy(string_input_list[1].begin(),string_input_list[1].end(),convert);
            convert[string_input_list[1].size()] = '\0';
            if(fs_mountCheck(convert)){                                                                                 // Do the consistency check for the mount
                diskSet = true;                                                                                         // if the consistency passed then set disk
                diskName = convert;
                fs_mount(convert);                                                                                      // call mount
            }
            delete[] convert;
        }


        else if(string_input_list[0] == "C" && string_input_list.size() == 3){                                          // if matches with create and the conditions meet call fs_create
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true && diskSet){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int size = stoi(string_input_list[2]);
                fs_create(name, size);
                // delete[] name;
            }
            

        }


        else if(string_input_list[0] == "D" && string_input_list.size() == 2){                                          // if matches with delete and the conditions meet call fs_delete
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                fs_delete(name);
                // delete[] name;
            }
        }

        else if(string_input_list[0] == "R" && string_input_list.size() == 3){                                          // if matches with read and the conditions meet call fs_read
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int blockNum = stoi(string_input_list[2]);
                fs_read(name, blockNum);
                // delete[] name;
            }
        }

        else if(string_input_list[0] == "W" && string_input_list.size() == 3){                                          // if matches with write and the conditions meet call fs_write
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int blockNum = stoi(string_input_list[2]);
                fs_write(name, blockNum);
                // delete[] name;
            }
            
        }

        else if(string_input_list[0] == "B" && string_input_list.size() > 1){                                           // if matches with buff and the conditions meet call fs_buff
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            else{
                uint8_t buff[1024];
                memset(buff, 0, 1024);
                copy(input.begin()+input.find("B")+2,input.end(),buff);
                fs_buff(buff);
            }
        }

        else if(string_input_list[0] == "L" && string_input_list.size() == 1){                                          // if matches with ls and the conditions meet call fs_ls
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            else{
                fs_ls();
            }
        }

        else if(string_input_list[0] == "E" && string_input_list.size() == 3){                                          // if matches with resize and the conditions meet call fs_resize
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str() );
                int newSize = stoi(string_input_list[2]);
                fs_resize(name, newSize);
                // delete[] name;
            }
        }

        else if(string_input_list[0] == "O" && string_input_list.size() == 1){                                          // if matches with defrag and the conditions meet call fs_defrag
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            else{
                fs_defrag();
            }
        }

        else if(string_input_list[0] == "Y" && string_input_list.size() == 2){                                          // if matches with cd and the conditions meet call fs_cd
            if(diskSet == false){
                cerr << "Error: No file system is mounted" << endl;
            }
            if(string_input_list[1].size() < 6 && diskSet == true){
                char name[5];
                strcpy(name,string_input_list[1].c_str());
                fs_cd(name);
                // delete[] name;
            }
        }

        else{
            cerr << "Command Error: " << filename <<" ,"<< whileloop << endl;                                           // otherwise command is wrong
        }
    }
    return 0;
}


void fs_mount(char *new_disk_name){                                                                                     // mounts the disk to the superBlock structure
    cerr <<"Disk name: "<< new_disk_name << endl;
    
    infile.open(new_disk_name);
    if(infile.fail()){
        cerr << "infile error" << endl;
    }

    // superBlock = new Super_block;
    infile.read(superBlock.free_block_list,16);
    for(uint8_t i =0; i<126; i++){
        infile.read(superBlock.inode[i].name,5);
        infile.read((char*)&superBlock.inode[i].used_size,1);
        infile.read((char*)&superBlock.inode[i].start_block,1);
        infile.read((char*)&superBlock.inode[i].dir_parent,1);
    }
    currentDir = 127;                                                                                                   // set the current directory to 127 which is root
    return;
}



void fs_create(char name[5], int size){
    for(int i =0; i < 126; i++){                                                                                        // go through all the inodes and if the name given matches and is in the same directory
        uint8_t parentDir = superBlock.inode[i].dir_parent;                                                             // then throw the error and return
        parentDir = parentDir << 1;
        parentDir = parentDir >> 1;
        if(currentDir == parentDir){ 
            string inodeName = superBlock.inode[i].name;
            inodeName = inodeName.substr(0,5);
            if(!inodeName.compare(name)){
                cerr<< "Error: File or directory "<< name << " already exists" << endl;
                return;
            }        
        }    
    }
    
    int inodeIndex = -1;                                                                                                // find an unused inode and get it's index
    for(int i =0; i<126; i++){
        if(superBlock.inode[i].used_size == 0){
            inodeIndex = i;
            break;
        }
    }
    if(inodeIndex == -1){                                                                                               // if no inode is free then throw an error
        cerr << "Error: Superblock in disk "<< diskName << " is full, cannot create "<< name << endl;
        return;
    }


    if(size == 0){                                                                                                       // we need to make a directory
        
        strncpy( superBlock.inode[inodeIndex].name, name, 5);
        superBlock.inode[inodeIndex].used_size |= (uint8_t)0x80U; // in use = 1
        superBlock.inode[inodeIndex].dir_parent |= (uint8_t)0x80U; // is dir = 1
        superBlock.inode[inodeIndex].dir_parent |= currentDir;                                                            // call changeDisk to write to the actual disk
        changeDisk();
    }
    else if(size > 0 && size < 128){                                                                                      // if it is a file find the index of contaguous blocks
        int freeListIndex = findBlock(size);
        if(freeListIndex < 0){
            cerr << "Error: Cannot allocate "<< size << " on " << diskName << endl;                                        // if not enough space left then throw an error 
        return;
        }
        for(int i = 0; i < 5; i++){                                                                                         // write the name to the superBlock
            superBlock.inode[inodeIndex].name[i] = name[i];
            cerr << superBlock.inode[inodeIndex].name[i]<< endl;
        }
        superBlock.inode[inodeIndex].used_size = (uint8_t)size;                                                          // set the size
        superBlock.inode[inodeIndex].used_size |= (uint8_t)0x80U;                                                       // set in use = 1
        superBlock.inode[inodeIndex].dir_parent = currentDir;                                                            // set the parent directory   
        superBlock.inode[inodeIndex].start_block = (uint8_t)freeListIndex+1;                                            // set the start block
        for(int i = freeListIndex+1; i < freeListIndex+1+size; i++){                                                    // flip the bits in the free_space list in the superBlock according to the index and size
            int byte_index = i / 8;
            int bit = i % 8;
            uint8_t byte = superBlock.free_block_list[byte_index];
            uint8_t mask_shift = 7 - bit;
            uint8_t mask = 1 << mask_shift;
            uint8_t new_byte = byte | mask;
            superBlock.free_block_list[byte_index] = new_byte;
        }
        
    changeDisk();                                                                                                        // write the changes to the disk
    }
    return;
}


void fs_delete(char name[5]){
    int index = -1;                                                                                             
    for(int i =0; i < 126; i++){                                                                                            // go through all the inodes and file the inode with this name and same parent directory
        uint8_t parentDir = superBlock.inode[i].dir_parent;
        parentDir = parentDir << 1;
        parentDir = parentDir >> 1;
        if(currentDir == parentDir){ 
            string inodeName = superBlock.inode[i].name;
            inodeName = inodeName.substr(0,5);
            if(!inodeName.compare(name)){
                index = i;
                break;
            }        
        }    
    }

    if(index < 0){                                                                                                          // if not found throw an error
        cerr << "Error: File or directory " << name << " does not exist" << endl;
    }

    uint8_t isDir = superBlock.inode[index].dir_parent;                                                                     // check to see if it is a directory
    isDir = isDir >> 7;
    
    if(isDir == 1){                                                                                                         // if it is a directory then recursively call fs_delete on it's children
        vector<int> childNodes = fileDirectory(index);
        for(uint8_t i = 0; i < childNodes.size(); i++){
            fs_delete(superBlock.inode[childNodes[i]].name);
        }
    }
    deleteInode(index);                                                                                                     // call deleteInode to clear out the inode and associated things
    return;
}


void fs_read(char name[5], int block_num){
    for(uint8_t i =0; i < 126; i++){
        uint8_t parentDir = superBlock.inode[i].dir_parent;
        parentDir = parentDir << 1;                             
        parentDir = parentDir >> 1;
        if(currentDir == parentDir){ 
            string inodeName = superBlock.inode[i].name;
            inodeName = inodeName.substr(0,5);
            uint8_t inUse = superBlock.inode[i].used_size;
            inUse = inUse >> 7;
            if(!inodeName.compare(name)){                                                                                   // finds the inode with the same name and same directory
                uint8_t size = superBlock.inode[i].used_size;
                size = size << 1;
                size = size >> 1;
                if(block_num > size){                                                                                       // if block num is greater than size throw error
                    cerr << "Error: "<< name <<" does not have block " << block_num << endl;
                    return;
                }
                if(inUse != 0){                                                                                             // if file not found
                    cerr << "Error: File " << name << " does not exist" << endl;    
                    return;
                }
                uint8_t startBlock = superBlock.inode[i].start_block;
                int readBlock = startBlock + block_num;                                                                     // if all conditions met then read from the disk and copy it into buffer 
                if(readBlock < 129){
                    infile.seekg(readBlock*1024,ios::beg);
                    char tempbuff[1024];
                    memset(tempbuff,0,1024);
                    infile.read(tempbuff,1024);
                    memset(buffer,0,1024);
                    memcpy(buffer, tempbuff,1024);
                }
            }
            else {
                cerr << "Error: File " << name << " does not exist" << endl;
                return;
            }        
        }    
    }
    return;
}

void fs_write(char name[5], int block_num){
    for(int i =0; i < 126; i++){
        uint8_t parentDir = superBlock.inode[i].dir_parent;                                                             // perform same checks as fs_read
        parentDir = parentDir << 1;
        parentDir = parentDir >> 1;
        if(currentDir == parentDir){ 
            string inodeName = superBlock.inode[i].name;
            inodeName = inodeName.substr(0,5);
            uint8_t inUse = superBlock.inode[i].used_size;
            inUse = inUse >> 7;
            if(!inodeName.compare(name)){
                uint8_t size = superBlock.inode[i].used_size;
                size = size << 1;
                size = size >> 1;
                if(block_num > size){
                    cerr << "Error: "<< name <<" does not have block " << block_num << endl;
                    return;
                }
                if(inUse != 0){
                    cerr << "Error: File " << name << " does not exist" << endl;    
                    return;
                }
                uint8_t startBlock = superBlock.inode[i].start_block;
                int writeBlock = startBlock + block_num;
                if(writeBlock < 129){                                                                                   // if all checks passes then copy from the buffer and write from the disk
                    infile.seekp(writeBlock*1024,ios::beg);
                    char tempbuff[1024];
                    memset(tempbuff,0,1024);
                    memcpy(tempbuff,buffer,1024);
                    infile.write(tempbuff,1024);
                }
            }
            else {
                cerr << "Error: File " << name << " does not exist" << endl;
                return;
            }        
        }    
    }
    return;
    
}


void fs_buff(uint8_t buff[1024]){                                                                                                   // clear current buffer and write on it
    memset(buffer, 0, 1024);
    memcpy(buffer, buff, 1024);
    return;
}


void fs_ls(void){
    vector<int> numberOfChildren = fileDirectory(currentDir);
    printf("%-5s %3d \n", ".",(int) numberOfChildren.size());

    uint8_t parentDir = superBlock.inode[currentDir].dir_parent;
    parentDir = parentDir << 1;
    parentDir = parentDir >> 1;
    vector<int> parentsFile = fileDirectory(parentDir);
    if(parentDir == 127){
        printf("%-5s %3d \n", ".",(int) numberOfChildren.size());
    }
    else{
        printf("%-5s %3d \n", "..",(int) numberOfChildren.size());
    }
    
    for (int i=0; i< 126; i++){
        uint8_t parentDir2 = superBlock.inode[i].dir_parent;
        parentDir2 = parentDir2 << 1;
        parentDir2 = parentDir2 >> 1;
        if(parentDir2 == currentDir){
            if(superBlock.inode[i].start_block == 0){
                vector<int> dirCount = fileDirectory(parentDir2);
                printf("%-5s %3d \n", superBlock.inode[i].name, (int)dirCount.size());
            }
            else{
                printf("%-5s %3d \n", superBlock.inode[i].name, superBlock.inode[i].used_size & 0x7f);
            }
        }

    }

    return;
}

void fs_resize(char name[5], int new_size){
    //cout << "inside fs resize, filename: " << name <<  ",    new size: " << new_size << endl;
    return;
}

void fs_defrag(void){
    //cout << "inside fs defrag" << endl;
    return;
}

void fs_cd(char name[5]){
    if(strcmp(name, ".") == 0){                                                                                                         // do nothing
        return;
    }
    
    if(strcmp(name, "..") == 0){                                                                                                        // got one directory back of the current directory only 
        if(currentDir != 127){                                                                                                          // if the current directory is not root
            uint8_t tempIndex = superBlock.inode[currentDir].dir_parent;
            currentDir = tempIndex << 1;
            currentDir = currentDir >> 1;
            return;
        }
            
    }
    int index = -1;                                                                                                                     // if an actual dir name is given then go into that dir
    for (int i = 0;i<126; i++){
        string inodeName = superBlock.inode[i].name;
        inodeName = inodeName.substr(0,5);
        if(!(inodeName.compare(name))){
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
        cerr << "Error: Directory" << name << " does not exist" << endl;
    }
    return;
}


/**
 * @brief write the whole superblock to the disk
 * 
  */
void changeDisk(){                                                                                                                     
    infile.seekp(0, ios::beg);
    infile.write(superBlock.free_block_list, 16);

    for(int i =0;i < 126; i++){
        infile.write(superBlock.inode[i].name, 5);
        infile.write((char *)& superBlock.inode[i].used_size, 1);
        infile.write((char *)& superBlock.inode[i].start_block,1);
        infile.write((char *)& superBlock.inode[i].dir_parent,1);
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


/**
 * @brief find the value of the specific bit in a byte 
 * 
 * @param uint8_t - The byte to check
 * @param int - the index of the bit to check
 * @return int 1/0
 */

int findBit(uint8_t byteFlag, int whichBit)
{
    if (whichBit > 0 && whichBit <= 8){
        if( (byteFlag & (1<<(whichBit-1))) != 0){
            return 1;
        }
    }
   return 0;
}

/**
 * @brief converts a char array to a string 
 * 
 * @param char* - The array
 * @param int - the size of the array
 * @return string - the new string
 */


string convertToString(char* a, int size) 
{ 
    int i; 
    string s = ""; 
    for (i = 0; i < size; i++) { 
        s = s + a[i]; 
    } 
    return s; 
} 

/**
 * @brief does the consistency checks for fs mount
 *          makes a temporary superblock and performs the checks on it
 * 
 * @param char* - The disk name
 * @return bool - true if the tests pass or false
 */

bool fs_mountCheck(char *new_disk_name){
    fstream infile2;
    infile2.open(new_disk_name);
    if(infile2.fail()){
        cerr << "Error: Cannot find disk " << new_disk_name << endl;
    }
    Super_block *S1 = new Super_block;
    infile2.read(S1->free_block_list,16);
    for(uint8_t i =0; i<126; i++){
        infile2.read((char*)&(S1->inode[i].name),5);
        infile2.read((char*)&(S1->inode[i].used_size),1);
        infile2.read((char*)&(S1->inode[i].start_block),1);
        infile2.read((char*)&(S1->inode[i].dir_parent),1);
    }
    infile2.close();


    // consistency check 1: Blocks that are marked free in the free-space list cannot be allocated to any file. Similarly, blocks
    // marked in use in the free-space list must be allocated to exactly one file.
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
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 4)" << endl;
                    return false;
                }
                int pos_bit = pos_map.find(startBlock + j)->second;
                if(pos_bit == 1){
                    pos_map.erase(startBlock + j);
                }
                if(pos_bit == 0){
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 1)" << endl;
                    return false;
                }
            }
        }
    }

    pos_map.erase(0);                                                                           // erasing the initial bit in the free space as it will alsways be 1
    for(auto it = pos_map.begin(); it != pos_map.end(); it++){
        if(it->second == 1){
            cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 1)" << endl;
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
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 2)" << endl;
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
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
                    return false;  
                }
            }
            if((temp.dir_parent != 0) || (temp.start_block != 0) || (temp.used_size != 0)){         // check for other shit
                cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
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
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 3)" << endl;
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
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 4)" << endl;
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
                uint8_t size = inUse_byte << 1;
                size = size >> 1;
                if(size != 0 || temp.start_block != 0){
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 5)" << endl;
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
                cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 6)" << endl;
                return false;
            }
            if(parent_byte >= 0 && parent_byte <= 125){
                uint8_t inUse_byte2 = S1->inode[parent_byte].used_size;
                int inUse_bit2 = findBit(inUse_byte2, 8);
                uint8_t isDir_byte = S1->inode[parent_byte].dir_parent;
                int isDir_bit = findBit(isDir_byte, 8);
                if(inUse_bit2 != 1 || isDir_bit != 1){
                    cerr << "Error: File system in " << new_disk_name << " is inconsistent (error code: 6)" << endl;
                    return false;
                }

            }

        }
    }
    return true;

}


/**
 * @brief finds the contaguious blocks in free disk space
 * 
 * @param int - The given size
 * @return int - the index of the starting of the contagiuos block
 */


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
                if(i == 0){
                    return j-requiredSize;
                }
                else {
                    foundindex = (i*8)+j;
                    foundindex -= requiredSize;
                    return foundindex;    
                }
            }
            mask = mask >> 1;
        }  
    }
    return foundindex;

}


/**
 * @brief create a vector map of child inodes of the given directory
 * 
 * @param int - The index of the directory
 * @return int - the index of the starting of the contagiuos block
 */


vector<int> fileDirectory(int directory){
    vector<int> nodesDir;
    for (int i=0; i < 126; i++){
        uint8_t parent = superBlock.inode[i].dir_parent;
        parent = parent << 1;
        parent = parent >> 1;
        if(parent == directory){
            nodesDir.push_back(i);
        }
    }
    return nodesDir;
}


/**
 * @brief delete everything associates with the inode
 * 
 * @param int - The index of the inode
 */

void deleteInode(int index){
    uint8_t isDir = superBlock.inode[index].dir_parent;
    isDir = isDir >> 7;
    
    if(isDir == 1){
        superBlock.inode[index].dir_parent = 0;
        superBlock.inode[index].start_block = 0;
        superBlock.inode[index].used_size = 0;
        for (int i=0;i<5;i++){
            superBlock.inode[index].name[i] = 0;
        }
    }
    else {
        uint8_t size = superBlock.inode[index].used_size;
        size = size << 1;
        size = size >> 1;
        uint8_t startBlock = superBlock.inode[index].start_block;

        for(int i = startBlock; i < startBlock+size; i++){
            int byte_index = i / 8;
            int bit = i % 8;
            uint8_t byte = superBlock.free_block_list[byte_index];
            uint8_t mask_shift = 7 - bit;
            uint8_t mask = ~(1 << mask_shift);
            uint8_t new_byte = byte & mask;
            superBlock.free_block_list[byte_index] = new_byte;
        }

        superBlock.inode[index].dir_parent = 0;
        superBlock.inode[index].start_block = 0;
        superBlock.inode[index].used_size = 0;
        for (int i=0;i<5;i++){
            superBlock.inode[index].name[i] = 0;
        }
        for(int j=0; j< size;j++){
            int writeBlock = startBlock +j;
            infile.seekp(writeBlock*1024,ios::beg);
            char tempBuff[1024];
            memset(tempBuff,0,1024);
            infile.write(tempBuff,1024);
        }
        
    }
    changeDisk();
    return;
}