# Simple Shell
As the name suggests, this is a simple shell implementation in C.

It supports basic shell commands such as `ls`, `cd`, `pwd`, and `exit` and many more. Shell builtins are not supported because I haven't found a way to implement them.

Support for pipes isn't supported yet, but will be added in the future.

## Motivation
While reading **Operating Systems: Three Easy Pieces**, I decided to implement a simple shell in C to better understand how process creation and execution work. A shell is a good practice project because it forces you to interact with the *fork* and *exec* system calls which are the fundamentals building blocks of process creation and execution.

## Build
### MacOS
Check if you have the readline library installed via Homebrew, or just install it directly
```shell
brew install readline
```

#### Compile & run
```shell
gcc main.c -o shell \
    -I/opt/homebrew/opt/readline/include \
    -L/opt/homebrew/opt/readline/lib \
    -lreadline && ./shell
```
