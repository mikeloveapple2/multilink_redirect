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
    pthread_create(&multilink->serial_thread, NULL, serial_thread_func, NULL);
    pthread_join(multilink->serial_thread, NULL);
}

void new_tcp_thread()
{
    puts("init new_tcp_thread");
    multilink_data_t* multilink = get_multilink_data();
    pthread_create(&multilink->tcp_thread, NULL,   tcp_thread_func,  NULL);
    pthread_join(multilink->tcp_thread, NULL);
}
