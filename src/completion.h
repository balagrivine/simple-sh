#ifndef COMPLETION_H
#define COMPLETION_H

#include <stdio.h>
#include <readline/readline.h>

char **simple_sh_completion(const char *text, int start, int end);

#endif /* COMPLETION_H */
