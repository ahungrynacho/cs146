#ifndef PARSE_H
#define PARSE_H

typedef struct Command {
    int num;
    char ** args;    
} Command;

void parse(char * dest, int size, char * buf);
#endif
