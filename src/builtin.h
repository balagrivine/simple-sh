#ifndef __BUILTIN_H__
#define __BUILTIN_H__

/*
 * handle_builtin
 * provides a generalized interface ot handle and builtin command
 * @param command the entire tokenized user input
 * return 0 on sucess; -1 on error
 */
int handle_builtin(char *commands);

/*
 * handle_chdir_builtin
 * handler function for `cd` command
 * @param command the entire tokenized user input
 * return 0 on sucess; -1 on error
 */
int handle_chdir_builtin(const char *command);

/*
 * handle_export_builtin
 * handler function for `export` command
 * @param command the entire tokenized user input
 * return 0 on sucess; -1 on error
 */
int handle_export_builtin(char *command);

/*
 * handle_echo_builtin
 * handler function for `echo` command, it's special because it handles
 * only printing of exported variables
 * @param command user input
 * return 0 on sucess; -1 on error
 */
int handle_echo_builtin(char *command);

/*
 * handle_source_builtin
 * handler function for `source` command
 * @param command is a line of exported variable from the enf file
 * return 0 on sucess; -1 on error
 */
int handle_source_builtin(char *command);

#endif /* __BUILTIN_H__ */
