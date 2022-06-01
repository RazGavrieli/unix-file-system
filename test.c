#include "myfs.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    printf("cerating..\n");
    mymkfs(100000); 
    myopendir("root/test");
    myopendir("root/test3");
    int fd = myopen("root/test/newfile", NULL);
    int fd2 = myopen("root/test/newfil1", NULL);
    printdir("root");
    printdir("test");
    printdir("test3");

    char* test = "this is a test"; 
    int ptr =  mywrite(fd, test, 25);
    char buf[25];
    mylseek(fd, -25, SEEK_CUR);
    myread(fd, buf, 25);
    printf("%s", buf);
    myclose(fd);
    return 0;
}
