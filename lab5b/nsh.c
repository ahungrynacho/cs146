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

void io_redir(struct commandLine *cmdLine, int i)
{
    int fd[2];
    int save_stdout = dup(STDOUT_FILENO);
    int save_stdin = dup(STDIN_FILENO);
    if (pipe(fd) == -1)
        perror("pipe");

    switch(fork())
    {
        case -1:
            perror("fork");
        case 0:
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            exec_command(cmdLine, i);
        default:
            dup2(fd[0], STDIN_FILENO);
            wait(NULL);

            int out;
            if (cmdLine->append)
                out = open(cmdLine->outfile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            else
                out = open(cmdLine->outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); 
            
            dup2(out, fd[0]);
            close(fd[0]);
            close(fd[1]);
            close(out);

            // restore original file descriptors
            dup2(save_stdout, STDOUT_FILENO);
            dup2(save_stdin, STDIN_FILENO);
            close(save_stdout);
            close(save_stdin);
            break;
    }

    int input;
    if (cmdLine->append)
    {
        input = open(cmdLine->infile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    }
    else
    {
        input = open(cmdLine->infile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
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
            int pid, status;
            int pfd[2];

            if (pipe(pfd) == -1) // create pipe
                perror("pipe");

            switch (pid = fork()) // first child: exec the command that generates output
            {
                case -1:
                    perror("fork");

                case 0:     // child
                    /*
                    if (close(pfd[0]) == -1)    // read end is unused
                        perror("close 1");
                    */
                    if (dup2(pfd[1], STDOUT_FILENO) == -1)
                        perror("dup2 1");
                    if (close(pfd[0]) == -1)
                        perror("close 1");
                    if (close(pfd[1]) == -1)
                        perror("close 2");
                    
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
                    /*
                    if (close(pfd[1] == -1))    // write end is unused
                        perror("close 3");
                    */

                    if (dup2(pfd[0], STDIN_FILENO) == -1)
                        perror("dup2 2");
                    if (close(pfd[1]) == -1)
                       perror("close 3"); 
                    if (close(pfd[0]) == -1)
                        perror("close 4");

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
        else
        {
            if (cmdLine.outfile != NULL)
                io_redir(&cmdLine, 0);
            else
            {
                switch (fork())
                {
                    case 0:
                        exec_command(&cmdLine, 0);

                    default:
                        wait(NULL);
                        break;
                }
            }
        }





        /* ------------------------------------------------------------------- */
        /*
        printf("%d: ", cmdLine.numCommands);

        if(cmdLine.infile)
            printf("< '%s' ", cmdLine.infile);

        for(i=0; i < cmdLine.numCommands; i++)
        {
            int j;
            for(j=cmdLine.cmdStart[i]; cmdLine.argv[j] != NULL; j++)
                printf("'%s' ", cmdLine.argv[j]);
            if(i < cmdLine.numCommands - 1)
                printf("| ");
        }

        if(cmdLine.append)
        {
            // verify that if we're appending there should be an outfile!
            assert(cmdLine.outfile);
            printf(">");
        }
        if(cmdLine.outfile)
            printf(">'%s'", cmdLine.outfile);

        
        // Print any other relevant info here, such as '&' if you implement
        // background execution, etc.
        

        printf("\n");
        
        
        printf("%d: ", cmdLine.numCommands);
        */

        /* ------------------------------------------------------------------- */
        if(input == stdin)
        {
            printf("? ");
            fflush(stdout);
        }
    }

    return 0;
}
