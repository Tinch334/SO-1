#ifndef __STRING_AUX_H__
#define __STRING_AUX_H__

#include <stddef.h>

//Gets a string using the standard input, returns a pointer to it.
char *get_string(size_t initial_length);

//Removes all trailing and leading whitespace from the given string.
void clean_string(char *string);

//If the given character is found returns 1, otherwise returns -1.
size_t find_char(char *str, char c);

#endif /* __STRING_AUX_H__ */