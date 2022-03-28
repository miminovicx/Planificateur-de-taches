#include "client-request.h"

/*
 * Writes opcode in the request pipe.
 * Returns 0 on success, -1 on failure.
 */
int write_opcode(uint16_t opcode, int request_pipe_fd){
  uint16_t opcode_endian = htobe16(opcode);
  if(write(request_pipe_fd, &opcode_endian, sizeof(opcode)) < 0){
    perror("write");
    return -1;
  }
  return 0;
}

/*
 * Writes taskid in the request pipe.
 * Returns 0 on success, -1 on failure.
 */
int write_taskid(uint64_t taskid, int request_pipe_fd){
  uint64_t taskid_endian = htobe64(taskid);
  if(write(request_pipe_fd, &taskid_endian, sizeof(taskid)) < 0){
    perror("write");
    return -1;
  }
  return 0;
}

/*
 * Writes p_timing in the request pipe.
 * Returns 0 on success. -1 on failure.
 */
int write_timing(timing *p_timing, int request_pipe_fd){
  uint64_t min_endian;
  uint32_t h_endian;
  uint8_t d;
  
  if(p_timing == NULL){
    perror("NULL argument");
    return -1;
  }

  min_endian = htobe64(p_timing->minutes);
  h_endian = htobe32(p_timing->hours);
  d = p_timing->daysofweek;
  
  if(write(request_pipe_fd, &min_endian, sizeof(min_endian)) < 0
     || write(request_pipe_fd, &h_endian, sizeof(h_endian)) < 0
     || write(request_pipe_fd, &d, sizeof(d)) < 0){
    perror("write");
    return -1;
  }
  return 0;
}

/*
 * Writes p_string in the request pipe.
 * Returns 0 on success. -1 on failure.
 */
int write_string(string *p_string, int request_pipe_fd){
  if(p_string == NULL)
    return -1;

  uint32_t length_endian = htobe32(p_string->length);
  
  if(write(request_pipe_fd, &length_endian, sizeof(length_endian)) < 0
     || write(request_pipe_fd, p_string->content, p_string->length) < 0){
    perror("write");
    return -1;
  }
  return 0;
}

/*
 * Writes p_cmdl in the request pipe.
 * Returns 0 on success. -1 on failure.
 */
int write_commandline(commandline *p_cmdl, int request_pipe_fd){
  if(p_cmdl == NULL)
    return -1;

  uint32_t argc_endian = htobe32(p_cmdl->argc);
  if(write(request_pipe_fd, &argc_endian, sizeof(argc_endian)) < 0){
    perror("write");
    return -1;
  }
  
  for(int i = 0; i < p_cmdl->argc; i++){
    if(write_string(p_cmdl->argv[i], request_pipe_fd) < 0)
      return -1;
  }
  return 0;

}
