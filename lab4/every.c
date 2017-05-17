#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


int main(int argc, char ** argv) {
    
    if (argc < 2) {
        char * line = NULL;
        size_t length = 0;
        ssize_t read = 0;

        while ((read = getline(&line, &length, stdin) != EOF)) {
            // do nothing and let the process hang
        }
        return 0;
    }

    int num_skips = 1;      // N
    int num_lines = 1;      // M
    int num_files = argc - 1;       // substract the name of program
    char ** file_names = argv + 1;        // if no interval is supplied
                                          // point to the beginning of the list of file names
    

    char * interval = NULL;
    if (argv[1][0] == '-') {    // check if the first argument is an option
        interval = argv[1];     // point to the option
        ++interval;     // advance the pointer by 1 char to skip the '-'
        file_names += 1;        // point to the beginning of the list of file names
        num_files = argc - 2;   // subtract the program name and supplied option


    }
    else if (getenv("EVERY")) {
        interval = getenv("EVERY");
    }

    if (interval != NULL) {

        const char delimiter[2] = ",";
        char * token = strtok(interval, delimiter);
        num_skips = atoi(token);    // first token

        while (token != NULL) {
            num_lines = atoi(token);    // second token
            token = strtok(NULL, delimiter);
        }
    }

    for (int i = 0; i < num_files; ++i) {
        
        char * line = NULL;
        size_t length = 0;
        ssize_t read = 0;
        FILE * file = fopen(file_names[i], "r");

        if (file == NULL)
            exit(EXIT_FAILURE);
        
        int line_num = 0;    // current line number in the file
        int begin = 0;      // beginning of interval
        int end = 0;        // end of interval

        printf("File: %s\n", file_names[i]);
        while ((read = getline(&line, &length, file)) != EOF) {
            if ((line_num % num_skips) == 0) {
                begin = line_num;
                end = begin + num_lines;
                printf("%s", line);
            }
            else if (line_num >= begin && line_num < end) {
                printf("%s", line);
            }
            ++line_num;
            
        }
        fclose(file);
        if (line)
            free(line);

    }
    return 0;
}
