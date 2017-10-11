/* tcp_redirect.c
 *
 * @author 
 * 
 * date: 2017/10/11
 * changelog:
 *
 */

#include "tcp_redirect.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define  DEBUG_TCP_RECV  (1)

int tcp_conn(int destport, const char* destip)
{
    int sockfd;
    struct sockaddr_in dest_addr;
    if( -1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))){
        return -1;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = htons(destport);
    memset(&dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));
    if(-1 == connect(sockfd, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr))){
        close(sockfd);
        return -2;
    }

    return sockfd;
}

void* tcp_thread(void* arg)
{
    tcp_data* data   = (tcp_data*)arg;
    const char* addr = data->address;
    int         port = data->port;

    pthread_setname_np(pthread_self(), "tcp_thread");

    int sock_fd = tcp_conn(port, addr);
    if(sock_fd > -1){
        tcp_start(sock_fd);
    }else{
        fprintf(stderr, "connect To %s:%d failed\n", addr, port);
    }

    pthread_exit(NULL);
}

void tcp_start(int fd)
{
    bool run = true;
    int  _fd = fd;
    while(run){
        fd_set fds;
        struct timeval timeout = {0, 0};
        char read_buf[2];
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);

        timeout.tv_sec   = 3;
        timeout.tv_usec  = 0;

        int ret = select( _fd + 1, &fds, NULL, NULL, &timeout);
        switch(ret){
            case -1 : {
                          fprintf(stderr, "tcp network error");
                          run = false;
                          break;
                      }
            case 0 : {
                         fprintf(stderr, "tcp network timeout");
                         run  = false;
                         break;
                     }
            default : {

                          if(FD_ISSET(_fd, &fds)){
                              if(read(_fd, read_buf, 1) > 0){
#if DEBUG_TCP_RECV > 0
                                  printf("0x%X\n", read_buf[0]);
#endif
                              }
                              break;
                          }

                      } 
                      usleep(10);
        } // switch
    }
}
