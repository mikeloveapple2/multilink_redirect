/* tcp_redirect.c
 *
 * @author  ele1000
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

#define  DEBUG_TCP_RECV  (0)

void* tcp_thread_func(void* arg)
{
    multilink_data_t* multilink = get_multilink_data();
    pthread_setname_np(pthread_self(), "tcp_thread");

    bool event_loop_run = true;
    link_status_t current_status = multilink->status.tcp_status;

    while(event_loop_run){
        current_status = multilink->status.tcp_status;
        switch(current_status){
            case RUNNING_STATUS:{
                                tcp_running_handle();
                                break;
                                }
            case INIT_STATUS:{
                                 tcp_init_handle();
                                 break;
                             }

            case WAITING_STATUS:
            default :{
                                 tcp_waiting_handle();
                                 break;
                     }
        }// switch current_status
        usleep(100);
    } // while
    pthread_exit(NULL);
}

void tcp_start(int fd)
{
    bool run = true;
    int  _fd = fd;
    multilink_data_t* multilink = get_multilink_data();
    printf("tcp_start recv fd %d\n", _fd);
    while(run){
        fd_set fds;
        struct timeval timeout = {0, 0};
        char read_buf[2];
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);

        timeout.tv_sec  = WAIT_DATA_TIMEOUT;
        timeout.tv_usec = 0;

        int ret = select( _fd + 1, &fds, NULL, NULL, &timeout);
        switch(ret){
            case -1 : {
                          printf("tcp network error\n");
                          run = false;
                          tcp_timeout_handle();
                          close(_fd);
                          break;
                      }
            case 0 : {
                         printf("tcp network timeout\n");
                         run  = false;
                         tcp_timeout_handle();
                         close(_fd);
                         break;
                     }
            default : {
                          if(FD_ISSET(_fd, &fds)){
                              if(read(_fd, read_buf, 1) > 0){
#if DEBUG_TCP_RECV > 0
                                  printf("tcp recv: 0x%X\n", read_buf[0]);
#endif
                                  if(multilink->tcp_recv_callback){
                                      multilink->tcp_recv_callback(read_buf[0]);
                                  }
                              }else{
                                  fprintf(stderr, "noting to read\n");
                                  run  = false;
                                  tcp_timeout_handle();
                                  close(_fd);
                              }
                          }else{
                                  fprintf(stderr, "! FD_ISSET\n");
                                  run  = false;
                                  tcp_timeout_handle();
                                  close(_fd);
                          }
                          break;
                      } 
                      fflush(stdout);
        } // switch
        usleep(20);
    } // while run
}



void tcp_init_handle()
{
    printf("tcp_init_handle()\n");
    fflush(stdout);
    multilink_data_t* multilink = get_multilink_data();
    const char* addr = multilink->props.tcp_addr;
    int         port = multilink->props.tcp_port;
    struct sockaddr_in serveraddr;

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_fd > -1){
        struct hostent* server;

        server = gethostbyname(addr);
        if(server == NULL){
            fprintf(stderr, "ERROR, no such host as %s\n", addr);
                multilink->status.tcp_status = WAITING_STATUS;
        }else{
            bzero((char*)&serveraddr, sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            bcopy((char*)server->h_addr,
                    (char*)&serveraddr.sin_addr.s_addr,
                    server->h_length);
            serveraddr.sin_port = htons(port);

            if(connect(sock_fd, &serveraddr, sizeof(serveraddr)) < 0){
                fprintf(stderr, "ERROR connecting\n");
                multilink->status.tcp_status = WAITING_STATUS;
            }else{
                multilink->tcp_fd = sock_fd;
                multilink->status.tcp_status = RUNNING_STATUS;
                printf("after tcp_start");
            }
        }
    }else{
        fprintf(stderr, "connect To %s:%d failed\n", addr, port);
        multilink->status.tcp_status = WAITING_STATUS;
    }
}

void tcp_running_handle()
{
    multilink_data_t* multilink = get_multilink_data();
    tcp_start(multilink->tcp_fd);
}

void tcp_waiting_handle()
{
    printf("tcp waitting handle\n");
    sleep(1);
}

void tcp_timeout_handle()
{
    multilink_data_t* multilink = get_multilink_data();
    multilink->tcp_fd = -1;
    multilink->status.tcp_status = WAITING_STATUS;
}
