#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

#include "../include/file.h"
#include "../include/parse.h"
#include "../include/common.h"

int main(int argc, char *argv[]) 
{
    struct dbheader_t *header = NULL;
    struct employee_t *employees =NULL;
    struct employee_t *Eupdate = NULL;

    char *filepath = NULL;
    char *addstring = NULL;
    char *remove_target = NULL;
    char *update = NULL;
    char *detail = NULL;

    int args;
    int dbfd = -1; //database file descriptor     
    int returnstatus =0;

    bool newfile = false;
    bool list=false;
    
    

    while((args = getopt(argc, argv, "hnf:a:lr:u:N:A:I:H:d:"))!=-1)
    {
        switch(args)
        {
            case 'h':
                print_usage(argv);
                return 0;
            case 'n': //new file
                newfile = true;
                break;
            case 'f': // filepath 
                filepath = optarg;
                break;
            case 'a': // add
                addstring=optarg;
                break;
            case 'l': // list
                list=true;
                break;
            case 'r':
                remove_target = optarg;
                break;
            case 'u': // update
                update = optarg;
                if (initUpdate(&Eupdate)) {
                    return ERROR_MALLOC;
                }
                break;
            case 'N':   
                strncpy(Eupdate->name, optarg, sizeof(Eupdate->name));
                break;
            case 'A': 
                strncpy(Eupdate->address, optarg, sizeof(Eupdate->address));
                break;
            case 'I': 
                Eupdate->ID = atoi(optarg);
                break;
            case 'H': 
                Eupdate->hours = atoi(optarg);
                break;
            case 'd':
                detail=optarg;
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                break;
            default:
                return STATUS_ERROR;
        }
    }


    if(filepath == NULL)
    {
        printf("Filepath is requeired\n");
        print_usage(argv);    
        free_all(header, employees);
        return STATUS_ERROR;
    }

    if(newfile)
    {
        dbfd = create_db_file(filepath);
        if(dbfd == FILE_ALREADY_EXIST)
        {
            printf("The file already exist\n");
            free_all(header, employees);
            return STATUS_ERROR;
        }
        
        if(create_db_header(dbfd, &header) ==ERROR_CREATING_DB_HEADER)
        {
            printf("Failed to create database header\n");
            free_all(header, employees);
            return STATUS_ERROR;
        }
    }
    else 
    {
        dbfd = open_db_file(filepath); 
        if(dbfd == ERROR_OPENING)
        {
            perror("open");
            printf("Unable to open database file");
            free_all(header, employees);
            return STATUS_ERROR;
        }
        returnstatus=validate_db_header(dbfd,&header);
        if(returnstatus<0)
        {
            switch (returnstatus)
            {
                case ERROR_MALLOC:
                    perror("calloc");
                    break;
                case ERROR_READING:
                    perror("read");
                    break;
                case INVALID_VERSION:
                    printf("The file it's not of the same version of the program\n");
                    break;
                case INVALIDE_MAGIC:
                    printf("The file doesn't have the right number\n");
                    break;
                case INVALID_FILESIZE:
                    printf("The file has been manipulated\n");
                    break;
                
                default:
                    break;
                
            }
            free_all(header, employees);
            return returnstatus;
        }
    }

    returnstatus=read_employees(dbfd, header, &employees);
    if(returnstatus<0)
    {
        free_all(header, employees);
        return returnstatus;
    }

    if(addstring)
    {
        header->count++;
        employees=realloc(employees, header->count*ESIZE);
        if(add_employee(header, employees, addstring)==0)
        {
            printf("Employee %s added!\n", employees[header->count-1].name);
        }
    }
    
    if(list)
    {
        list_employees(header,employees);
    }

    if (remove_target)
    {
        switch (remove_employee(header->count, employees, remove_target))
        {
            case EMPLOYEE_NOT_FOUND:
                printf("Employee not found with: %s\n", remove_target);
                list_employees(header, employees);
                free_all(header, employees);
                return EMPLOYEE_NOT_FOUND;

            case STATUS_ABORTED:
                break;

            case STATUS_SUCCESS:
                header->count--;
                employees = realloc(employees, header->count * ESIZE);
                break;
        }
    }

    if(update)
    {
        switch(update_employee(header->count,employees, update, Eupdate))
        {
            case EMPLOYEE_NOT_FOUND:
                printf("Employee not found with: %s\n", update);
                list_employees(header, employees);
                free_all(header, employees);
            return EMPLOYEE_NOT_FOUND;

            case STATUS_ABORTED:
                break;
        }   
    }

    if(detail)
    {
        switch(detail_employee(header->count,employees, detail))
        {
            case EMPLOYEE_NOT_FOUND:
                printf("Employee not found with: %s\n", detail);
                list_employees(header, employees);
                free_all(header, employees);
            return EMPLOYEE_NOT_FOUND;
        }
    }

    
    returnstatus = output_file(dbfd, header, employees);
    if(returnstatus<0)
    {
        free_all(header, employees);
        return returnstatus;
    }

}



