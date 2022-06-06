#include "mystdio.h"
#include <math.h>
/* // for reference:
typedef struct myFILE {
	char* data;
	int size;
	int ptr;
}myFILE;
*/

myFILE* myfopen(const char *pathname, const char *mode) {
    /**
     * @brief This function uses myfs.h lib to find the fd of the given file, 
     * then read all of the file's data into myFILE. The data is stored on the heap for quick accsess. 
     * 
     */
    if (!mode) {
        errno = 22;
        return -1; 
    }
    if ((strcmp(mode, "a")&&strcmp(mode, "w")&&strcmp(mode, "r")&&strcmp(mode, "r+"))) {
        errno = 22;
        return -1; 
    }
    int fd = myopen(pathname, 0);
    if (fd==-1) {
        return -1;
    }
    
    myFILE* currfile = (myFILE*)malloc(sizeof(myFILE));
    strcpy(currfile->mode, mode);
    currfile->fd = fd;
    if (!strcmp(mode, "a")) { // set the pointer to the end of the file
        currfile->ptr = inodes[fd].size;
    } else {
        currfile->ptr = 0;
    }
    currfile->size = inodes[fd].size;
    currfile->data = malloc(currfile->size);
    if (!strcmp(mode, "w")) { // wipe the content of the file 
        for (size_t i = 0; i < currfile->size; i++)
        {
            currfile->data[i] = ' ';
        }
        currfile->data[currfile->size] = '\0';
    } else {
        for (size_t i = 0; i < currfile->size; i++)
        {
            currfile->data[i] = readbyte(fd, i);
        }
    }
    myclose(fd);
    return currfile;
}
int myfclose(myFILE *stream) {
    /**
     * @brief Copies each byte of data from @param stream into the file itself,
     *  Then, free the @struct myFILE. 
     */
    if (!strcmp(stream->mode, "r")) {
        free(stream->data);
        free(stream);
        return 1;
    }
    
    for (size_t i = 0; i < stream->size; i++)
    {
        writebyte(stream->fd, i, stream->data[i]);
    }
    
    free(stream->data);
    free(stream);
    return 1;
}
size_t myfread(void * ptr, size_t size, size_t nmemb, myFILE * stream) { 
    /**
     * @brief  The function myfread() reads @param nmemb items of data, 
     * each @param size bytes long, from the stream pointed to by @param stream, storing them at the location given by @param ptr.
     * @return the actual position of the current myFILE ptr index.
     */
    int count = size*nmemb;
    char* buffer = malloc(count+1);
    size_t i;
    int optr = stream->ptr;
    for (i = 0; i < count; i++)
    {
        if (stream->ptr+i>stream->size) {
            break;
        }
        buffer[i] = stream->data[stream->ptr+i];
        optr++;
    }
    stream->ptr = optr;
    buffer[count] = '\0';
    strncpy(ptr, buffer, count);
    free(buffer);

    return stream->ptr;

}
size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream) {
    /**
     * @brief  The function fwrite() writes @param nmemb items of data, 
     * each @param size bytes long, to the stream pointed to by @param stream, obtaining them from the location given by @param ptr.
     * @return the actual position of the current myFILE ptr index.
     */
    if (!strcmp(stream->mode, "r")) {
        errno = 30;
        return -1;
    }
    int count = size*nmemb;
    char* buffer = (char*)ptr;
    if (stream->ptr+count>stream->size) {
        char* tempbuffer = malloc(stream->size+1);
        for (size_t i = 0; i < stream->size; i++)
        {
            tempbuffer[i] = stream->data[i];
        }
        free(stream->data);
        stream->data = malloc(stream->ptr+count);
        for (size_t i = 0; i < stream->size; i++)
        {
            stream->data[i] = tempbuffer[i];
        }
        free(tempbuffer);
    }
    int optr = stream->ptr;
    for (size_t i = 0; i < count; i++)
    {
        stream->data[stream->ptr+i] = buffer[i];
        optr++;
    }
    stream->ptr = optr;
    return stream->ptr;
}
int myfseek(myFILE *stream, long offset, int whence) {
    /**
     * @brief This function is used to move the file pointer, @return the new location of the pointer.
     * 
     */
    if (whence==SEEK_CUR) {
        stream->ptr += offset;
    } else if (whence==SEEK_END) {
        stream->ptr = stream->size+offset;
    } else if (whence==SEEK_SET) {
        stream->ptr = offset;
    }
    if (stream->ptr<0) {
        stream->ptr = 0;
    }
    return stream->ptr;
}
int myfscanf(myFILE * stream, const char * format, ...) {
    /**
     * @brief This function uses a va list format to read data from the file stream
     */
    va_list arguments;
    va_start ( arguments, format );
    char* currbuffer; // used to store numbers 
    int i = 0, j = 0; // format index, strean->data index
    while (format && format[i]) {
        if (format[i] == '%') {
            if (format[i+1]=='d') {
                *(int *)va_arg( arguments, int* ) = strtol(&stream->data[j], &currbuffer, 10);

                j+=strlen(currbuffer) - stream->size;
            } else if (format[i+1]=='f') {
                *(double *)va_arg( arguments, double* ) = strtol(&stream->data[j], &currbuffer, 10);
                j+=strlen(currbuffer) - stream->size;
            } else if (format[i+1]=='c') {
                *(char *)va_arg( arguments, char* ) = stream->data[j];
            } 
            i++;
        }
        i++;
        j++;
    }
    return 1;
}


int myfprintf(myFILE * stream, const char * format, ...) {
    /**
     * @brief This function uses a va list format to write data into the file stream
     */
    va_list arguments;
    va_start ( arguments, format );
    char *buffer = malloc(strlen(format)+5000);
    char *currbuffer = malloc(500);
    int j = 0; // buffer's index
    for (size_t i = 0; i < strlen(format); i++)
    {
        memset(currbuffer, 0, 500);
        if (format[i]=='%') {
            if (format[i+1]=='d') {
                int currvar = va_arg ( arguments, int );
                sprintf(currbuffer,"%d",currvar);
            } else if (format[i+1]=='f') {
                double currvar = va_arg ( arguments, double );
                sprintf(currbuffer,"%f",currvar);
            } else if (format[i+1]=='c') {
                char currvar = va_arg ( arguments, int );
                currbuffer[0] = currvar;
            } 
            i++;
        } else {
            currbuffer[0] = format[i];
        }
        for (size_t k = 0; k < strlen(currbuffer); k++)
        {
            buffer[j] = currbuffer[k];
            j++;
        }
    }
    buffer[j] = '\0';
    myfwrite(buffer, strlen(buffer), 1, stream);
    //free(buffer);
    return 1;
}

