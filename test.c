#include "myfs.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("cerating..\n");
    mymkfs(10000); // 10000 size
    printf("syncing..\n");
    sync();  // writing it into an actual file
    printf("printing..\n");
    printfs("fs_data.txt"); // read from the file and print to terminal
    printf("allocating a file..\n");
    allocate_file("test"); // just allocated an inode and a first block to a file named test
    allocate_file("test2");
    printf("printing..\n"); 
    printfs("fs_data.txt"); // print, but without syncing so you won't see the change
    printf("syncing..\n");
    sync(); // writing it into an actual file
    printf("printing..\n"); 
    printfs("fs_data.txt"); // now you can see the change

    printf("done!\n");
    return 0;
}
