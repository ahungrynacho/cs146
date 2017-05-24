#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

void parse(char * dest, int size, char * buf) {
    Command commands;
    commands.num = 0;
    commands.args = NULL;
   
    char * buf_copy = NULL;
    buf[strlen(buf) - 1] = '\0';    // replace \n with \0
   
    char result[size];

    if (strlen(buf) > 0) {
        buf_copy = (char *) malloc(sizeof(char) * strlen(buf));
        strcpy(buf_copy, buf);
    }

    char * token = strtok(buf_copy, "|");
    if (strlen(buf) > 0 && strlen(token) > 1) {        // if the input is not blank
        while (token != NULL) {
            ++commands.num;
            token = strtok(NULL, "|");
        }
        free(buf_copy); 
      
        
        buf_copy = (char *) malloc(sizeof(char) * strlen(buf));
        strcpy(buf_copy, buf);

        int arg_num = 0;
        token = strtok(buf_copy, " ");
        while (token != NULL) {
            ++arg_num;
            token = strtok(NULL, " ");
        }
        free(buf_copy);
        
        buf_copy = (char *) malloc(sizeof(char) * strlen(buf));
        strcpy(buf_copy, buf);

        // allocate array of pointers
        commands.args = (char **) malloc(sizeof(char *) * arg_num);
        char ** args = commands.args;
        
        token = strtok(buf_copy, " ");
        while (token != NULL) {
            *args = (char *) malloc(sizeof(char) * strlen(token));
            strcpy(*args, token);
            token = strtok(NULL, " ");
            ++args;
        }
        *args = NULL;
        free(buf_copy);
        
        // concatentate the output to a single buffer    
        sprintf(result, "%d:", commands.num);
        for (char ** arg = commands.args; *arg != NULL; ++arg) {
            char buf_cmd[strlen(*arg) * 2];
            if (strcmp(*arg, "<") == 0 || strcmp(*arg, ">") == 0 || strcmp(*arg, "|") == 0) {
                // strcat result without single quotes
                sprintf(buf_cmd, " %s", *arg);
                strcat(result, buf_cmd);
            }
            else {
                // strcat result with single quotes
                sprintf(buf_cmd, " \'%s\'", *arg);
                strcat(result, buf_cmd);
            }
        }
        strcpy(dest, result);


        /*        
        // cleanup
        for (char ** ptr = commands.args; *ptr != NULL; ++ptr) {
            free(*ptr);
        }
        free(commands.args);
        */
        
    }

}
