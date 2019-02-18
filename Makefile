CFLAGS := -g -Wall -Werror


.PHONY : all
all : server client

server : server.c

client : client.c
	
.PHONY : clean
clean :
	-rm -f server client 

