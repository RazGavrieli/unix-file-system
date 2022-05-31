#include <stdlib.h>

#define BLOCK_SIZE 512
#define NAME_SIZE 8

struct super_block {
    int inodes;
    int blocks;
    // int size_blocks; = BLOCK_SIZE?

};


struct disk_block {
    int next;
    char data[BLOCK_SIZE];

};


struct inode {
    int next;
    char name[NAME_SIZE];
};


typedef struct myDIR{ // == FILE for directories

}myDIR;


struct mydirent { // 

};

// creates a new fs with the given size
void mymkfs(int); 
// writes the fs that is currently on the memory into an actual file
void sync(); 
// prints the fs from the disk (from a given file name)
void printfs_dsk(char*); 
// printfs the fs from the memory
void printfs_mem(); 
// allocates a new file with a given size and name (allocates inode and blocks)
int allocate_file(int , char*); 
// write only ONE char into the block
void writebyte(int , int, char); 
// print a file from our fs
void printfd(int); 



// wtf: 
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);
int myopen(const char *pathname, int flags);
int myclose(int myfd);
size_t myread(int myfd, void *buf, size_t count);
size_t mywrite(int myfd, const void *buf, size_t count);
off_t mylseek(int myfd, off_t offset, int whence);

myDIR *myopendir(const char *name);
struct mydirent *myreaddir(myDIR *dirp);
int myclosedir(myDIR *dirp);