/* 
 * tcpserver.c - A simple TCP echo server 
 * usage: tcpserver <port>
 */

#include "serial_redirect.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFSIZE 512

#define DEBUG_SERVER_OUTPUT   (1)

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

pthread_t th1;
pthread_t th_serial;
serial_data th_serial_data;

typedef struct _thread_data {
    int count;
    int fd;
}thread_data;

void new_serial_thread();

void* th1_func(void* arg)
{
    thread_data* data = (thread_data*)arg;
    printf("recv count : %d\n", data->count);
    
    bool run = true;
    while(run){
        fd_set fds;
        struct timeval timeout = {0, 0};
        char read_buf[2];
        FD_ZERO(&fds);
        FD_SET(data->fd, &fds);

        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        int ret = select(data->fd + 1, &fds, NULL, NULL, &timeout);
        switch(ret){
            case -1 : {
                          puts("select -1 disconnect");
                          run = false;
                          break;
                      }
            case 0 : {
                         puts("select 0 timeout");
                         const char* timeout_str = "select timeout!\n";
                         write(data->fd, timeout_str, strlen(timeout_str));
                         run = false;
                         break;
                     }
            default : {
                          if(FD_ISSET(data->fd, &fds)){
                              if(read( data->fd , read_buf, 1) > 0){
                                  if(read_buf[0] == 'X'){
                                      puts("recv X command");
                                      new_serial_thread();
                                  }
#if DEBUG_SERVER_OUTPUT > 0
                                  printf("0x%X ", read_buf[0]);
                                  printf("\n");
                                  fflush(stdout);
#endif
                                  // very important
                              } // read
                          }
                          break;
                      }
        } // switch
        usleep(5);
    }// while run

    close(data->fd);
    pthread_exit(NULL);
}


int main(int argc, char **argv) {
    int parentfd; /* parent socket */
    int childfd; /* child socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */

    /* 
     * check command line arguments 
     */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /* 
     * socket: create the parent socket 
     */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) 
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets 
     * us rerun the server immediately after we kill it; 
     * otherwise we have to wait about 20 secs. 
     * Eliminates "ERROR on binding: Address already in use" error. 
     */
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
            (const void *)&optval , sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short)portno);

    /* 
     * bind: associate the parent socket with a port 
     */
    if (bind(parentfd, (struct sockaddr *) &serveraddr, 
                sizeof(serveraddr)) < 0) 
        error("ERROR on binding");

    /* 
     * listen: make this socket ready to accept connection requests 
     */
    if (listen(parentfd, 1) < 0) /* allow 5 requests to queue up */ 
        error("ERROR on listen");

    /* 
     * main loop: wait for a connection request, echo input line, 
     * then close connection.
     */
    clientlen = sizeof(clientaddr);
    while (1) {

        /* 
         * accept: wait for a connection request 
         */
        childfd = accept(parentfd, (struct sockaddr *)&clientaddr, (socklen_t*)&clientlen);
        if (childfd < 0) 
            error("ERROR on accept");

        /* 
         * gethostbyaddr: determine who sent the message 
         */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
                sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL)
            error("ERROR on gethostbyaddr");
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL)
            error("ERROR on inet_ntoa\n");
        printf("server established connection with %s (%s)\n", 
                hostp->h_name, hostaddrp);

        thread_data data;
        data.count = 5;
        data.fd    = childfd;
        pthread_create(&th1, NULL, th1_func, &data);
        pthread_setname_np(th1, "th1_func");
         // pthread_join(th1, NULL);
        pthread_detach(th1);
        fprintf(stderr,"after pthread_detach\n");
    }
}

void new_serial_thread()
{
    puts("init new_serial_thread");
    const char* tmp_path = "/dev/ttyUSB0";
    th_serial_data.baudrate = 57600;
    memset(&th_serial_data.filepath, '\0', strlen(tmp_path));
    memcpy(&th_serial_data.filepath, tmp_path, strlen(tmp_path));
    pthread_create(&th_serial, NULL, serial_thread,  &th_serial_data);
    pthread_detach(th_serial);
}
