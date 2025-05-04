#ifndef FILE_H
#define FILE_H

#include "common.h"

int create_db_file(char *filename);
int open_db_file(char *filename);
int output_file(int fd, struct dbheader_t* header, struct employee_t *emp);
void free_all(struct dbheader_t* header, struct employee_t * emp);

#endif