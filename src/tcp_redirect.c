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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define  DEBUG_TCP_RECV  (1)

void* tcp_thread_func(void* arg)
{
    tcp_data* data   = (tcp_data*)arg;
    const char* addr = data->address;
    int         port = data->port;
    struct sockaddr_in serveraddr;

    pthread_setname_np(pthread_self(), "tcp_thread");

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd > -1){
        struct hostent* server;

        server = gethostbyname(addr);
        if(server == NULL){
            fprintf(stderr, "ERROR, no such host as %s\n", addr);
        }else{
            bzero((char*)&serveraddr, sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            bcopy((char*)server->h_addr,
                    (char*)&serveraddr.sin_addr.s_addr,
                    server->h_length);
            serveraddr.sin_port = htons(port);

            if(connect(sock_fd, &serveraddr, sizeof(serveraddr)) < 0){
                fprintf(stderr, "ERROR connecting");
            }else{
                get_multilink_data()->tcp_fd = sock_fd;
                tcp_start(sock_fd);
                printf("after tcp_start");
            }
        }
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
                                  fprintf(stderr, "noting to read");
                                  run  = false;
                                  multilink->tcp_fd = -1;
                                  close(_fd);
                              }
                          }else{
                                  fprintf(stderr, "! FD_ISSET");
                                  run  = false;
                                  multilink->tcp_fd = -1;
                                  close(_fd);
                          }
                          break;
                      } 
                      fflush(stdout);
        } // switch
        usleep(20);
    } // while run
}
