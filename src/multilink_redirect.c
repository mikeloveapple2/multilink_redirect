#include "multilink_redirect.h"
#include "serial_redirect.h"
#include "tcp_redirect.h"

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void new_serial_thread()
{
    puts("init new_serial_thread");
    multilink_data_t* multilink = get_multilink_data();
    multilink->status.serial_status = WAITING_STATUS;
    pthread_create(&multilink->serial_thread, NULL, serial_thread_func, NULL);
    // pthread_join(multilink->serial_thread, NULL);
    pthread_detach(multilink->serial_thread);
}

void new_tcp_thread()
{
    puts("init new_tcp_thread");
    multilink_data_t* multilink = get_multilink_data();
    multilink->status.tcp_status = WAITING_STATUS;
    pthread_create(&multilink->tcp_thread, NULL,   tcp_thread_func,  NULL);
    //pthread_join(multilink->tcp_thread, NULL);
    pthread_detach(multilink->tcp_thread);
}

void init_multilink()
{
    g_multilink = malloc(sizeof(multilink_data_t));
    memset(g_multilink, 0, sizeof(g_multilink));

    g_multilink->tcp_recv_callback = NULL;
    g_multilink->serial_recv_callback = NULL;
    g_multilink->p2p_recv_callback = NULL;
}

void write_byte(const void* buf, size_t count)
{
    multilink_data_t* multilink = get_multilink_data();
    if( multilink-> p2p_fd > -1 ){
        write(multilink->p2p_fd, buf, count);
    }

    if( multilink->tcp_fd > -1 ){
        write(multilink->tcp_fd, buf, count);
    }
    if( multilink-> serial_fd > -1 ){
        write(multilink->serial_fd, buf, count);
    }
}

void recv_byte(int fd, char c)
{
    multilink_data_t* multilink = get_multilink_data();
    char bar = c;
    if(fd == multilink->serial_fd){

    }else if(fd == multilink->tcp_fd){

    }else if(fd == multilink->p2p_fd){

    }
    
    if(multilink->server_fd > -1){
        write(multilink->server_fd, &bar, 1);
    }
}
