#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <stdint.h>
#include "string_type.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct commandline {
  uint32_t argc; //the number of arguments
  string ** argv; //the arguments of the command
} commandline;

/*
 * Builds a commandline from the arguments, the number of args of the cmdline
 * the command to exec and its args, and returns a pointer to it.
 */
commandline *build_commandline(int n_args, char *argv[]);


/*
 * Frees all the memory allocated to the commandline.
 */
void free_commandline(commandline *cmdl);

#endif // COMMANDLINE_H
