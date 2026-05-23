#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define GREEN "\001\033[32m\002"
#define RESET "\001\033[0m\002"

char *prompt(void) {
    static char prompt[1024];

    struct passwd *pw = getpwuid(getuid());
    const char *username = pw ? pw->pw_name : "unknown";

    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    const char *home = getenv("HOME");

    if (strncmp(cwd, home, strlen(home)) == 0){
        snprintf(
            prompt,
            sizeof(prompt),
            "%s%s@%s ~%s $ %s",
            GREEN,
            username,
            hostname,
            cwd + strlen(home),
            RESET
        );
    }

    return prompt;
}
