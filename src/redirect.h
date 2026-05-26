#ifndef __REDIRECT_H__
#define __REDIRECT_H__

// setup_redirect
// Sets up output redirection by making STDOUT_FILENO refer
// to the same open file description as the redirect file.
// @param command user input
// returns 0 on success, -1 on error
int setup_redirect(char *command);

# endif /* __REDIRECT_H__ */
