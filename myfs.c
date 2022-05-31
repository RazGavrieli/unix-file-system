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
    inodes = malloc(size_without_superblock/10);
    disk_blocks = malloc(size_without_superblock-size_without_superblock/10);

 
    super_block.inodes = (size_without_superblock/10)/(sizeof(struct inode)); 
    super_block.blocks = (size_without_superblock-size_without_superblock/10)/(sizeof(struct disk_block));


    for (size_t i = 0; i < super_block.inodes; i++)
    {
        strcpy(inodes[i].name, "");
        inodes[i].next = -1;
    }

    for (size_t i = 0; i < super_block.blocks; i++)
    {
        strcpy(disk_blocks[i].data, "");
        disk_blocks[i].next = -1;
    }
    
}
void sync() {
    FILE *file;
    file = fopen("fs_data.txt", "w+");
    fwrite(&super_block, sizeof(struct super_block), 1, file);
    fwrite(inodes, super_block.inodes*sizeof(struct inode), 1, file);
    fwrite(disk_blocks, super_block.blocks*sizeof(struct disk_block), 1, file);
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
        printf("%ld.\t name: %s | next: %d\n",i , temp_inodes[i].name, temp_inodes[i].next);
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
        printf("%ld.\t name: %s | next: %d\n",i , inodes[i].name, inodes[i].next);
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

int allocate_file(int size, char* name) {
    if (strlen(name)>7) {
        printf("name of file too long\n");
        exit(EXIT_FAILURE);
    }
    int inode = find_empty_inode();
    if (inode == -1) {
        printf("no empty inode\n");
        exit(EXIT_FAILURE);
    }
    int curr_block = find_empty_block();
    if (curr_block == -1) {
        printf("no empty blocks\n");
        exit(EXIT_FAILURE);
    }
    inodes[inode].next = curr_block;
    disk_blocks[curr_block].next = -2; 
    strcpy(inodes[inode].name, name);

    int bn = size/BLOCK_SIZE;
    int next_block;
    for (size_t i = 0; i < bn; i++)
    {
        next_block = find_empty_block();
        if (next_block == -1) {
            printf("no empty blocks\n");
            exit(EXIT_FAILURE);
        }
        disk_blocks[curr_block].next = next_block;
        curr_block = next_block;
    }
    disk_blocks[curr_block].next = -2;

    
    return inode; // is this the right number to send back?
}

void writebyte(int fd, int pos, char data) { // write only ONE byte into the block
    int rb = inodes[fd].next;
    while (pos>=BLOCK_SIZE) {
        pos-=512;
        rb = disk_blocks[rb].next;
        if (rb==-2||rb==-1) {
            printf("ERROR");
            exit(EXIT_FAILURE);
        }
    }
    disk_blocks[rb].data[pos] = data;
}

void printfd(int fd){
    int rb = inodes[fd].next;
    printf("NAME: %s\n", inodes[fd].name);

    while(rb!=-2) {
        if (rb==-1) {
            printf("ERROR\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", disk_blocks[rb].data);
        rb = disk_blocks[rb].next;
    }
    printf("\nDONE %s\n", inodes[fd].name);
}

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) {
    
}


int myopen(const char *pathname, int flags);
int myclose(int myfd);
size_t myread(int myfd, void *buf, size_t count);
size_t mywrite(int myfd, const void *buf, size_t count);
off_t mylseek(int myfd, off_t offset, int whence);

myDIR *myopendir(const char *name);
struct mydirent *myreaddir(myDIR *dirp);
int myclosedir(myDIR *dirp);