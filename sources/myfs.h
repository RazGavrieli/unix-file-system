#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BLOCK_SIZE 512
#define NAME_SIZE 8
#define MAX_DIR_SIZE 10
#define MAX_FILES 10000
#define O_CREAT 0100

#define SEEK_SET 0
#define SEEK_END 1
#define SEEK_CUR 2

struct myopenfile {
    int fd;
    int pos; 
};

struct super_block {
    int inodes;
    int blocks;
};


struct disk_block {
    int next;
    char data[BLOCK_SIZE];
};


struct inode {
    int next;
    char name[NAME_SIZE];
    int dir; // 0 for file 1 for dir
    int size;
};

struct mydirent { 
    int size;
    int fds[MAX_DIR_SIZE];
    char d_name[NAME_SIZE];
};

typedef struct myDIR { 
    int fd; 
}myDIR;

extern struct myopenfile openfiles[MAX_FILES];
extern struct super_block super_block;
extern struct inode *inodes;
extern struct disk_block *disk_blocks;

// creates a new fs with the given size
void mymkfs(int); 
// creates a root folder
void createroot();
// writes the fs that is currently on the memory into an actual file
void sync(const char*); 
// read an fs from a given file and load it into the memory
void resync(const char* );
// free the relevant memory
void destroyfs();
// prints the fs from the disk (from a given file name)
void printfs_dsk(char*); 
// printfs the fs from the memory
void printfs_mem(); 
// allocates a new file with a given size and name (allocates inode and blocks, return file fd)
int allocate_file(int , const char*); 
// write only ONE char into the block (fd, pos, byte of data)
void writebyte(int , int, char); 
// read only ONE char from the block (fd, pos) returns the char
char readbyte(int, int);
// print a single file from our fs with a given fd
void printfd(int); 
// print all the files and directories that are inside a given dir path
void printdir(const char*);
// creates a file at the given path with the given name, assign it one block
int mycreatefile(const char *path, const char* name);


// copy source ufs to target ufs, keep in mind that one of them can be NULL and the function will only load or sync.
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data); 
// finds the file from a given path, or creates it if it does not exists. Load the fd and a file pointer into the struct myopenfile list. 
int myopen(const char *pathname, int flags);
// removes the given fd from the myopenfile list. 
int myclose(int myfd);
// read a chunk of data the size of count from a given myfd.
size_t myread(int myfd, void *buf, size_t count);
// write a chunk of data the size of count from a given buf into myfd.
size_t mywrite(int myfd, const void *buf, size_t count);
// increment the file ptr by offset
int mylseek(int myfd, int offset, int whence);
// returns the FD(INSIDE STRUCT myDIR) of a given directory, if the dir does not exists at the path it will mymkdir and then return the new fd. 
myDIR* myopendir(const char *name); 
// creates a new directory at path with given name
int mymkdir(const char *path, const char* name);
// returns the struct mydirent of a given fd (checks first if the fd is of a directory)
struct mydirent *myreaddir(myDIR* dirp); 
// free dirp pointer memory.
int myclosedir(myDIR* dirp);  


        // there is going to be a reduction we
        // we now saw a redudction from CNF to 3-CNF
        // and maybe in the test there will be from CND to 4-CNF and it is different from 3-CNF