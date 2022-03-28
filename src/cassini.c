#include "cassini.h"

const char usage_info[] = "\
   usage: cassini [OPTIONS] -l -> list all tasks\n\
      or: cassini [OPTIONS]    -> same\n\
      or: cassini [OPTIONS] -q -> terminate the daemon\n\
      or: cassini [OPTIONS] -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
          -> add a new task and print its TASKID\n\
             format & semantics of the \"timing\" fields defined here:\n\
             https://pubs.opengroup.org/onlinepubs/9699919799/utilities/crontab.html\n\
             default value for each field is \"*\"\n\
      or: cassini [OPTIONS] -r TASKID -> remove a task\n\
      or: cassini [OPTIONS] -x TASKID -> get info (time + exit code) on all the past runs of a task\n\
      or: cassini [OPTIONS] -o TASKID -> get the standard output of the last run of a task\n\
      or: cassini [OPTIONS] -e TASKID -> get the standard error\n\
      or: cassini -h -> display this message\n\
\n\
   options:\n\
     -p PIPES_DIR -> look for the pipes in PIPES_DIR (default: /tmp/<USERNAME>/saturnd/pipes)\n\
";


int main(int argc, char * argv[]) {
  errno = 0;
  
  char * minutes_str = "*";
  char * hours_str = "*";
  char * daysofweek_str = "*";
  char * pipes_directory = NULL;
  char * request_pipe_name = "/saturnd-request-pipe";
  char * reply_pipe_name = "/saturnd-reply-pipe";
  char * request_pipe_path = NULL;
  char * reply_pipe_path = NULL;
  
  uint16_t operation = CLIENT_REQUEST_LIST_TASKS;
  uint64_t taskid;
  uint16_t errcode;
  uint16_t reptype;
  uint32_t nb_tasks;
  uint32_t nb_runs;
  uint32_t length;
  char* output;

  timing *p_timing = NULL;
  commandline *p_cmdl = NULL;
  
  int opt;
  char * strtoull_endp;
  while ((opt = getopt(argc, argv, "hlcqm:H:d:p:r:x:o:e:")) != -1) {
    switch (opt) {
    case 'm':
      minutes_str = optarg;
      break;
    case 'H':
      hours_str = optarg;
      break;
    case 'd':
      daysofweek_str = optarg;
      break;
    case 'p':
      pipes_directory = strdup(optarg);
      if (pipes_directory == NULL) goto error;
      break;
    case 'l':
      operation = CLIENT_REQUEST_LIST_TASKS;
      break;
    case 'c':
      operation = CLIENT_REQUEST_CREATE_TASK;
      break;
    case 'q':
      operation = CLIENT_REQUEST_TERMINATE;
      break;
    case 'r':
      operation = CLIENT_REQUEST_REMOVE_TASK;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'x':
      operation = CLIENT_REQUEST_GET_TIMES_AND_EXITCODES;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'o':
      operation = CLIENT_REQUEST_GET_STDOUT;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'e':
      operation = CLIENT_REQUEST_GET_STDERR;
      taskid = strtoull(optarg, &strtoull_endp, 10);
      if (strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
      break;
    case 'h':
      printf("%s", usage_info);
      return 0;
    case '?':
      fprintf(stderr, "%s", usage_info);
      goto error;
    }
  }

  /*
   * MANAGE PIPES DIRECTORY
   */

  // set default directory for pipes if -p option was not called :
  // /tmp/<USERNAME>/saturnd/pipes
  if(pipes_directory == NULL){

    // get username
    char *username = getenv("USER");
    if(username == NULL){
      perror("getlogin");
      goto error;
    }

    // build pipes directory path
    char* tmp;
    tmp = concat_strings("/tmp/", username);
    pipes_directory = concat_strings(tmp, "/saturnd/pipes");
    free(tmp);
  }

  /*
   * OPEN REQUEST PIPE
   */

  // path to request pipe
  request_pipe_path = concat_strings(pipes_directory, request_pipe_name);

  // open request pipe in write only mode
  // NOTE : if there is no reader this call will block execution
  int request_pipe_fd = open(request_pipe_path, O_WRONLY);
  if(request_pipe_fd < 0){
    perror("open");
    goto error;
  }

  /*
   * WRITE IN REQUEST PIPE
   */
  
  // send the operation code to saturnd
  if(write_opcode(operation, request_pipe_fd) < 0)
      goto error;

  // send timing and command line for 'CR' task
  if(operation == CLIENT_REQUEST_CREATE_TASK){

    //build timing from parameters
    p_timing = build_timing(minutes_str, hours_str, daysofweek_str);
    if(p_timing == NULL)
      goto error;


    // build commandline from parameters
    // compute number of unparsed arguments (they correspond to the commandline)
    int nb_args = argc - optind;
    p_cmdl = build_commandline(nb_args, &argv[optind]);

    // send 'CR' request
    if(write_timing(p_timing, request_pipe_fd) < 0
       || write_commandline(p_cmdl, request_pipe_fd) < 0)
      goto error;
  }

  // send taskid for remaining options
  if(operation == CLIENT_REQUEST_REMOVE_TASK
     || operation == CLIENT_REQUEST_GET_TIMES_AND_EXITCODES
     || operation == CLIENT_REQUEST_GET_STDOUT
     || operation == CLIENT_REQUEST_GET_STDERR){

    if(write_taskid(taskid, request_pipe_fd) < 0)
      goto error;
  }

  /*
   * OPEN REPLY PIPE
   */

  // path to reply pipe
  reply_pipe_path = concat_strings(pipes_directory, reply_pipe_name);

  // open reply pipe in read only mode
  // NOTE : if there is no writer this call will block execution
  int reply_pipe_fd = open(reply_pipe_path, O_RDONLY);
  if(reply_pipe_fd < 0){
    perror("open");
    goto error;
  }

  /*
   * READ FROM REPLY PIPE
   */

  if((reptype = read_uint16(reply_pipe_fd)) == -1) {
    goto error;
  }

  /*
   * If reptype is ok, read response from reply pipe
   */
  if(reptype == SERVER_REPLY_OK) {
    switch (operation) {
    case CLIENT_REQUEST_LIST_TASKS:
      if((nb_tasks = read_uint32(reply_pipe_fd)) < 0)
        goto error;
      if(read_display_tasks(nb_tasks,reply_pipe_fd) < 0)
        goto error;
    break;
    case CLIENT_REQUEST_CREATE_TASK:
      if((taskid = read_uint64(reply_pipe_fd)) < 0)
        goto error;
      //print the taskid
      printf("%lu\n",taskid);
    break;
    case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
      if((nb_runs = read_uint32(reply_pipe_fd)) < 0)
        goto error;
      if(read_time_and_exitcode(nb_runs,reply_pipe_fd) < 0)
        goto error;
    break;
    // same process for the two cases
    case CLIENT_REQUEST_GET_STDERR:
    case CLIENT_REQUEST_GET_STDOUT:

      // we read the length of the string on the reply pipe
      // and we check if there is an error
      if((length = read_uint32(reply_pipe_fd)) < 0)
        goto error;

      // we allocate memory for 'output'
      output = malloc(length);
      if(read(reply_pipe_fd, output, length) < 0)
        goto error;
      
      // diplay of the output on stdout
      printf("%s", output);
      
      // free memory allocated to 'output'
      free(output);
      break;
    }
  }
  
  /*
   * If reptype is err
   */
  if(reptype == SERVER_REPLY_ERROR){
    switch (operation){
    case CLIENT_REQUEST_REMOVE_TASK :
      
      // if the value read is less than 0 there is an error
      if((errcode = read_uint16(reply_pipe_fd)) < 0)
          goto error;
      
      // we check that the error code read is equal to the only possible value
      // which is SERVER_REPLY_ERROR_NOT_FOUND
      if(errcode == SERVER_REPLY_ERROR_NOT_FOUND)
          printf("il n'existe aucune tâche avec cet identifiant");
    break;
    case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:

      if((errcode = read_uint16(reply_pipe_fd)) < 0)
          goto error;
      
      // we check that the errcode read is equal to the only possible value
      // which is SERVER_REPLY_ERROR_NOT_FOUND
      if(errcode == SERVER_REPLY_ERROR_NOT_FOUND){
          printf("il n'existe aucune tâche avec cet identifiant");
          goto error;
      }
    break;
    // same process for both cases
    case CLIENT_REQUEST_GET_STDERR:
    case CLIENT_REQUEST_GET_STDOUT:
      
      // if the value read is less than 0 there is an error
      if((errcode = read_uint16(reply_pipe_fd)) < 0)
          goto error;
      
      // check for error code and print error message accordingly
      if(errcode == SERVER_REPLY_ERROR_NOT_FOUND){
          printf("il n'existe aucune tâche avec cet identifiant");
          goto error;
      }
      
      if(errcode == SERVER_REPLY_ERROR_NEVER_RUN){
          printf("la tâche n'a pas encore été exécutée au moins une fois");
          goto error;
      }
      break;
    }
  }

  /*
   * PRINT REPLY
   */

  // close request and reply pipes
  if(close(request_pipe_fd) < 0 || close(reply_pipe_fd) < 0){
    perror("close");
    goto error;
  }

  // free allocated memory
  free(pipes_directory);
  free(request_pipe_path);
  free(reply_pipe_path);
  free(p_timing);
  free(p_cmdl);
  
  return EXIT_SUCCESS;

 error:
  if (errno != 0) perror("main");
  
  free(request_pipe_path);
  free(reply_pipe_path);
  free(pipes_directory);
  free(p_timing);
  free(p_cmdl);
  
  request_pipe_path = NULL;
  reply_pipe_path = NULL;
  pipes_directory = NULL;
  
  p_timing = NULL;
  p_cmdl = NULL;
  
  return EXIT_FAILURE;
}

