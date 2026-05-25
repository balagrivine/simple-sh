#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "builtin.h"
#include "completion.h"
#include "prompt.h"

int tokenize(char *command, char **tokens);
void register_interrupt_signal_handler(void);
void sigint_handler(int sig);
void execute_readline_command(char *command);
static sigjmp_buf sigint_env;
int initialize_shell();

int
main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (initialize_shell() != 0){
        perror("failed to initialize shell");
        exit(1);
    }

    register_interrupt_signal_handler();
    rl_attempted_completion_function = simple_sh_completion;

    while (1){

        if (sigsetjmp(sigint_env, 1) != 0) {
            write(STDOUT_FILENO, "\n", 1);
        }

        char *command = readline(prompt());
        if (command == NULL) {
            break;
        }

        if (strcmp(command, "exit") == 0) {
            free(command);
            break;
        }

        if (*command != '\0') {
            add_history(command);
        }

        execute_readline_command(command);

        free(command);
    }

    return 0;
}

void execute_readline_command(char *command){
    if (handle_builtin(command) == 0){
        return;
    }

    int rc = fork();
    if (rc < 0) {
        perror("fork");
        exit(1);
    } else if (rc == 0) {
        // Reset the signal disposition in the child process.
        // From the signal man page -> https://man7.org/linux/man-pages/man7/signal.7.html,
        // the dispositions of handled signals are reset to the default;
        // the dispositions of ignored signals are left unchanged. Since we ignore `SIGINT`
        // signals, the child process will inherit that and cause issues later on.
        signal(SIGINT, SIG_DFL);

        char *tokens[100];

        int num_tokens = tokenize(command, tokens);
        if (num_tokens == 0) {
            exit(0);
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

int
tokenize(char *command, char **tokens)
{
    int i = 0;
    char *p = command;

    while (*p) {
        while (isspace((unsigned char)*p)) {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        if (*p == '"') {
            p++;

            tokens[i++] = p;

            while (*p && *p != '"') {
                p++;
            }

            if (*p == '"') {
                *p = '\0';
                p++;
            }
        } else {
            tokens[i++] = p;

            while (*p && !isspace((unsigned char)*p)) {
                p++;
            }

            if (*p) {
                *p = '\0';
                p++;
            }
        }
    }

    tokens[i] = NULL;

    return i;
}

//register_interrupt_signal_handler registers the desired signal handler
// for SIGINT(CTRL-C). By default, the behaviour is to ignore interrupt
// signals when in the parent shell process.
void
register_interrupt_signal_handler(void){
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}

void
sigint_handler(int sig){
    (void)sig;
    rl_free_line_state();
    siglongjmp(sigint_env, 1);
}

int initialize_shell(void){
    return setenv("CLICOLOR", "1", 0);
}
