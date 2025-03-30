#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mini_shell.h"
#include "string_aux.h"
#include "slist.h"

#define SPACE_SEPARATOR " "
#define ARROW_SEPARATOR ">"
#define PIPE_SEPARATOR "|"


int main()
{
    while (1) {
        fprintf(stdout, "> ");

        //Read command.
        char *fullCommand = get_string(10);

        //Check for exit command.
        if (strcmp(fullCommand, "exit") == 0)
            break;

        int hasPipe = find_char(fullCommand, '|');
        int hasRedirect = find_char(fullCommand, '>');
        pid_t id;

        if (hasRedirect) {
            if (hasPipe == 1) {
                fprintf(stderr, "Cannot pipe and use redirection at the same time\n");
                exit(-1);
            }

            char *command = strtok(fullCommand, ARROW_SEPARATOR);
            clean_string(command);
            char *filename = strtok(NULL, ARROW_SEPARATOR);
            clean_string(filename);
            int fileFd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);

            if (fileFd == (-1)) {
                fprintf(stderr, "\"%s\" is an invalid filename\n", filename);
                exit(-1);
            }

            id = fork();

            if (id == 0)
                executeCommand(command, -1, fileFd);
            else
                wait(NULL);
        }
        //There's pipe but no redirection.
        else if (hasPipe) {
            SList *pipeCommands = slist_create();
            char *token;

            //Add first element.
            char *firstElem = strtok(fullCommand, PIPE_SEPARATOR);
            clean_string(firstElem);
            pipeCommands = slist_add_end(pipeCommands, firstElem);

            //Add the rest of the commands.
            while ((token = strtok(NULL, PIPE_SEPARATOR)) != NULL) {
                clean_string(token);
                pipeCommands = slist_add_end(pipeCommands, token);
            }

            pipeExecutor(pipeCommands);
            //Convert commands to string list.
        }
        //No pipe or redirection.
        else {
            id = fork();

            if (id == 0) {
                executeCommand(fullCommand, -1, -1);
            }
            else
                wait(NULL);
        }
    }

    fprintf(stdout, "\nExited shell\n");

    return 0;
}


void pipeExecutor(SList *commands) {
    size_t commandCount = slist_length(commands);
    int pipefd[commandCount - 1][2];
    pid_t p;

    SNode *node = commands->firstNode;
    for (size_t i = 0; i < commandCount; i++, node = node->next) {
        //Check we are not in the last command.
        if (i < commandCount - 1)
            //Create pipe and check for errors.
            if (pipe(pipefd[i]) == -1) {
                fprintf(stderr, "FATAL ERROR: Could not create pipe\n");
                exit(-1);
            }

        p = fork();

        if (p == 0) {
            //We cannot use the commodities provided by "executeCommand", because leaving unclosed pipes leads to errors.
            if (i > 0) {
                dup2(pipefd[i - 1][0], STDIN_FILENO);
                close(pipefd[i - 1][0]);
            }
            if (i < commandCount - 1) {
                dup2(pipefd[i][1], STDOUT_FILENO);
                close(pipefd[i][1]);
            }
            
            for (size_t j = 0; j < commandCount - 1; j++) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }
            /*int inFd = i > 0 ? pipefd[i - 1][0] : -1;
            int outFd = i < commandCount - 1 ? pipefd[i][1] : -1;*/
            executeCommand(node->value, -1, -1);
        }
    }

    //Close all unused pipes since we are the parent..
    for (size_t i = 0; i < commandCount - 1; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    //Wait until all children finish executing
    while (wait(NULL) > 0);
}


void executeCommand(char *command, const int in, const int out) {
    if (in > 0)
        //Check if filename is valid.
        if (dup2(in, 0) == (-1)) {
            fprintf(stderr, "FATAL ERROR: Invalid FD for dup2\n");
            exit(-1);

        close(in);
    }

    if (out > 0)
        //Check if filename is valid.
        if (dup2(out, 1) == (-1)) {
            fprintf(stderr, "FATAL ERROR: Invalid FD for dup2\n");
            exit(-1);

        close(out);
    }

    char *name = strtok(command, SPACE_SEPARATOR);

    SList *arguments = slist_create();
    char *token;

    //The first argument should be the filename of the executable.
    arguments = slist_add_end(arguments, name);

    //Store all arguments in the argument list.
    while ((token = strtok(NULL, SPACE_SEPARATOR)) != NULL)
                arguments = slist_add_end(arguments, token);

    //According to the documentation the last element in the argument list must be a null terminator.
    char **arguments_array = slist_to_list(arguments);

    //Execute command with given arguments.
    execvp(name, arguments_array);

    //If execution has not been transferred to the command, show an error.
    fprintf(stderr, "Invalid command \"%s\"\n", name);
    exit(-1);
}