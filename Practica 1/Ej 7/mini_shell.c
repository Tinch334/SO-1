#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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
            puts("Redirect");
            fflush(stdout);

            if (hasPipe == 1) {
                fprintf(stderr, "Cannot pipe and use redirection at the same time\n");
                exit(-1);
            }

            char *command = strtok(fullCommand, ARROW_SEPARATOR);
            clean_string(command, strlen(command));
            char *filename = strtok(NULL, ARROW_SEPARATOR);
            clean_string(filename, strlen(filename));

            id = fork();

            if (id == 0)
                executeCommand(command, NULL, filename);
            else
                wait(NULL);
        }
        //There's pipe but no redirection.
        else if (hasPipe) {
            puts("Pipe");
            fflush(stdout);

            SList *pipeCommands = slist_create();
            char *token;

            //Add first element.
            pipeCommands = slist_add_end(pipeCommands, strtok(fullCommand, PIPE_SEPARATOR));

            while ((token = strtok(NULL, PIPE_SEPARATOR)) != NULL) {
                clean_string(token, strlen(token));
                pipeCommands = slist_add_end(pipeCommands, token);
            }
        }
        //No pipe or redirection.
        else {
            id = fork();

            if (id == 0) {
                executeCommand(fullCommand, NULL, NULL);
            }
            else
                wait(NULL);
        }
    }

    fprintf(stdout, "\nExited shell\n");

    return 0;
}


void executeCommand(char *command, const char *in, const char *out) {
    if (in != NULL)
        //Check if filename is valid.
        if (freopen(in, "r", stdin) == NULL) {
            fprintf(stderr, "\"%s\" is not a valid path\n", in);
            exit(-1);
    }

    if (out != NULL)
        //Check if filename is valid.
        if (freopen(out, "w", stdout) == NULL) {
            fprintf(stderr, "\"%s\" is not a valid path\n", out);
            exit(-1);
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