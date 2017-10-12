#include "multilink_redirect.h"
#include "serial_redirect.h"
#include "tcp_redirect.h"

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

serial_data th_serial_data;

tcp_data  th_tcp_data;

void new_serial_thread(const char* path, int baudrate)
{
    multilink_data* multilink = get_multilink_data();
    puts("init new_serial_thread");
    th_serial_data.baudrate = baudrate;
    memset(&th_serial_data.filepath, '\0', sizeof(&th_serial_data.filepath));
    memcpy(&th_serial_data.filepath, path, strlen(path));
    pthread_create(&multilink->serial_thread, NULL, serial_thread_func,  &th_serial_data);
    pthread_detach(multilink->serial_thread);
}

void new_tcp_thread(const char* ip, int port)
{
    puts("init new_tcp_thread");
    multilink_data* multilink = get_multilink_data();
    th_tcp_data.port = port;
    memset(&th_tcp_data.address, '\0', sizeof(&th_tcp_data.address));
    memcpy(&th_tcp_data.address, ip, strlen(ip));
    pthread_create(&multilink->tcp_thread, NULL,   tcp_thread_func,  &th_tcp_data);
    pthread_detach(multilink->tcp_thread);
}
