#include "mystdio.h"
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
    int fd = myopen(pathname, mode);
    if (fd==-1) {
        perror("error fd is -1");
        exit(EXIT_FAILURE);
    }
    
    myFILE* currfile = (myFILE*)malloc(sizeof(myFILE));
    currfile->fd = fd;
    currfile->ptr = 0;
    currfile->size = inodes[fd].size;
    currfile->data = malloc(currfile->size);
    for (size_t i = 0; i < currfile->size; i++)
    {
        currfile->data[i] = readbyte(fd, i);
    }
        
    myclose(fd);
    return currfile;
}
int myfclose(myFILE *stream) {
    /**
     * @brief Copies each byte of data from @param stream into the file itself,
     *  Then, free the @struct myFILE. 
     */
    for (size_t i = 0; i < stream->size; i++)
    {
        writebyte(stream->fd, i, stream->data[i]);
    }
    
    free(stream->data);
    free(stream);
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
    for (i = 0; i < count; i++)
    {
        if (stream->ptr+i>stream->size) {
            break;
        }
        buffer[i] = stream->data[stream->ptr+i];
        stream->ptr++;
    }
    buffer[i+1] = '\0';
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
    for (size_t i = 0; i < count; i++)
    {
        stream->data[stream->ptr+i] = buffer[i];
        stream->ptr++;
    }
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
int myfscanf(myFILE * stream, const char * format, ...);
int myfprintf(myFILE * stream, const char * format, ...);