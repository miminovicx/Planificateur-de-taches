#ifndef STRING_TYPE_H
#define STRING_TYPE_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct string_type {
  uint32_t length; // length of the string (without the ending \0)
  char *content; // content of the string
} string;

/*
 * Builds a string from the argument content and returns a pointer to it.
 */
string *build_string(const char *content);

/*
 * Frees all the memory allocated to the argument s.
 */
void free_string(string *s);

/*
 * Frees all the memory allocated to a tab of strings  
 */
void free_string_tab(string **stab, int size);

/*
 * Returns a string pointer from a char tab
 */
string **string_tab_from_char_tab(char* tab[], int size);

/*
 * Allocates with malloc() a new buffer containing the concatenation of str1 and str2
 * and returns it. Returns NULL if malloc failed.
 */
char *concat_strings(const char *str1, const char *str2);

#endif // STRING_TYPE_H
