#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#define BUF_SIZE 4096

int main(int argc, char ** argv) {


    while (1) {
        printf("? ");
        char buf[BUF_SIZE];
        char dest[BUF_SIZE];
        fgets(buf, BUF_SIZE, stdin);
        parse(dest, BUF_SIZE, buf);
        printf("%s\n", dest);
    }
    

        
    return 0;
}
