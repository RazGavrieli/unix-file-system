#include "myfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int virtual_disk_size;

struct super_block super_block;
struct inode *inodes;
struct disk_block *disk_blocks;

void mymkfs(int s) {
    virtual_disk_size = s;
    int size_without_superblock = s - sizeof(struct super_block);

    super_block.inodes = (size_without_superblock/10)/(sizeof(struct inode)); 
    super_block.blocks = (size_without_superblock-size_without_superblock/10)/(sizeof(struct disk_block));

    inodes = malloc(super_block.inodes*sizeof(struct inode));
    disk_blocks = malloc(super_block.blocks*sizeof(struct disk_block));


    for (size_t i = 0; i < super_block.inodes; i++)
    {
        strcpy(inodes[i].name, "");
        inodes[i].next = -1;
        inodes[i].dir = 0; 
    }

    for (size_t i = 0; i < super_block.blocks; i++)
    {
        strcpy(disk_blocks[i].data, "");
        disk_blocks[i].next = -1;
    }

    // ############### CREATE ROOT FOLDER ###############
    createroot();
}

void createroot() {
    int zerofd = allocate_file(sizeof(struct mydirent),  "root");
    if (zerofd != 0 ) {
        perror("root couldn't initialize at zero");
        exit(EXIT_FAILURE);
    }
    inodes[zerofd].dir = 1;
    struct mydirent* rootdir = malloc(sizeof(struct mydirent));
    for (size_t i = 0; i < MAX_DIR_SIZE; i++)
    {
        rootdir->fds[i] = -1;        
    }
    rootdir->size = 0;
    char* rootdiraschar = (char*)rootdir;
    for (size_t i = 0; i < sizeof(struct mydirent); i++)
    {
        writebyte(zerofd, i, rootdiraschar[i]);
    }
    free(rootdir);
}
void sync(const char* target) {
    FILE *file;
    file = fopen(target, "w+");
    fwrite(&super_block, sizeof(struct super_block), 1, file);
    fwrite(inodes, super_block.inodes*sizeof(struct inode), 1, file);
    fwrite(disk_blocks, super_block.blocks*sizeof(struct disk_block), 1, file);
    fclose(file);
}

void resync(const char* target) {
    FILE *file;
    file = fopen(target, "r");
    fread(&super_block, sizeof(super_block), 1, file);
    inodes = malloc(super_block.inodes*sizeof(struct inode));
    disk_blocks = malloc(super_block.blocks*sizeof(struct disk_block));
    fread(inodes, super_block.inodes*sizeof(struct inode), 1, file);
    fread(disk_blocks, super_block.blocks*sizeof(struct disk_block), 1, file);
    fclose(file);

}

void printfs_dsk(char* target) {
    FILE *file;
    file = fopen(target, "r");
    struct super_block temp_super_block;
    fread(&temp_super_block, sizeof(super_block), 1, file);
    struct inode *temp_inodes = malloc(temp_super_block.inodes*sizeof(struct inode));
    struct disk_block *temp_disk_blocks = malloc(temp_super_block.blocks*sizeof(struct disk_block));
    fread(temp_inodes, temp_super_block.inodes*sizeof(struct inode), 1, file);
    fread(temp_disk_blocks, temp_super_block.blocks*sizeof(struct disk_block), 1, file);
    printf("SUPERBLOCK\n");
    printf("\t inodes amount: %d\n\t blocks amount: %d\n", temp_super_block.inodes, temp_super_block.blocks);
    printf("\nINODES\n");
    for (size_t i = 0; i < temp_super_block.inodes; i++)
    {
        printf("%ld.\t name: %s | isdir: %d | next: %d\n",i , temp_inodes[i].name, temp_inodes[i].dir ,temp_inodes[i].next);
    }

    printf("\nBLOCKS\n");
    for (size_t i = 0; i < temp_super_block.blocks; i++)
    {
        printf("%ld.\t next: %d\n",i , temp_disk_blocks[i].next);
    }
    
    
    fclose(file);
    free(temp_disk_blocks); 
    free(temp_inodes);
}

void printfs_mem() {
    printf("SUPERBLOCK\n");
    printf("\t inodes amount: %d\n\t blocks amount: %d\n", super_block.inodes, super_block.blocks);
    printf("\nINODES\n");
    for (size_t i = 0; i < super_block.inodes; i++)
    {
        printf("%ld.\t name: %s | isdir: %d | next: %d\n",i , inodes[i].name, inodes[i].dir, inodes[i].next);
    }

    printf("\nBLOCKS\n");
    for (size_t i = 0; i < super_block.blocks; i++)
    {
        printf("%ld.\t next: %d\n",i , disk_blocks[i].next);
    }
}

