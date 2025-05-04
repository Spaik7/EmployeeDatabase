#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/file.h"
#include "../include/common.h"

int create_db_file(char *filename)
{
    int fd =open(filename, O_RDWR);
    {
        if(fd != -1)
        {
            close(fd);
            return FILE_ALREADY_EXIST;
        }
    }
    
    
    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if(fd==-1)
    {
        perror("open");
        return STATUS_ERROR;
    }
    return fd;
}

int open_db_file(char *filename)
{
    int fd = open(filename, O_RDWR, 0644 );
    if(fd==-1)
    {
        return ERROR_OPENING;
    }
    return fd;
}

int output_file(int fd, struct dbheader_t* header, struct employee_t *emp)
{
    if(fd<0)
    {
        return BAD_FD;
    }

    if(ftruncate(fd, 0)!= 0)
    {
        return STATUS_ERROR;
    }

    int realcount = header->count;
    
    header->magic=htonl(header->magic);
    header->filesize=htonl(HSIZE + ESIZE * realcount);
    header->count=htons(header->count);
    header->version=htons(VERSION);

    lseek(fd, 0, SEEK_SET);
    write(fd, header, HSIZE);

    int i=0;
    for(;i<realcount;i++)
    {
        emp[i].hours = htonl(emp[i].hours);
        emp[i].ID = htonl(emp[i].ID);
        write(fd,&emp[i], ESIZE);
    }

    return STATUS_SUCCESS;
}

void free_all(struct dbheader_t* header, struct employee_t * emp)
{
    free(emp);
    free(header);
}