#include <stdio.h>  
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    while (1){
        // FLAGS
        char flag_waitForChildDeath = 1;

        /* INPUT */
        printf("Your command>");
        //fflush(stdout);

        /* INPUT CONVERSION */
        char line[100];
        int c, i = 0;

        while ((c = getchar()) != '\n' && i < 99){
            line[i++] = c;
        }
        line[i] = '\0'; //End line with null string terminator

        // If empty line, continue to next line
        if (i == 0) continue;

        // check optional '&' at end
        if (i > 0 && line[i-1] == '&') {
            flag_waitForChildDeath = 0;
            line[i-1] = '\0';
        }

        // Convert input string to argc, argv format
        char *argv[64];
        int argc = 0;
        char * p = line;

        //loop pointer p through the line
        while (*p) {    
            while (*p == ' ') p++; //skip spaces
            
            if (!*p) break; //end loop when pointer reaches end
            
            argv[argc++] = p;            //Add pointer to array of arg variables
            while (*p && *p != ' ') p++; //and loop through the line until it reaches a space (next arg)
            
            if (*p == ' ') *p++ = '\0'; //Add a null terminator to the end of each term.
            
            if (argc >= 63) break; //Break in cases of 64+ arguments
        }

        argv[argc] = NULL; // Set last argument to NULL, so we can detect last argument in list

        if (argc == 0) continue; // Do nothing if no arguments

        /* PROCESS EXECUTION */
        pid_t pid = fork();
        if (pid == 0) { // CHILD process
            execvp(argv[0], argv);
            perror("execvp failed");
            _exit(127); // Command not found
        } else if (pid > 0) { // PARENT process
            // Wait for child death, if flag set
            if (flag_waitForChildDeath) {
                int status;
                waitpid(pid, &status, 0); // Wait for the process ID
            }
        } else { // ERROR (pid < 0)
            perror("fork failed");
        }
    }
    return 0; //Could also say exit(exit_code)
}