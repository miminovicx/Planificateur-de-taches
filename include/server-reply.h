#ifndef SERVER_REPLY_H
#define SERVER_REPLY_H

#define SERVER_REPLY_OK 0x4f4b     // 'OK'
#define SERVER_REPLY_ERROR 0x4552  // 'ER'

#define SERVER_REPLY_ERROR_NOT_FOUND 0x4e46  // 'NF'
#define SERVER_REPLY_ERROR_NEVER_RUN 0x4e52  // 'NR'

#include <unistd.h>
#include <stdint.h>
#include "timing.h"
#include "timing-text-io.h"
#include "string_type.h"
#include "commandline.h"
#include <time.h>

/*
 * Reads uint16_t (like reptype) and returns it in case of success
 * Returns -1 on failure
 */
uint16_t read_uint16(int reply_pipe_fd);

/*
 * Reads uint32_t and returns it in case of success
 * Returns -1 on failure
 */
uint32_t read_uint32(int reply_pipe_fd);

/*
 * Reads uint64_t and returns it in case of success
 * Returns -1 on failure
 */
uint64_t read_uint64(int reply_pipe_fd);

/*
 * Reads the timing from the reply pipe
 * Returns -1 if failure and 0 if success 
 */
int read_timing(char* timing_read, int reply_pipe_fd);

/*
 * Reads the commandline from the reply pipe and prints it on stdout
 * Returns -1 if failure and 0 if success 
 */
int read_display_commandline(char* cmdline,int reply_pipe_fd);

/*
 * Reads the tasks from the reply pipe and prints it on stdout
 * Returns 0 if success and -1 on failure
 */
int read_display_tasks(uint32_t nb_tasks,int reply_pipe_fd);

/*
 * Reads the time and exitcode from the reply pipe
 * Return 0 on the success case and -1 on failure 
 */
int read_time_and_exitcode(uint32_t nb_runs, int reply_pipe_fd);

#endif // SERVER_REPLY_H
