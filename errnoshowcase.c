#include "sources/mystdio.h"


int main(int argc, char const *argv[])
{
    /**
     * @brief The following are three cases of errno use in my ufs implementation
     * 
     */
    mymount("testfile.txt", NULL, NULL, NULL, NULL);
    myFILE* fptr = myfopen("thits", "f");
    if (fptr == -1) {
        printf("Value of errno: %d\n", errno);
        fprintf( stderr, "system error: %s\n" , strerror(errno) );
    }

    myFILE* fptr1 = myfopen("root/test1/file2", "r");
    char* test = "test";
    int ret = myfwrite(test, 1, 4, fptr1);
    if (ret == -1) {
        printf("Value of errno: %d\n", errno);
        fprintf( stderr, "system error: %s\n" , strerror(errno) );
    }
    myfclose(fptr1);

    int fd = myopen("root/test1", 0); // (Is a directory)
    if (fd == -1) {
        printf("Value of errno: %d\n", errno);
        fprintf( stderr, "system error: %s\n" , strerror(errno) );
    }
    myclose(fd);
    return 0;
}
