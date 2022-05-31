#include "myfs.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    printf("cerating..\n");
    mymkfs(10000); // 10000 size
    printf("syncing..\n");
    sync();  // writing it into an actual file
    printf("printing..\n");
    printf("allocating a file..\n");
    char * test = "abcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuzabcdefghijklmnopqrstuvwxuz";
    int fd = allocate_file(strlen(test), "test"); // just allocated an inode and a first block to a file named test
    printfs_mem("fs_data.txt"); // read from the file and print to terminal
    for (size_t i = 0; i < strlen(test); i++)
    {
        writebyte(fd, i, test[i]);
    }
    printfd(fd);
    
    printf("done!\n");
    return 0;
}
