#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


struct inode{
  char name[16];
  int size;
  int blockPointers[8];
  int used;
};

class myFileSystem{ // class thats holds all the functions for controlling the file system within
public:


  void fileSystem(char diskName[16]){

    int f_write = open(diskName, O_RDWR); // f_write opens file and returns -1 if file not opened correctly

    if(f_write == -1){ // if file not opened correctly
      std::cout << "Error opening file" << std::endl;
      exit(1);
    }

    init = f_write; // assign position of f_write (1st byte) to private int pos
  }


  int create(char name[16], int size){
    int open_space = 0; // count for number of open slots
    int blockList[8];
    char open_slot;
    lseek(init, 0, SEEK_SET);


    for(int i = 0; i < 128; i++){ // for loop that checks number of open blocks to see if space available
      read(init, &open_slot, 1);

      if(open_slot != '1'){
        blockList[open_space] = i;
        open_space++;
      }

      if(open_space == size){ // dont need to check rest of blocks, already found enough space
        break;
      }
    }

    if(open_space < size){ // means less blocks open than new file requires
      std::cout << "Not enough space for a new file" << std::endl;
      return 1;
    }

    // Now checking for free inodes
    lseek(init, 128, SEEK_SET); // move pointer to beginning of where inodes are
    int numSkips = 0;


    while(true){
    char filename[16]; //inode tasks to be read in
    char nodeSize[4];
    char blockP[32];
    char nodeUse[4];
    char blockWrite[4];
    int isUse = 1;
    char inUse[4];
    snprintf(inUse, sizeof(inUse), "%d", isUse);
    inUse[3] = char(isUse);
    char file[4];
    char takenBlock = '1';


    read(init, filename, 16); // reads in filename, size of file, blocks used, and if node is in use
    read(init, nodeSize, 4);
    for(int i = 0; i < 8; i++){
      read(init, blockP, 4);
    }
    read(init, nodeUse, 4);
    int compare = 0;
    sscanf(nodeUse, "%d", &compare);

    if(compare == 0){
      std::cout << "Found Free iNode to Create in" << std::endl;
      lseek(init, 128 + (56 * numSkips), SEEK_SET);
      write(init, name, 16);
      snprintf(file, sizeof(file), "%d", size);
      write(init, file, 4);
      for(int i = 0; i < size; i++){
        snprintf(blockWrite, sizeof(blockWrite), "%d", blockList[i]);
        write(init, blockWrite, 4);
      }
      int advance = 4 * (8 - size);
      lseek(init, advance, SEEK_CUR);
      write(init, &inUse, 4);

      for(int i = 0; i < size; i++){
        lseek(init, blockList[i], SEEK_SET); // marking blocks as taken here using loop that goes to index as offset and writes a char 1 to it
        write(init, &takenBlock, 1);
      }

      break;
    }
    else{
      numSkips++;
    }
    }



    return 1;

  }


  int deleteFile(char name[16]){
    lseek(init, 128, SEEK_SET); // move file descriptor to beginning of inodes
    int matchingBlocks[8]; // blocks that are pointed to by file
    int foundFileSize;
    bool wasFound = false;
    char freeBlock = '0';

    int replace = 0;
    char charReplace[4];
    snprintf(charReplace, sizeof(charReplace), "%d", replace);

    for(int i = 0; i < 16; i++){
      char currentName[16];
      read(init, currentName, 16);
      if(strcmp(name, currentName) == 0){ // means matching file was found
        char readNum[4];
        wasFound = true;
        read(init, readNum, 4);
        sscanf(readNum, "%d", &foundFileSize);
        for(int j = 0; j < foundFileSize; j++){
          char clearBlock[4];
          int fillSlot;
          read(init, clearBlock, 4); // reading in the number of the block that is being taken up
          sscanf(clearBlock, "%d", &fillSlot);
          matchingBlocks[j] = fillSlot;
        }
        lseek(init, 4 * (8 - foundFileSize), SEEK_CUR);
        write(init, charReplace, 4);
        break;
      }
      else{
        lseek(init, 40, SEEK_CUR);
      }
    }

    if(wasFound){
      std::cout << "File found to be deleted" << std::endl;
      for(int i = 0; i < foundFileSize; i++){
        lseek(init, matchingBlocks[i], SEEK_SET);
        write(init, &freeBlock, 1);
      }
      std::cout << "File was successfully deleted" << std::endl;
      return 1;
    }
    else{
      std::cout << "File could not be found" << std::endl;
      return -1;
    }

  }


  void ls(){
    lseek(init, 128, SEEK_SET);
    char filename[16];
    char size[4];
    char blocks[32];
    char open[4];
    int isOpen;

    for(int i = 0; i < 16; i++){
      read(init, filename, 16);
      read(init, size, 4);
      read(init, blocks, 32);
      read(init, open, 4);
      sscanf(open, "%d", &isOpen);
      if(isOpen == 1){ // means there is file and space is taken
        std::cout << "Filename: " << filename << " | " << "Size: " << size << std::endl;
    }
      isOpen = 0;
  }
    std::cout << std::endl;
}


