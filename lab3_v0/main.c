#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    while (1) {
        printf("Your command> ");
        
        char line[100];
        int ch, i = 0;

        // read one line from stdin, char by char
        // stop on new line or EOF, cap at 99 chars
        while ((ch = getchar()) != '\n' && ch != EOF) {
            if (i < 99) line[i++] = (char)ch;
        }

        // null terminate input buffer
        line[i] = '\0';

        // ignore empty lines
        if (i == 0)
            continue;

        // background execution flag
        int wait_for_child = 1;
        if (line[i - 1] == '&') {
            wait_for_child = 0;
            line[i - 1] = '\0';   // strip '&' from command
        }

        // build argv[] by splitting on spaces
        char *argv[64];
        int argc = 0;

        char *p = line;
        while (*p) {
            // skip leading spaces between args
            while (*p == ' ')
                p++;

            if (!*p)
                break;

            // argv points directly into line buffer
            argv[argc++] = p;

            // advance to end of this word
            while (*p && *p != ' ')
                p++;

            // terminate this argument
            if (*p == ' ')
                *p++ = '\0';

            // leave room for NULL terminator
            if (argc >= 63)
                break;
        }

        argv[argc] = NULL;

        if (argc == 0)
            continue;

        pid_t pid = fork();

        if (pid == 0) {
            // child: replace process image with requested command
            execvp(argv[0], argv);

            // only reached on error
            perror("execvp failed");
        } else if (pid > 0) {
            // parent: wait unless background execution requested
            if (wait_for_child) {
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            // pid < 0 implies fork failed
            perror("fork failed");
        }
    }

    return 0;
}