int find_empty_inode() {
    for (size_t i = 0; i < super_block.inodes; i++)
    {
        if (inodes[i].next == -1) {
            return i;
        }
            
    }
    
    return -1;
}
int find_empty_block() {
    for (size_t i = 0; i < super_block.blocks; i++)
    {
        if (disk_blocks[i].next == -1) {
            return i;
        }
    }
    return -1;
}

int allocate_file(int size, const char* name) {
    if (strlen(name)>7) {
        perror("name of file too long\n");
        exit(EXIT_FAILURE);
    }
    int inode = find_empty_inode();
    if (inode == -1) {
        perror("no empty inode\n");
        exit(EXIT_FAILURE);
    }
    int curr_block = find_empty_block();
    if (curr_block == -1) {
        perror("no empty blocks\n");
        exit(EXIT_FAILURE);
    }
    inodes[inode].size = size;
    inodes[inode].next = curr_block;
    disk_blocks[curr_block].next = -2; 
    strcpy(inodes[inode].name, name);
    if (size>BLOCK_SIZE) {  // REQUIRES TESTS
        int allocated_size = -(3*BLOCK_SIZE)/4;
        //int bn = size/BLOCK_SIZE;
        int next_block;
        while (allocated_size<size)
        {
            next_block = find_empty_block();
            if (next_block == -1) {
                perror("no empty blocks\n");
                exit(EXIT_FAILURE);
            }
            disk_blocks[curr_block].next = next_block;
            curr_block = next_block;
            allocated_size+=BLOCK_SIZE;
        }
    }
    disk_blocks[curr_block].next = -2;

    return inode; 
}

void writebyte(int fd, int pos, char data) { // write only ONE byte into the block
    int rb = inodes[fd].next;
    while (pos>=BLOCK_SIZE) {
        pos-=BLOCK_SIZE;
        rb = disk_blocks[rb].next;
        if (rb==-2||rb==-1) {
            perror("ERROR");
            exit(EXIT_FAILURE);
        }
    }
    disk_blocks[rb].data[pos] = data;
}

char readbyte(int fd, int pos) {
    int rb = inodes[fd].next;
    while (pos>=BLOCK_SIZE) {
        pos-=BLOCK_SIZE;
        rb = disk_blocks[rb].next;
        if (rb==-2||rb==-1) {
            perror("ERROR");
            exit(EXIT_FAILURE);
        }
    }
    return disk_blocks[rb].data[pos];
}

