#ifndef COMMON_H
#define COMMON_H

#define STATUS_SUCCESS 0
#define STATUS_ERROR -1
#define FILE_ALREADY_EXIST -2
#define ERROR_OPENING -3
#define ERROR_CREATING_DB_HEADER -4
#define BAD_FD -5
#define ERROR_MALLOC -6
#define ERROR_READING -7


#define INVALID_VERSION -10
#define INVALIDE_COUNT -11
#define INVALID_FILESIZE -12
#define INVALIDE_MAGIC -13

#define EMPLOYEE_NOT_FOUND 404
#define STATUS_ABORTED 1


#define VERSION 2
#define HEADER_MAGIC 0x4c4c4144

struct dbheader_t{
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct employee_t {
    char name[256];
    char address[256];
    unsigned int hours;
    unsigned int ID; 
};

#define HSIZE sizeof(struct dbheader_t)
#define ESIZE sizeof(struct employee_t)

#endif
