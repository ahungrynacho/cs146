#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#define BUF_SIZE 4096

int main(int argc, char ** argv) {


    while (1) {
        printf("? ");
        char buf[BUF_SIZE];
        fgets(buf, BUF_SIZE, stdin);
        parse(buf, BUF_SIZE);
        //printf("%s\n", parse(buf, BUF_SIZE));
    }
    

        
    return 0;
}
