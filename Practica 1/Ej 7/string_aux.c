#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string_aux.h"

char *get_string(size_t initial_length) {
    size_t length = initial_length;
    size_t index = 0;
    char *string = malloc(sizeof(char) * length);
    char c;

    while ((c = getchar()) != '\n') {
        string[index] = c;
        index++;

        //If string is too short allocate more space.
        if (c >= length - 1) {
            length *= 2;
            string = realloc(string, length);
        }
    }

    string[index] = '\0';

    return string;
}

void clean_string(char *string, size_t length) {
    //Remove trailing whitespace.
    size_t ptr = length - 1;
    for (; ptr >= 0 && string[ptr] == ' '; ptr--);
    string[ptr + 1] = '\0';
    
    //String has been modified, recalculate length.
    length = strlen(string);

    //Remove leading whitespace.
    ptr = 0;
    for (; ptr < length && string[ptr] == ' '; ptr++);
    if (ptr != 0) {
        memmove(string, string + ptr, length - ptr);
        string[length - 1] = '\0';
    }
}

size_t find_char(char *str, char c, size_t length) {
    for (size_t i = length - 1; i >= 0; i--)
        if (str[i] == c)
            return i;

    return -1;
}
