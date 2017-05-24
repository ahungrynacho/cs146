#ifndef PARSE_H
#define PARSE_H

typedef struct Command {
    int num;
    char ** args;    
} Command;

char * parse(char * buf, int size);
#endif
