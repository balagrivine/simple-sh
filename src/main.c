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
#include "redirect.h"
#include "pipe.h"

int tokenize(char *command, char **tokens);
void register_interrupt_signal_handler(void);
void sigint_handler(int sig);
static sigjmp_buf sigint_env;
static volatile sig_atomic_t readline_active = 0;
int initialize_shell();
int split_pipe_subcommands(char *command, char **subcommands);
void handle_command(char *command);
void execute_readline_command(char *command);

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
            // Reap any zombie left by a Ctrl-C interrupted child.
            waitpid(-1, NULL, WNOHANG);
            if (readline_active) {
                rl_reset_after_signal();
                rl_crlf();
            } else {
                write(STDOUT_FILENO, "\n", 1);
            }
        }

        readline_active = 1;
        char *command = readline(prompt());
        readline_active = 0;
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

        handle_command(command);

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

        if (setup_redirect(command) < 0) {
            exit(1);
        }

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
        waitpid(rc, NULL, 0);
    }
}

void handle_command(char *command){
    char *subcommands[128];

    int num_cmds = split_pipe_subcommands(command, subcommands);
    if (num_cmds == 0){
        execute_readline_command(command);
        return;
    }

    int prev_fd = -1;
    pid_t pids[128];

    for (int i = 0; i < num_cmds; i++) {
        int pipefd[2] = {-1, -1};
        int is_last = (i == num_cmds - 1);

        if (!is_last && setup_pipe(pipefd) < 0) {
            exit(1);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            signal(SIGINT, SIG_DFL);

            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (!is_last) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }

            if (setup_redirect(subcommands[i]) < 0) exit(1);
            char *tokens[100];
            int n = tokenize(subcommands[i], tokens);
            if (n == 0) exit(0);

            execvp(tokens[0], tokens);
            perror(tokens[0]);
            exit(1);
        }

        pids[i] = pid;
        if (prev_fd != -1) close(prev_fd);
        if (!is_last) {
            close(pipefd[1]);
            prev_fd = pipefd[0];
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
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

int
split_pipe_subcommands(char *command, char **subcommands){
    int count = 0;
    char *p = command;

    if (strchr(command, '|') == NULL){
        return 0;
    }

    subcommands[count++] = p;

    while (*p) {
        if (*p == '|') {
            *p = '\0';
            p++;

            while (isspace((unsigned char)*p)) {
                p++;
            }

            subcommands[count++] = p;
        } else {
            p++;
        }
    }

    subcommands[count] = NULL;

    return count;
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
    if (readline_active) {
        rl_free_line_state();
        rl_reset_line_state();
    }
    siglongjmp(sigint_env, 1);
}

int initialize_shell(void){
    setenv("CLICOLOR", "1", 0);
    setenv("INPUTRC", "simpleshrc", 0);

    return 0;
}
