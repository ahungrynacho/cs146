nsh SHELL DOCUMENTATION
BRIAN HUYNH 57641580

FILE DEFINITIONS
File with bash test cases: test.sh
File with bash output: bash.out
File with nsh output: nsh.out

SAMPLE EXECUTION
1. Run $ ./test.sh > bash.out
2. Run $ nsh test.sh > nsh.out
3. diff nsh.out bash.out

The only differences you should see are for files where IO redirection operators 
like > and >> overwrite or append new lines to a file and the increased file sizes.

BUGS
1. The shell only supports 1 pipe.
2. The < IO operator only works in the first command in the case where there 
are 2 commands and 1 pipe.
3. The >> and > IO operator only work in the last command in the case where there 
are 2 commands and 1 pipe.
