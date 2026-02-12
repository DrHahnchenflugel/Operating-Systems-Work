#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE     1024
#define MAX_ARGS     64
#define MAX_PIPES    64
#define MAX_CMDS     (MAX_PIPES + 1)

/* trim leading + trailing spaces in-place, return pointer to first non-space */
static char *trim(char *s) {
    while (*s == ' ') s++;
    if (*s == '\0') return s;

    char *end = s + strlen(s) - 1;
    while (end >= s && *end == ' ') {
        *end = '\0';
        end--;
    }
    return s;
}

/* build argv from a space-separated string. modifies string in-place */
static int build_argv(char *s, char *out_argv[], int max_args) {
    int argc = 0;
    char *p = s;

    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;

        if (argc >= max_args - 1) break;

        out_argv[argc++] = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') *p++ = '\0';
    }

    out_argv[argc] = NULL;
    return argc;
}

/* split line into command segments by '|', up to MAX_CMDS; modifies line in-place */
static int split_pipeline(char *line, char *cmds[], int max_cmds) {
    int n = 0;
    char *p = line;

    while (1) {
        if (n >= max_cmds) return -1;

        cmds[n++] = p;

        char *bar = strchr(p, '|');
        if (!bar) break;

        *bar = '\0';
        p = bar + 1;
    }

    return n;
}

int main(void) {
    while (1){
        printf("Your command> ");
        fflush(stdout);

        char line[MAX_LINE];
        int c, i = 0;

        while ((c = getchar()) != '\n' && c != EOF && i < MAX_LINE - 1) {
            line[i++] = (char)c;
        }
        line[i] = '\0';

        if (c == EOF) return 0;
        if (i == 0) return 0;

        // Split into commands by '|'
        char *cmd_strs[MAX_CMDS];
        int n_cmds = split_pipeline(line, cmd_strs, MAX_CMDS);
        if (n_cmds < 0) {
            fprintf(stderr, "Error: too many pipes (max %d).\n", MAX_PIPES);
            return 1;
        }

        // Trim + validate each command segment, and build argv arrays
        char *argvs[MAX_CMDS][MAX_ARGS];
        for (int k = 0; k < n_cmds; k++) {
            cmd_strs[k] = trim(cmd_strs[k]);
            if (cmd_strs[k][0] == '\0') {
                fprintf(stderr, "Error: empty command near pipe.\n");
                return 1;
            }

            int argc_k = build_argv(cmd_strs[k], argvs[k], MAX_ARGS);
            if (argc_k == 0) {
                fprintf(stderr, "Error: empty command near pipe.\n");
                return 1;
            }
        }

        // 0 pipes case: just fork/exec one command
        if (n_cmds == 1) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                return 1;
            }
            if (pid == 0) {
                execvp(argvs[0][0], argvs[0]);
                perror("execvp failed");
                _exit(127); 
            }
            int status;
            waitpid(pid, &status, 0);
            continue;
        }

        // Create pipes: need n_cmds-1 pipes
        int pipes[MAX_PIPES][2];
        for (int p = 0; p < n_cmds - 1; p++) {
            if (pipe(pipes[p]) < 0) {
                perror("pipe failed");
                return 1;
            }
        }

        pid_t pids[MAX_CMDS];

        // Fork each command in pipeline
        for (int k = 0; k < n_cmds; k++) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                return 1;
            }

            if (pid == 0) {
                // If not first command, hook stdin to previous pipe read end
                if (k > 0) {
                    if (dup2(pipes[k - 1][0], STDIN_FILENO) < 0) {
                        perror("dup2 stdin failed");
                        _exit(1);
                    }
                }

                // If not last command, hook stdout to next pipe write end
                if (k < n_cmds - 1) {
                    if (dup2(pipes[k][1], STDOUT_FILENO) < 0) {
                        perror("dup2 stdout failed");
                        _exit(1);
                    }
                }

                // Close ALL pipe fds in child (avoid hangs)
                for (int p = 0; p < n_cmds - 1; p++) {
                    close(pipes[p][0]);
                    close(pipes[p][1]);
                }

                execvp(argvs[k][0], argvs[k]);
                perror("execvp failed");
                _exit(127);
            }

            // Parent
            pids[k] = pid;
        }

        // Parent closes all pipe fds
        for (int p = 0; p < n_cmds - 1; p++) {
            close(pipes[p][0]);
            close(pipes[p][1]);
        }

        // Wait for all children
        int status;
        for (int k = 0; k < n_cmds; k++) {
            waitpid(pids[k], &status, 0);
        }
    }
    return 0;
}