#ifndef PARSE_H
#define PARSE_H

#include "common.h"

void print_usage (char *argv[]);
int initUpdate(struct employee_t **Eupdate);

int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut); 
int read_employees (int fd, struct dbheader_t *, struct employee_t **employeesOut);
int add_employee(struct dbheader_t *header, struct employee_t *employees, char * addstring);
void list_employees(struct dbheader_t* header,struct employee_t *employees);
int remove_employee(int count, struct employee_t *employees, char *target);
int update_employee(int count, struct employee_t *employees, char *target, struct employee_t *change);
int detail_employee(int count, struct employee_t *employees, char *target);

#endif