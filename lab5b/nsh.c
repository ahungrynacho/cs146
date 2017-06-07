#define _BSD_SOURCE
#include "parse.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*
** A very simple main program that re-prints the line after it's been scanned and parsed.
*/

void exec_command(struct commandLine *cmdLine, int i) 
{
    execvp(cmdLine->argv[cmdLine->cmdStart[i]], &(cmdLine->argv[cmdLine->cmdStart[i]]));
}

void io_infile(struct commandLine *cmdLine, int i)
{
    int in = open(cmdLine->infile, O_RDONLY, S_IRUSR | S_IWUSR);
    dup2(in, STDIN_FILENO);
    exec_command(cmdLine, i);
}

void io_outfile(struct commandLine *cmdLine, int i)
{
  
    int out; 
    switch(cmdLine->append)
    {
        case 0:
            out = open(cmdLine->outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); 
            break;
        default:
            out = open(cmdLine->outfile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            break;
    }
    dup2(out, STDOUT_FILENO);
    exec_command(cmdLine, i);
}

void one_command(struct commandLine *cmdLine)
{
    if (cmdLine->outfile)
    {
        switch(fork())
        {
            case 0:
                io_outfile(cmdLine, 0);
                break;
            default:
                wait(NULL);
                break;
        }
    }
    else if (cmdLine->infile)
        switch(fork())
        {
            case 0:
                io_infile(cmdLine, 0);
                break;
            default:
                wait(NULL);
                break;
        }
    else
    {
        switch (fork())
        {
            case 0:
                exec_command(cmdLine, 0);

            default:
                wait(NULL);
                break;
        }
    }

}


int main(int argc, char *argv[])
{
    FILE *input;    // script file passed in as an argument
    char line[MAX_LINE];

    if(argc == 2)
    {
	input = fopen(argv[1], "r");
        if(input == NULL)
        {
            perror(argv[1]);
            exit(1);
        }
    }
    else
    {
        assert(argc == 1);
        input = stdin;
        printf("? ");
        /* By default, printf will not "flush" the output buffer until
        * a newline appears.  Since the prompt does not contain a newline
        * at the end, we have to explicitly flush the output buffer using
        * fflush.
        */
        fflush(stdout);
    }

    setlinebuf(input);
    while(fgets(line, sizeof(line), input))
    {
        struct commandLine cmdLine;

        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';   /* zap the newline */

        Parse(line, &cmdLine);
            
        if (cmdLine.numCommands == 2)
        {
            int pid;
            int pfd[2];

            if (pipe(pfd) == -1) // create pipe
                perror("pipe");

            switch (pid = fork()) // first child: exec the command that generates output
            {
                case -1:
                    perror("fork");

                case 0:     // child
                    if (dup2(pfd[1], STDOUT_FILENO) == -1)
                        perror("dup2 1");
                    if (close(pfd[0]) == -1)
                        perror("close 1");
                    if (close(pfd[1]) == -1)
                        perror("close 2");
                   
                    if (cmdLine.infile)
                        io_infile(&cmdLine, 0);
                    else
                        exec_command(&cmdLine, 0);      // writes to pipe
                    perror("execvp");

                default:
                    wait(NULL);
                    break;

            }

            switch (pid = fork()) // second child: exec the command that receives input
            {
                case -1:
                    perror("fork");

                case 0:
                    if (dup2(pfd[0], STDIN_FILENO) == -1)
                        perror("dup2 2");
                    if (close(pfd[1]) == -1)
                       perror("close 3"); 
                    if (close(pfd[0]) == -1)
                        perror("close 4");

                    if (cmdLine.outfile)
                        io_outfile(&cmdLine, 1);
                    else
                        exec_command(&cmdLine, 1);
                    perror("execvp");

                default:
                    // 1 parent with 2 children only needed to close fd created by pipe()
                    close(pfd[0]);
                    close(pfd[1]);
                    wait(NULL);
                    break;

            }
        }
        else if (cmdLine.numCommands == 1 && strcmp(cmdLine.argv[0], "cd") == 0)
            chdir(cmdLine.argv[1]);
        else
            one_command(&cmdLine);


        if(input == stdin)
        {
            printf("? ");
            fflush(stdout);
        }
    }

    return 0;
}
