#ifndef CASSINI_H
#define CASSINI_H

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "client-request.h"
#include "server-reply.h"
#include "timing-text-io.h"
#include "commandline.h"

/*
 * Allocates with malloc() a new buffer containing the concatenation of str1 and str2
 * and returns it. Returns NULL if malloc failed.
 */
char *concat_strings(const char *str1, const char *str2);

#endif // CASSINI
