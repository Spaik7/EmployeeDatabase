#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "../include/parse.h"
#include "../include/common.h"


void print_usage (char *argv[]) {
    printf("\t -h - show this page\n");
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    printf("\t -a - add a new employee\n");
    printf("\t -l - list all the employee\n");
    printf("\t -r - remove an employee by ID or by name\n");
    printf("\t -d - detail about an employee (to implement)\n");
    printf("\t -u - update an employee\n");
    printf("\t\t -N - update the name of an employee \"Name\"\n");
    printf("\t\t -A - update the address of an employee \"addr\"\n");
    printf("\t\t -I - update the ID of an employee\n");
    printf("\t\t -H - update the hour of an employee\n");

    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("Usage: %s -f <database file> -a \"Name,Address,hour\"\n", argv[0]);
    printf("\tNote: maintain the order for the add\n");
    printf("Usage: %s -f <database file> -u <ID> -N \"Name\"\n", argv[0]);
    printf("Usage: %s -f <database file> -u <ID> -A \"Addr\"\n", argv[0]);
    printf("Usage: %s -f <database file> -u <ID> -I <New ID>\n", argv[0]);
    printf("Usage: %s -f <database file> -u <ID> -H <New hour>\n", argv[0]);
    printf("Usage: %s -f <database file> -r <ID>/<\"Name\">\n", argv[0]);
    return;
}

int create_db_header(int fd, struct dbheader_t **headerOut)
{
    struct dbheader_t *header = calloc(1, HSIZE);
    if(header == -1)
    {
        return ERROR_CREATING_DB_HEADER;
    }
    header->count = 0;
    header->version =0x1;
    header->magic = HEADER_MAGIC;
    header->filesize = HSIZE;

    *headerOut = header;
    return STATUS_SUCCESS;

}

int validate_db_header(int fd, struct dbheader_t **headerOut)
{
    if(fd<0)
    {
        return BAD_FD;
    }

    struct dbheader_t *header = calloc(1, HSIZE);
    if(header == -1)
    {
        return ERROR_MALLOC;
    }
    
    if(read(fd, header, HSIZE) != HSIZE)
    {
        free(header);
        return ERROR_READING;
    }

    header->version=ntohs(header->version);
    header->count=ntohs(header->count);
    header->magic=ntohl(header->magic);
    header->filesize=ntohl(header->filesize);

    if(header->version!=VERSION)
    {
        free(header);
        return INVALID_VERSION;
    }

    if(header->magic!=HEADER_MAGIC)
    {
        free(header);
        return INVALIDE_MAGIC;
    }

    struct stat dbstat = {0};
    fstat(fd,&dbstat);
    if(header->filesize!= dbstat.st_size)
    {
        free(header);
        return INVALID_FILESIZE;
    }

    *headerOut=header;

}

int read_employees (int fd, struct dbheader_t *header, struct employee_t **employeesOut)
{
    if(fd<0)
    {
        return BAD_FD;
    }
    int count = header->count;

    struct employee_t *employees = calloc(count, ESIZE);
    if(employees == -1)
    {
        return ERROR_MALLOC;
    }

    read(fd, employees, count*ESIZE);
    int i=0;
    for(;i<count; i++)
    {
        employees[i].hours = ntohl(employees[i].hours);
        employees[i].ID = ntohl(employees[i].ID);
    }
    *employeesOut=employees;
    return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *header, struct employee_t *employees, char * addstring)
{
    char *name =strtok(addstring, ",");
    char *addr = strtok(NULL, ",");
    int hours = atoi(strtok(NULL, ","));
    int ID = header->count;

    strncpy(employees[header->count-1].name, name,sizeof(employees[header->count-1].name));
    strncpy(employees[header->count-1].address, addr,sizeof(employees[header->count-1].address));
    employees[header->count-1].hours = hours;
    employees[header->count-1].ID = ID;
    return STATUS_SUCCESS;
}

void list_employees(struct dbheader_t* header,struct employee_t *employees)
{
    int i=0;
    for(;i<header->count;i++)
    {
        printf("Employee with ID: %d\n", employees[i].ID);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHour Work: %d\n", employees[i].hours);
    }
    
}

