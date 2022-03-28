#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include <stdint.h>
#include <unistd.h>
#include <mac-comp.h>
//#include <endian.h>
#include "timing.h"
#include "string_type.h"
#include "commandline.h"

#define CLIENT_REQUEST_LIST_TASKS 0x4c53              // 'LS'
#define CLIENT_REQUEST_CREATE_TASK 0x4352             // 'CR'
#define CLIENT_REQUEST_REMOVE_TASK 0x524d             // 'RM'
#define CLIENT_REQUEST_GET_TIMES_AND_EXITCODES 0x5458 // 'TX'
#define CLIENT_REQUEST_TERMINATE 0x544d               // 'TM'
#define CLIENT_REQUEST_GET_STDOUT 0x534f              // 'SO'
#define CLIENT_REQUEST_GET_STDERR 0x5345              // 'SE'

// MAIN FUNCTIONS

/*
 * Writes opcode in the request pipe.
 * Returns 0 on success, -1 on failure.
 */
int write_opcode(uint16_t opcode, int request_pipe_fd);

/*
 * Writes taskid in the request pipe.
 * Returns 0 on success, -1 on failure.
 */
int write_taskid(uint64_t taskid, int request_pipe_fd);

/*
 * Writes p_timing in the request pipe.
 * Returns 0 on success. -1 on failure.
 */
int write_timing(timing *p_timing, int request_pipe_fd);

/*
 * Writes p_cmdl in the request pipe.
 * Returns 0 on success. -1 on failure.
 */
int write_commandline(commandline *p_cmdl, int request_pipe_fd);

// HELPER FUNCTIONS

/*
 * Writes p_string in the request pipe.
 * Returns 0 on success. -1 on failure.
 */
int write_string(string *p_string, int request_pipe_fd);

#endif // CLIENT_REQUEST_H
