#include "myfs.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    printf("creating root..\n");
    mymkfs(100000); 
    // ############### test1: open the same directory ###############
    int t1 = myopendir("root/test1");
    int t2 = myopendir("root/test1");
    int t = (t1 == t2);
    if(t == 1)
    {
        printf("myopendir test PASSED\n");
    }else{
        printf("myopendir test FAILED\n");
    }


    myopendir("root/test2");

    // ############### test2: open the same file with myopen ###############


    int fd = myopen("root/test1/file1", 0);
    int testfd = myopen("root/test1/file1", 0);
    int fd2 = myopen("root/test2/file2",0);
    int fd3 = myopen("root/test1/file2",0);
  
    int i = (fd == testfd);
    if(i == 1)
    {
        printf("myopen test PASSED\n");
    }else{
        printf("myopen test FAILED\n");
    }
    // ############### test3: open the same file in different directories ###############

    i = (fd2 == fd3);
    if(i == 1)
    {
        printf("different directory open file PASSED\n");
    }else{
        printf("different directory open file FAILED\n");
    }



    // ############### test4: closing file changing fd ###############

    testfd = myclose(testfd);

    i = (fd != testfd);
    if(i == 1)
    {
        printf("myclose test PASSED\n");
    }else{
        printf("myclose test FAILED\n");
    }

    // ############### test5: write, read and seek operation on files ###############

    char* test = "this is a test\n"; 

    int ptr =  mywrite(fd2, test, 25);
    char buf[25];
    char buf2[25];
    mylseek(fd2, -25, SEEK_CUR);
    myread(fd2, buf, 25);


     if(strcmp(buf, test) ==0)
    {
        printf("read test PASSED\n");
    }else
    {
        printf("read test FAILED\n");
    }

    mylseek(fd2,-15,SEEK_CUR);
    myread(fd2, buf2, 15);
   
    if(strcmp(buf2, "test\n") ==0)
    {
        printf("seek test PASSED\n\n");
    }else
    {
        printf("seek test FAILED\n\n");
    }

    // ############### test6: testing write big file ###############

    char* test2 = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$test";
    int newfd = myopen("root/test1/bigfile", 0);

    char bigbuf[10];
    mywrite(newfd,test2,607);
    mylseek(newfd,-8,SEEK_CUR);
    myread(newfd,bigbuf,4);
    printf("read from big file: %s\n", bigbuf);
    // printfd(newfd);
    if(strcmp(bigbuf, "test") ==0)
    {
        printf("seek test big file PASSED\n\n");
    }else
    {
        printf("seek test big file FAILED\n\n");
    }
    
    

    printf("our fs looks like this:\n\n");
    printdir("root");
    printdir("test1");
    printdir("test2");

   
    return 0;
}
