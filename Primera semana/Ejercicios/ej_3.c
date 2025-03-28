#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARG_COUNT 16
#define SEPARATOR " "

int main()
{
    while (1) {
        fprintf(stdout, "> ");

        //Read command.
        char command[MAX_COMMAND_LENGTH + 1];
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        //Remove read newline
        command[strlen(command) - 1] = '\0';

        //Check for exit command.
        if (strcmp(command, "exit") == 0)
            break;

        pid_t command_id = fork();

        if (command_id == 0) {
            char *name = strtok(command, SEPARATOR);

            char *arguments[MAX_ARG_COUNT];
            size_t arg_counter = 1;
            char *token;

            //The first argument should be the filename of the executable.
            arguments[0] = strdup(name);

            //Store all arguments in the argument list.
            while (token = strtok(NULL, SEPARATOR)) {
                arguments[arg_counter] = strdup(token);
                arg_counter++;

                //Check there's space for the null terminator.
                if (arg_counter >= MAX_ARG_COUNT - 1) {
                    fprintf(stderr, "Maximum argument amount exceeded\n");
                    exit(-1);
                }
            }

            //According to the documentation the last element in the argument list must be a null terminator.
            arguments[arg_counter] = NULL;
            char final_command[MAX_COMMAND_LENGTH];

            if (name[0] == '.') {
                final_command = strdup(name);
            }
            else {
                //Assemble final command.
                final_command = strdup("/bin/");
                strcat(final_command, name);
            }


            execv(final_command, arguments);

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