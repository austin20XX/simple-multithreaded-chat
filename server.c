
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#define PORT 17386
#define MAX_USERS 50
#define MAX_MSG_SIZE 512

typedef struct user{
        char user_name[33];
        unsigned int user_id;
        struct sockaddr_in addr;
        int user_fd;
}user_t;

//The client count needs to be global so all functions can access it without it being passed
//But need the static modifier so that other files cant access it.
static unsigned int user_count = 0;
user_t *users[MAX_USERS];
static unsigned int id = 1;

void *manage_client(void *args);
int add_to_user_queue(user_t* user);
int delete_from_user_queue(user_t* user);

void remove_newline(char*);

int main(int argc, char const *argv[])
{
        int server_fd, user_fd;
        struct sockaddr_in server_address;
        struct sockaddr_in user_address;
        socklen_t addrlen = (socklen_t)sizeof(server_address);

        // Create socket file descriptor and check if there was an error
        // AF_INET is Ipv4, SOCK_STREAM is TCP, 0 is IP protocol
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (0 == (server_fd)) {
                perror("Socket creation failed.");
                return 1;
        }
        //Settings for the socket
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(PORT);

        // Attach socket to port, AKA bind
        int bind_success =  bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address));
        if (bind_success <  0) {
                perror("Socket binding failed");
                return 2;
        }
        //Listen up ladies and gents
       int listen_success = listen(server_fd,5);
        //The second argument here is the backlog. How many connections to let into queue
        if (listen_success < 0) {
                perror("Listen failed");
                return 3;
        }


        puts("<< Server started...");

        while(1){
                user_fd = accept(server_fd, (struct sockaddr *)&user_address, &addrlen);
                if (user_fd < 0) {
                        perror("Accept failed");
                        return 4;
                }

                //Check if client limit has been reached
                if (user_count >= MAX_USERS){
                        char rejection_message[] = "Server >> Server at maximum client capacity\n";
                        send(user_fd, rejection_message, strlen(rejection_message), 0);
                        puts("<< Client tried to connect, but capacity reached");
                }

                user_t *user = (user_t*)malloc(sizeof(user_t));
                user -> user_id = id++;
                //Initially set the username to their user_id
                sprintf(user-> user_name,"%d", user -> user_id);
                user -> user_fd = user_fd;
                user -> addr = user_address;

                add_to_user_queue(user);

                pthread_t thread_id;
                pthread_create(&thread_id, NULL, &manage_client, (void*)user);

                //Because JimR says while(1) loops are really bad I can at least make it a little less bad lol
                sleep(1);
        }


return 0;

}

void *manage_client(void *args){
//Next Up
return NULL;
}

/***Takes a user and adds them to the user array
    Return 1 on success, 0 on failure
***/
int add_to_user_queue(user_t* user) {
    //Initially I was just going to place the user at whatever index whatever the user_count-1 was
    //But that doesn't use all the spots when users can join and leave

    int i;
    for(i=0;i<MAX_USERS;i++) {
        if(NULL == users[i]) {
            users[i] = user;
            user_count++;
            return 1;
        }//end if
    }//end for
    return 0;
}

/**Takes a user and deletes them from queue by matching the user_id
    Return 1 on success, 0 on failure
***/
int delete_from_user_queue(user_t* user) {
    int i;
    for(i=0; i<MAX_USERS; i++) {
        if(users[i]->user_id == user->user_id) {
            users[i] = NULL;
            user_count--;
            return 1;
        }//end if
    }//end for
    return 0;

}

void remove_newline(char* str){
	char* newline_char;
	if (NULL != (newline_char = strchr(str,'\n'))) {
		*newline_char = '\0';
	}
	return;
}
