#include "string_type.h"

/*
 * Builds a string from the argument content and returns a pointer to it.
 */
string *build_string(const char *content){
  string *p_string; // a pointer to the string field to return
  size_t l; // length of content

  // allocate memory for the pointer to the string and its content field
  p_string = malloc(sizeof(string));
  if(p_string == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  l = strlen(content);
  p_string->content = malloc((l + 1) * sizeof(char));
  if(p_string->content == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  //assign values
  p_string->length = l;
  p_string->content = strcpy(p_string->content, content);

  return p_string;
}

/*
 * Frees all the memory allocated to the argument s.
 */
void free_string(string *s){
  free(s->content);
  free(s);
}

/*
 * Frees all the memory allocated to a tab of strings  
 */
void free_string_tab(string **stab, int size)
{
  for(int i = 0; i < size; i++)
  {
    free_string(stab[i]);
  }

  free(stab);
}

/*
 * Returns a string pointer from a char tab
 */
string **string_tab_from_char_tab(char* tab[], int size)
{
  string** pp_string = malloc(sizeof(string) * size);
  if(pp_string == NULL)
  {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for(int i = 0; i < size; i++)
  {
    pp_string[i] = build_string(tab[i]);  
  }

  return pp_string;
}

/*
 * Allocates with malloc() a new buffer containing the concatenation of str1 and str2
 * and returns it. Returns NULL if malloc failed.
 */
char *concat_strings(const char *str1, const char *str2){
  int length;
  char *buf;
  
  length = strlen(str1) + strlen(str2) + 1;
  buf = malloc(length * sizeof(char));
  if(buf == NULL){
    return NULL;
  }
  
  buf = strcpy(buf, str1);
  buf = strcat(buf, str2);
  return buf;
}