  int writeFile(char filename[16], int blockNum, char buf[1024]){
    lseek(init, 128, SEEK_SET); // seek to beginning of inodes

    for(int i = 0; i < 16; i++){ // check all the inodes for the file to be written to
      char file[16];
      char fileSize[4];
      char blockWrite[4];
      int fs;
      int offset;

      read(init, file, 16);
      if(strcmp(filename, file) == 0){ // means file was found
        read(init, fileSize, 4);
        sscanf(fileSize, "%d", &fs);

        if(fs < blockNum){
          return -1; // trying to write past file
        }

        lseek(init, 4 * blockNum, SEEK_CUR);
        read(init, blockWrite, 4);
        sscanf(blockWrite, "%d", &offset);

        std::cout << "Accessing block to write in: " << std::endl;
        lseek(init, 1024 + 1024 * offset, SEEK_SET);
        write(init, buf, 1024);
        std::cout << "Write Successful" << std::endl << std::endl;
        return 1;
      }
      else{
        lseek(init, 40, SEEK_CUR);
      }
    }
    std::cout << "File could not be found" << std::endl;
    return -1;
  }


    
    int readFile(char filename[16], int blockNum){
      char readInfo[1024];
      lseek(init, 128, SEEK_SET); // seek to beginning of inodes

      for(int i = 0; i < 16; i++){ // check all the inodes for the file to be written to
        char file[16];
        char fileSize[4];
        char blockWrite[4];
        int fs;
        int offset;

        read(init, file, 16);
        if(strcmp(filename, file) == 0){ // means file was found
          read(init, fileSize, 4);
          sscanf(fileSize, "%d", &fs);

          if(fs < blockNum){
            return -1; // trying to write past file
          }

          lseek(init, 4 * blockNum, SEEK_CUR);
          read(init, blockWrite, 4);
          sscanf(blockWrite, "%d", &offset);

          lseek(init, 1024 + 1024 * offset, SEEK_SET);
          read(init, readInfo, 1024);
          std::cout << "Block accessed for reading. Contents of block: " << readInfo <<  std::endl << std::endl;
          return 1;
        }
        else{
          lseek(init, 40, SEEK_CUR);
        }
      }
      std::cout << "File could not be found" << std::endl;
      return -1;
    }
    

  void checkFile(){
    lseek(init, 128, SEEK_SET);
    char output[56];
    read(init, output, 56);
    std::cout << output << std::endl;
    for(int i = 0; i < 56; i++){
      std::cout << output[i] << std::endl;
    }
  }



private:
  int init; // current position of disk pointer

};

int main(){
  char dummyWrite[1024]; // dummy buffer for writing series of 1's when write command is passed
  char systemName[16];
  char filename[16];
  char mode;
  int val;
  int errorCheck;

  std::ifstream infile("sample-test.txt");

  if(infile.fail()){
    std::cout << "Error opening file" << std::endl;
    exit(1);
  }

  for(int i = 0; i < 1024; i++){
    dummyWrite[i] = '1';
  }

  infile >> systemName;
  myFileSystem myFiles;
  myFiles.fileSystem(systemName);
  while(!infile.eof()){
    infile >> mode;
    if(mode != 'L'){
      infile >> filename;
      if(mode != 'D'){
        infile >> val;
      }
    }

    std::cout << "Next operation: " << mode << std::endl << std::endl;

    switch(mode){
      case 'C':
        std::cout << "Making file: " << filename << " of size: " << val << std::endl;
        errorCheck = myFiles.create(filename, val);
        if(errorCheck == -1)
          std::cout << "Could not complete task" << std::endl;
        break;
      case 'D':
        std::cout << "Deleting file: " << filename << std::endl;
        errorCheck = myFiles.deleteFile(filename);
        if(errorCheck == -1)
          std::cout << "Could not complete task" << std::endl;
        break;
      case 'W':
        std::cout << "Writing to file: " << filename << " Block: " << val << std::endl;
        errorCheck = myFiles.writeFile(filename, val, dummyWrite);
        if(errorCheck == -1)
          std::cout << "Could not complete task" << std::endl;
        break;
      case 'R':
        std::cout << "Reading from file: " << filename << " Block: " << val << std::endl;
        errorCheck = myFiles.readFile(filename, val);
        if(errorCheck == -1)
          std::cout << "Could not complete task" << std::endl;
        break;
      case 'L':
        std::cout << "Listing Files" << std::endl;
        myFiles.ls();
        break;
      default:
        std::cout << "Invalid Operation" << std::endl;
    }

  }

  infile.close();
  return 0;
}
