#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    char command[100];

    while (1){
        fputs("$", stdout);
        fflush(stdout);

        fgets(command, 100, stdin);
        command[strlen(command) - 1] = '\0';
        if (strcmp(command, "exit") == 0) {
            break;
        }

        printf("%s\n", command);
    }
}
