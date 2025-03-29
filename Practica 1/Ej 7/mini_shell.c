#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "mini_shell.h"
#include "string_aux.h"
#include "slist.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARG_COUNT 16
#define SPACE_SEPARATOR " "
#define ARROW_SEPARATOR ">"


int main()
{
    while (1) {
        fprintf(stdout, "> ");

        //Read command.
        char *fullCommand = get_string(10);

        //Check for exit command.
        if (strcmp(fullCommand, "exit") == 0)
            break;

        pid_t id = fork();

        if (id == 0) {
            char *command = strtok(fullCommand, ARROW_SEPARATOR);
            clean_string(command, strlen(command));
            char *filename = strtok(NULL, ARROW_SEPARATOR);

            if (filename != NULL) {
                clean_string(filename, strlen(filename));

                //Check if filename is valid.
                if (freopen(filename, "w", stdout) == NULL) {
                    fprintf(stderr, "\"%s\" is not a valid path\n", filename);
                    exit(-1);
                }
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
        else {
            //Wait until called child finishes.
            wait(NULL);
        }
    }

    fprintf(stdout, "\nExited shell\n");

    return 0;
}