int remove_employee(int count, struct employee_t *employees, char *target) 
{
    int i = 0;
    int is_id = 1;

    for (int j = 0; target[j]; j++) {
        if (target[j] < '0' || target[j] > '9') {
            is_id = 0;
            break;
        }
    }

    int target_id = atoi(target);

    for (; i < count; i++) {
        if ((is_id && employees[i].ID == target_id) ||
            (!is_id && strcmp(employees[i].name, target) == 0)) {
            
            printf("You are about to remove this employee:\n");
            printf("\t%s\n", employees[i].name);
            printf("\t%s\n", employees[i].address);
            printf("\t%d\n", employees[i].hours);
            printf("Continue? [Y/n] ");

            char input;
            scanf(" %c", &input);
            if (input != 'Y' && input != 'y') {
                return STATUS_ABORTED;
            }

            // Shift elements
            for (int j = i; j < count - 1; j++) {
                employees[j] = employees[j + 1];
            }

            // Clear last
            memset(&employees[count - 1], 0, sizeof(struct employee_t));
            strncpy(employees[count - 1].name, "null", sizeof(employees[count - 1].name));
            strncpy(employees[count - 1].address, "null", sizeof(employees[count - 1].address));

            return STATUS_SUCCESS;
        }
    }

    return EMPLOYEE_NOT_FOUND;
}

int update_employee(int count, struct employee_t *employees, char *target, struct employee_t *change)
{
    int i = 0;
    int is_id = 1;

    for (int j = 0; target[j]; j++) {
        if (target[j] < '0' || target[j] > '9') {
            is_id = 0;
            break;
        }
    }

    int target_id = atoi(target);

    for (; i < count; i++) {
        if ((is_id && employees[i].ID == target_id) ||
            (!is_id && strcmp(employees[i].name, target) == 0)) 
        {
            char input;
            printf("You are about to update this employee, with the ID %d: \n", employees[i].ID);
            printf("\t %s\n", employees[i].name);
            printf("\t %s\n", employees[i].address);
            printf("\t %d\n", employees[i].hours);

            change->ID=  change->ID ?: employees[i].ID;
            change->hours= change->hours ?: employees[i].hours;
            if (strlen(change->name) == 0)
                strncpy(change->name, employees[i].name, sizeof(employees[i].name));

            if (strlen(change->address) == 0)
                strncpy(change->address, employees[i].address, sizeof(employees[i].address));


            printf("It will became -> \n");
            printf("\t %d\n", change->ID);
            printf("\t %s\n", change->name);
            printf("\t %s\n", change->address);
            printf("\t %d\n", change->hours);
            printf("Continue? [Y/n] ");
            scanf("%c", &input);
            if(input != 'Y' || input != 'y')
            {
                employees[i]=*change;
                
                return STATUS_SUCCESS;
            }
            return STATUS_ABORTED;
        }
    }
    return EMPLOYEE_NOT_FOUND;
}

int initUpdate(struct employee_t **Eupdate)
{
    *Eupdate = calloc(1, sizeof(struct employee_t));
    if (*Eupdate != NULL)
    {
        (*Eupdate)->ID = 0;
        (*Eupdate)->hours = 0;
        (*Eupdate)->name[0] = '\0';
        (*Eupdate)->address[0] = '\0';
        return STATUS_SUCCESS;
    }
    return STATUS_ERROR;
}

int detail_employee(int count, struct employee_t *employees, char *target)
{
    int i = 0;
    int is_id = 1;

    for (int j = 0; target[j]; j++) {
        if (target[j] < '0' || target[j] > '9') {
            is_id = 0;
            break;
        }
    }

    int target_id = atoi(target);

    for (; i < count; i++) {
        if ((is_id && employees[i].ID == target_id) ||
            (!is_id && strcmp(employees[i].name, target) == 0))
        {
            printf("Employee ID: %d\n", employees[i].ID);
            printf("Employee name: %s\n", employees[i].name);
            printf("Employee address: %s\n", employees[i].address);
            printf("Employee hours: %d\n", employees[i].hours);
            return STATUS_SUCCESS;
        }
    }
    return EMPLOYEE_NOT_FOUND;
}