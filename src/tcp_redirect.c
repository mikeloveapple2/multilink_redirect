/* tcp_redirect.c
 *
 * @author 
 * 
 * date: 2017/10/11
 * changelog:
 *
 */

#include "tcp_redirect.h"
#include "multilink_redirect.h"

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

void* tcp_thread_func(void* arg)
{
    tcp_data* data   = (tcp_data*)arg;
    const char* addr = data->address;
    int         port = data->port;

    pthread_setname_np(pthread_self(), "tcp_thread");

    int sock_fd = tcp_conn(port, addr);
    if(sock_fd > -1){
        get_multilink_data()->tcp_fd = sock_fd;
        tcp_start(sock_fd);
        printf("after tcp_start");
    }else{
        fprintf(stderr, "connect To %s:%d failed\n", addr, port);
    }

    pthread_exit(NULL);
}

void tcp_start(int fd)
{
    bool run = true;
    int  _fd = fd;
    multilink_data* multilink = get_multilink_data();
    printf("tcp_start recv fd %d\n", _fd);
    while(run){
        fd_set fds;
        struct timeval timeout = {0, 0};
        char read_buf[2];
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);

        timeout.tv_sec  = 3;
        timeout.tv_usec = 0;

        int ret = select( _fd + 1, &fds, NULL, NULL, &timeout);
        fprintf(stdout, "tcp select return ret(%d)\n", ret);
        switch(ret){
            fprintf(stdout, "tcp select return ret(%d)\n", ret);
            case -1 : {
                          printf("tcp network error");
                          run = false;
                          multilink->tcp_fd = -1;
                          close(_fd);
                          break;
                      }
            case 0 : {
                         printf("tcp network timeout");
                         run  = false;
                         multilink->tcp_fd = -1;
                         close(_fd);
                         break;
                     }
            default : {
                          if(FD_ISSET(_fd, &fds)){
                              if(read(_fd, read_buf, 1) > 0){
#if DEBUG_TCP_RECV > 0
                                  printf("tcp recv: 0x%X\n", read_buf[0]);
#endif
                              }else{
                                  fprintf(stderr, "Nothing to read");
                                  run  = false;
                                  multilink->tcp_fd = -1;
                                  close(_fd);
                              }
                          }
                          break;
                      } 
                      fflush(stdout);
        } // switch
        usleep(20);
    } // while run
}
