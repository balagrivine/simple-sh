#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

int tokenize(char *command, char **tokens);
void register_interrupt_signal_handler(void);
void sigint_handler(int sig);

int
main(int argc, char *argv[])
{
    register_interrupt_signal_handler();

    while (1){

        char *command = readline("simple-shell$ ");
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

        free(command);
    }

    return 0;
}

int
tokenize(char *command, char ** tokens)
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
    putchar('\n');
}
