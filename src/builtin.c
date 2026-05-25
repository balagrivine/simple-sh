#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtin.h"

int handle_builtin(char *command){
    if (!command) return -1;

    if (strncmp(command, "cd ", 3) == 0) {
        return handle_chdir_builtin(command);
    } else if (strncmp(command, "export ", 7) == 0) {
        return handle_export_builtin(command);
    } else if (strncmp(command, "echo ", 5) == 0) {
        return handle_echo_builtin(command);
    }else if (strncmp(command, "source ", 7) == 0) {
        return handle_source_builtin(command);
    }

    return -1;
}

int handle_chdir_builtin(const char *command) {
    if (!command) {
        return -1;
    }

    const char *relative_path = command + 3;

    char *current_dir = getcwd(NULL, 0);
    char absolute_path[1024];

    snprintf(absolute_path, sizeof(absolute_path), "%s/%s", current_dir, relative_path);
    free(current_dir);

    return chdir(absolute_path);

    return -1;
}

int handle_export_builtin(char *command) {
    if (!command) {
        return -1;
    }

    char *env = command + strlen("export ");

    char *equal = strchr(env, '=');
    if (!equal) {
        fprintf(stderr, "export: Invalid syntax\n");
        return 0;
    }

    *equal = '\0';

    char *key = env;
    char *value = equal + 1;

    return setenv(key, value, 1);
}

int handle_echo_builtin(char *command) {
    if (!command) {
        return -1;
    }

    char *dollar = strchr(command, '$');
    if (!dollar) {
        return -1;
    }

    char *key = dollar + 1;

    char *value =  getenv(key);
    printf("%s\n", value);

    return 0;
}

int handle_source_builtin(char *command)
{
    if (!command) {
        return -1;
    }

    char *filename = command + strlen("source ");

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("source");
        return -1;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0) {
            continue;
        }

        if (strncmp(line, "export ", 7) == 0) {
            handle_export_builtin(line);
        }
    }

    free(line);
    fclose(fp);

    return 0;
}
