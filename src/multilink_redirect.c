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
extern pthread_t g_thread_serial;

tcp_data  th_tcp_data;
extern pthread_t g_thread_tcp;

void new_serial_thread(const char* path, int baudrate)
{
    puts("init new_serial_thread");
    th_serial_data.baudrate = baudrate;
    memset(&th_serial_data.filepath, '\0', sizeof(&th_serial_data.filepath));
    memcpy(&th_serial_data.filepath, path, strlen(path));
    pthread_create(&g_thread_serial, NULL, serial_thread,  &th_serial_data);
    pthread_detach(g_thread_serial);
}

void new_tcp_thread(const char* ip, int port)
{
    puts("init new_tcp_thread");
    th_tcp_data.port = port;
    memset(&th_tcp_data.address, '\0', sizeof(&th_tcp_data.address));
    memcpy(&th_tcp_data.address, ip, strlen(ip));
    pthread_create(&g_thread_tcp, NULL,   tcp_thread,  &th_tcp_data);
    pthread_detach(g_thread_tcp);

}
