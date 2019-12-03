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
    string inter;
    vector<string> string_input_list;

    getline(cin, input);

    string_input_list = tokenize(input, " ");
    if(string_input_list[0] == "M" && string_input_list.size() > 1){
        char *convert = new char[string_input_list[1].size() + 1];
        copy(string_input_list[1].begin(),string_input_list[1].end(),convert);
        convert[string_input_list[1].size()] = '\0';
        fs_mount(convert);
        delete[] convert;
    }

    return 0;
}


void fs_mount(char *new_disk_name){
    cout<<"Disk name: "<<new_disk_name<<endl;

    FILE *fp = fopen(new_disk_name, "r");
    if(fp < 0){
        cout << "Cannot find disk " << new_disk_name << endl;
        return;
    }
    Super_block superBlock;
    ssize_t superBlock_size = 1024;
    fp.read(&superBlock, 1024);

    int inodes[126] = {};
    int inode_index = 0;
    for (int i=17; i <= 1024; i+=8){
        inodes[inode_index] = 1;
        inode_index++;
        cout<< inodes[inode_index] <<"       count: "<< inode_index << "        i value "<< i << endl;
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
