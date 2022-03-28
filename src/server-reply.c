#include "server-reply.h"

/*
 * Reads uint16_t (like reptype) and returns it in case of success
 * Returns -1 on failure
 */
uint16_t read_uint16(int reply_pipe_fd) {
    uint16_t result;
    //reading the answer if its ok or err
    if(read(reply_pipe_fd,&result,sizeof(uint16_t)) < 0){
        perror("read");
        return -1;
    }
    result = be16toh(result);
    return result;
}


/*
 * Reads uint32_t and returns it in case of success
 * Returns -1 on failure
 */
uint32_t read_uint32(int reply_pipe_fd) {
    uint32_t result;
    if(read(reply_pipe_fd,&result,sizeof(uint32_t)) < 0) {
        perror("read");
        return -1;
    }
    result = be32toh(result);
    return result;
}

/*
 * Reads uint64_t (like taskid) and returns it in case of success
 * Returns -1 on failure
 */
uint64_t read_uint64(int reply_pipe_fd) {
    uint64_t result;
    if(read(reply_pipe_fd,&result,sizeof(uint64_t)) < 0)
    {
        perror("read");
        return -1;
    }
    result = be64toh(result);
    return result;
}

/*
 * Reads the timing in the reply pipe
 * Returns -1 if failure and 0 if success 
 */
int read_timing(char* timing_string, int reply_pipe_fd) {
    
    //minutes
    uint64_t minutes_read;
    if((minutes_read = read_uint64(reply_pipe_fd)) == -1) {
        return -1;
    }

    //hours
    uint32_t hours_read;
    if((hours_read = read_uint32(reply_pipe_fd)) == -1) {
        return -1;
    }

    //days
    uint8_t days_read;
    if(read(reply_pipe_fd,&days_read,sizeof(days_read)) < sizeof(days_read)) {
        return -1;
    }

    timing *timing_read;
    timing_read = malloc(sizeof(timing));
    if(timing_read < 0) {
        perror("malloc");
        return -1;
    }

    timing_read -> minutes = minutes_read;
    timing_read -> hours = hours_read;
    timing_read -> daysofweek = days_read;

    timing_string_from_timing(timing_string,timing_read);
    
    free(timing_read);
    
    return 0;
}

/*
 * Reads the commandline from the reply pipe 
 * Returns -1 if failure and 0 if success 
 */
int read_display_cmdline(char* cmdline_string,int reply_pipe_fd) {
    uint32_t argc;
    if((argc = read_uint32(reply_pipe_fd)) == -1) {
        return -1;
    }
    
    char* string_read;
    for(int i = 0; i < argc ; i++) {
        string_read = malloc(sizeof(string));
        if(string_read < 0) {
            perror("malloc");
            return -1;
        }
        uint32_t length;
        if((length = read_uint32(reply_pipe_fd)) == -1) {
            return -1;
        }
        if((string_read = malloc(length)) < 0) {
            perror("malloc");
            return -1;
        }
        
        if(read(reply_pipe_fd,string_read,length) < length) {
            perror("read");
            return -1;
        }
       printf(" %s",string_read);

       free(string_read);
   }
   printf("\n");
    return 0;
}


/*
 * Reads the tasks from the reply pipe and display it
 * Returns 0 if success and -1 on failure
 */
int read_display_tasks(uint32_t nb_tasks,int reply_pipe_fd) {
    uint64_t taskid;
    char * timing_string;
    char* cmdline_string;

    for(int i = 0; i < nb_tasks; i++) {
        // read taskid
        if((taskid = read_uint64(reply_pipe_fd)) < 0){
            return -1;
        }
        
        // read timing
        timing_string = malloc(TIMING_TEXT_MIN_BUFFERSIZE);
        if(timing_string < 0) {   
            perror("malloc");
            return -1;            
        }
        if((read_timing(timing_string,reply_pipe_fd)) == -1) {
            return -1;
        }
        printf("%lu: %s",taskid,timing_string);

        // read commandline
        cmdline_string = malloc(sizeof(commandline));
        if(read_display_cmdline(cmdline_string,reply_pipe_fd) == -1) {
            return -1;
        }

           
        free(timing_string);
    }
    return 0;
}


/*
 * Reads the time and exitcode from the reply pipe
 * return 0 on the success case and -1 on failure 
 */
int read_time_and_exitcode(uint32_t nb_runs, int reply_pipe_fd)
{
    time_t time;
    uint64_t time_tmp;
    struct tm* times;
    uint16_t exitcode;

    for(int i=0 ; i < nb_runs ; i++) 
    {
        //on commence par lire time sur le pipe 
        if( (time_tmp = read_uint64(reply_pipe_fd)) < 0 )
        {
            return -1;
        } 

        time = (time_t)time_tmp;

        // //on converti ici le time en type tm 
        times = localtime(&time);
        if(times == NULL)
        {
            return -1;
        }
        // //On lit le exitcode
        if ( (exitcode = read_uint16(reply_pipe_fd)) < 0 ) 
        {
            return -1;
        } 
        // //on affiche chaque champ sous le format time exitcode
        // //ex : 1970-01-01 00:00:00 (UTC) exitcode
        printf("%04d-%02d-%02d ", times-> tm_year +1900 , times->tm_mon +1, times-> tm_mday);
        printf("%02d:%02d:%02d %d\n",times-> tm_hour,times-> tm_min, times->tm_sec, exitcode);
        
        }
    return 0;
}
