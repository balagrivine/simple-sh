#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "completion.h"

static const char *git_subcommands[] = {
    "add", "bisect", "branch", "checkout", "cherry-pick", "clone",
    "commit", "diff", "fetch", "init", "log", "merge", "pull",
    "push", "rebase", "reset", "restore", "revert", "show",
    "stash", "status", "switch", "tag", NULL
};

static char *git_subcommand_generator(const char *text, int state) {
    static int idx;
    if (state == 0) idx = 0;

    const char *cmd;
    while ((cmd = git_subcommands[idx++]) != NULL) {
        if (strncmp(cmd, text, strlen(text)) == 0)
            return strdup(cmd);
    }
    return NULL;
}

static char *git_branch_generator(const char *text, int state) {
    static FILE *fp;
    static char line[256];

    if (state == 0) {
        if (fp) { pclose(fp); fp = NULL; }
        fp = popen("git branch --format='%(refname:short)' 2>/dev/null", "r");
        if (!fp) return NULL;
    }

    while (fp && fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line, text, strlen(text)) == 0)
            return strdup(line);
    }

    if (fp) { pclose(fp); fp = NULL; }
    return NULL;
}

static const char *get_git_subcommand(const char *line) {
    static char subcmd[64];
    const char *rest = line + 4;  // skip "git "
    size_t i = 0;
    while (rest[i] && rest[i] != ' ' && i < sizeof(subcmd) - 1) {
        subcmd[i] = rest[i];
        i++;
    }
    subcmd[i] = '\0';
    return subcmd;
}

char **simple_sh_completion(const char *text, int start, int end) {
    (void)end;

    if (start == 0) return NULL;

    const char *line = rl_line_buffer;
    if (strncmp(line, "git ", 4) != 0) return NULL;

    // Check if any space exists between "git " and the word being completed.
    // If not, we are still completing the subcommand itself.
    int completing_arg = 0;
    for (int i = 4; i < start; i++) {
        if (line[i] == ' ') { completing_arg = 1; break; }
    }

    if (!completing_arg) {
        rl_attempted_completion_over = 1;
        return rl_completion_matches(text, git_subcommand_generator);
    }

    // Completing an argument — use the subcommand to pick the right generator.
    const char *subcmd = get_git_subcommand(line);
    if (strcmp(subcmd, "checkout") == 0 ||
        strcmp(subcmd, "switch")   == 0 ||
        strcmp(subcmd, "merge")    == 0 ||
        strcmp(subcmd, "rebase")   == 0 ||
        strcmp(subcmd, "diff")     == 0 ||
        strcmp(subcmd, "branch")   == 0) {
        rl_attempted_completion_over = 1;
        return rl_completion_matches(text, git_branch_generator);
    }

    // For subcommands like add/restore/rm, fall through to default file completion.
    return NULL;
}
