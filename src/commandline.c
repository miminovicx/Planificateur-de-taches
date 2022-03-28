#include "commandline.h"

/*
 * Builds a commandline from the arguments, the number of args of the cmdline
                                            the command to exec and its args,
    and returns a pointer to it.
 */
commandline *build_commandline(int n_args, char * argv[])
{
    commandline *cmdline; //a pointer to the commandline to return

    //number of args must be at least 1
    // argv[0] representing the name of the command to call must not be empty
    if((n_args < 1))
    {
        perror("Nombre d'arguments pour la cmdline insuffisant");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[0],"") == 0)
    {
      perror("cmd vide pour la cmdline");
      exit(EXIT_FAILURE);
    }
    

    cmdline = malloc(sizeof(commandline));
    if(cmdline == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }



    cmdline -> argc = (uint32_t)n_args;
    cmdline -> argv = string_tab_from_char_tab(argv, n_args);
    return cmdline;
}

/*
 * Frees all the memory allocated to the commandline.
 */
void free_commandline(commandline *cmdl)
{
  free_string_tab(cmdl -> argv, cmdl -> argc);
  free(cmdl);
}