void printfd(int fd){
    int rb = inodes[fd].next;
    printf("NAME: %s\n", inodes[fd].name);

    while(rb!=-2) {
        if (rb==-1) {
            perror("fatal error got into an unallocated disk block\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", disk_blocks[rb].data);
        rb = disk_blocks[rb].next;
    }
    printf("\nDONE %s\n", inodes[fd].name);
}

void printdir(const char* pathname) {
    int fd = myopendir(pathname);
    if (inodes[fd].dir==0) {
        perror("given path is not a dir!");
        exit(EXIT_FAILURE);
    }
    printf("NAME: %s\n", inodes[fd].name);
    struct mydirent* currdir = (struct mydirent*)disk_blocks[inodes[fd].next].data;
    for (size_t i = 0; i < currdir->size; i++)
    {
        printf("%s, ", inodes[currdir->fds[i]].name);
    }
    printf("\nDONE\n");
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) {
    if (source==NULL&&target==NULL) {
        perror("bad use of 'mymount', please specify AT LEAST one of the pair: target or source");
        exit(EXIT_FAILURE);
    }
    if (source!=NULL)
        {resync(source);}
    if (target!=NULL)
        {sync(target);}
}
int mycreatefile(const char *path, const char* name) {
    int newfd = allocate_file(BLOCK_SIZE-50, name);
    int dirfd = myopendir(path);
    struct mydirent *currdir = myreaddir(dirfd);
    currdir->fds[currdir->size++] = newfd;
    return newfd;
}

int myopen(const char *pathname, int flags) {
    char str[80];
    strcpy(str, pathname);
    char *token;
    const char s[2] = "/";
    token = strtok(str, s); // may not work with const 
    char currpath[NAME_SIZE] = "";
    char lastpath[NAME_SIZE] = "";
    while(token != NULL ) {          
        if (token != NULL) {
            strcpy(lastpath, currpath);
            strcpy(currpath, token);
        }
        token = strtok(NULL, s);
    }
    for (size_t i = 0; i < super_block.inodes; i++)
    {
        if (!strcmp(inodes[i].name, currpath)) {
            if (inodes[i].dir!=0) {
                perror("the given path is a directory and can't be opened like a file!");
                exit(EXIT_FAILURE);
            }
            openfiles[i].fd = i;
            openfiles[i].pos = 0;
            return i; 
        }
    }
    int i = mycreatefile(lastpath, currpath); 
    openfiles[i].fd = i;
    openfiles[i].pos = 0;
    return i;
}
int myclose(int myfd) {
    openfiles[myfd].fd = -1;
    openfiles[myfd].pos = -1;
}

size_t myread(int myfd, void *buf, size_t count) {

    if (inodes[myfd].dir==1) {
        perror("cant read a directory like a file");
        exit(EXIT_FAILURE);
    }

    char* buffer = malloc(count);
    for (size_t i = 0; i < count; i++)
    {
        buffer[i] = readbyte(myfd, openfiles[myfd].pos);
        openfiles[myfd].pos++;
    }
    memcpy(buf, buffer, count);
    free(buffer);

    return openfiles[myfd].pos;
    
}

size_t mywrite(int myfd, const void *buf, size_t count) {
    if (inodes[myfd].dir==1) {
        perror("cant write into a directory like a file");
        exit(EXIT_FAILURE);
    }
    char* buffer = (char*)buf;
    for (size_t i = 0; i < count; i++)
    {
        writebyte(myfd, openfiles[myfd].pos, buffer[i]);
        openfiles[myfd].pos++;
    }
    return openfiles[myfd].pos;
}
int mylseek(int myfd, int offset, int whence) {
    if (openfiles[myfd].fd != myfd) {
        perror("file is not opened");
        exit(EXIT_FAILURE);
    }
    if (whence==SEEK_CUR) {
        openfiles[myfd].pos += offset;

    } else if (whence==SEEK_END) {
        openfiles[myfd].pos = inodes[myfd].size+offset;
    } else if (whence==SEEK_SET) {
        openfiles[myfd].pos = offset;
    }
    if (openfiles[myfd].pos<0) {
        openfiles[myfd].pos = 0;
    }
    return openfiles[myfd].pos;
}

int mymkdir(const char *path, const char* name) {
    /**
     * @brief This function goes through the path and finds the FD of the last directory in the path. 
     * Then, it creates a new directory inside the FD that was found. 
     */
    int fd = myopendir(path);
    if (fd==-1) {
        perror("couldnt find dir");
        exit(EXIT_FAILURE);
    }
    if (inodes[fd].dir!=1) {
        perror("can't open a new directory inside a file");
        exit(EXIT_FAILURE);
    }
    int dirblock = inodes[fd].next;
    struct mydirent* currdir = (struct mydirent*)disk_blocks[dirblock].data;
    if (currdir->size>=MAX_DIR_SIZE) {
        perror("too many directories in this directory");
        exit(EXIT_FAILURE);  
    }
    int newdirfd = allocate_file(sizeof(struct mydirent), name);
    currdir->fds[currdir->size++] = newdirfd;
    inodes[newdirfd].dir = 1;
    struct mydirent* newdir = malloc(sizeof(struct mydirent));
    newdir->size = 0;
    for (size_t i = 0; i < MAX_DIR_SIZE; i++)
    {
        newdir->fds[i] = -1;
    }
    
    char *newdiraschar = (char*)newdir;

    for (size_t i = 0; i < sizeof(struct mydirent); i++)
    {
        writebyte(newdirfd, i, newdiraschar[i]);        
    }
    
    return newdirfd;
}

int myopendir(const char *pathname) {
    /**
     * @brief Goes through the pathname and opens the last directory in the path. 
     * Returns the FD of this directory. 
     * 
     */

    char str[80];
    strcpy(str, pathname);
    char *token;
    const char s[2] = "/";
    token = strtok(str, s); // may not work with const 
    char currpath[NAME_SIZE] = "";
    char lastpath[NAME_SIZE] = "";
    while(token != NULL ) {          
        if (token != NULL) {
            strcpy(lastpath, currpath);
            strcpy(currpath, token);
        }
        token = strtok(NULL, s);
    }
    for (size_t i = 0; i < super_block.inodes; i++)
    {
        if (!strcmp(inodes[i].name, currpath)) {
            if (inodes[i].dir!=1) {
                perror("the given path is a file and can't be opened like a directory!");
                exit(EXIT_FAILURE);
            }
            return i; 
        }
    }
    return mymkdir(lastpath, currpath); 
}


struct mydirent *myreaddir(int fd) {
    if (inodes[fd].dir!=1) {
        perror("given fd is not a directory");
        exit(EXIT_FAILURE);
    }
    return (struct mydirent*)disk_blocks[inodes[fd].next].data;
}

int myclosedir(int fd) {
    perror("was not implemented, not relevant in my implementation method of ufs");
    exit(EXIT_FAILURE);
}