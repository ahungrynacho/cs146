#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

void parse(char * dest, int size, char * buf) {
    Command commands;
    commands.num = 0;
    commands.args = NULL;
   
    buf[strlen(buf) - 1] = '\0';    // replace \n with \0
    char buf_copy[strlen(buf) + 1]; // +1 for the \0 since strcpy appends a \0
    char result[size];

    strcpy(buf_copy, buf);
    char * token = strtok(buf_copy, "|");
    if (token != NULL && strlen(token) >= 1) {        // if the input is not blank
        while (token != NULL) {
            ++commands.num;
            token = strtok(NULL, "|");
        }
        
        strcpy(buf_copy, buf);
        int arg_num = 0;
        token = strtok(buf_copy, " ");
        while (token != NULL) {
            ++arg_num;
            token = strtok(NULL, " ");
        }
        
        strcpy(buf_copy, buf);

        // allocate array of pointers on the stack
        char * args[arg_num];
        commands.args = args;
        
        int i = 0;
        token = strtok(buf_copy, " ");
        while (token != NULL) {
            args[i] = (char *) malloc(sizeof(char) * (strlen(token) + 1));    // each element points to an argument
            strcpy(args[i], token);
            token = strtok(NULL, " ");
            ++i;
        }
        
        // concatentate the output to a single buffer    
        sprintf(result, "%d:", commands.num);
        for (i = 0; i < arg_num; ++i) {
            char buf_cmd[strlen(args[i]) * 2];
            if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0 || strcmp(args[i], "|") == 0) {
                // strcat result without single quotes
                sprintf(buf_cmd, " %s", args[i]);
                strcat(result, buf_cmd);
            }
            else {
                // strcat result with single quotes
                sprintf(buf_cmd, " \'%s\'", args[i]);
                strcat(result, buf_cmd);
            }
        }
        
        // cleanup
        for (int i = 0; i < arg_num; ++i) {
            free(args[i]);
        }

    }
    else {
        sprintf(result, "0:", NULL);
    }
    strcpy(dest, result);

}
