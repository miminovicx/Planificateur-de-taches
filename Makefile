CC = gcc
CFLAGS = -Wall -g
EXEC = cassini
INC_DIR = include/
SRC_DIR = src/
OBJS = cassini.o string_type.o commandline.o timing-text-io.o client-request.o server-reply.o

$(EXEC) : $(OBJS)
	$(CC) $(CFLAGS) -I $(INC_DIR) -o $(EXEC) $(OBJS)

cassini.o : $(SRC_DIR)cassini.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $(SRC_DIR)cassini.c -o cassini.o

timing-text-io.o : $(SRC_DIR)timing-text-io.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $(SRC_DIR)timing-text-io.c -o timing-text-io.o

string_type.o : $(SRC_DIR)string_type.c 
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $(SRC_DIR)string_type.c -o string_type.o

commandline.o : $(SRC_DIR)commandline.c 
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $(SRC_DIR)commandline.c -o commandline.o

client-request.o : $(SRC_DIR)client-request.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $(SRC_DIR)client-request.c -o client-request.o

server-reply.o : $(SRC_DIR)server-reply.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $(SRC_DIR)server-reply.c -o server-reply.o
distclean :
	rm -rf *.o
	clear
