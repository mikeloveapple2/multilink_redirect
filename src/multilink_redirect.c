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
}

