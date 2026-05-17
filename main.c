#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int tokenize(char *command, char **tokens);

int
main(int argc, char *argv[])
{
    char command[256];

    while (1){
        fputs("$", stdout);
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;
        }

        int rc = fork();
        if (rc < 0) {
            perror("fork");
            exit(1);
        } else if (rc == 0) {
            char *tokens[100];

            int num_tokens = tokenize(command, tokens);
            if (num_tokens == 0) {
                continue;
            }

            int ret = execvp(tokens[0], tokens);
            if (ret == -1) {
                perror(tokens[0]);
            }
            exit(ret);
        } else {
            wait(NULL);
        }
    }

    return 0;
}

int tokenize(char *command, char ** tokens)
{
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }
    tokens[i] = NULL;
    return i;
}
