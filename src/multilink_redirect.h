#ifndef __MULTILINK_REDIRECT_H__
#define __MULTILINK_REDIRECT_H__

#include <pthread.h>

typedef struct _multilink_data {
    int serial_fd;
    int tcp_fd;
    int p2p_fd;
    int server_fd;

    pthread_t server_thread;
    pthread_t serial_thread;
    pthread_t tcp_thread;
    pthread_t p2p_thread;
} multilink_data;

static multilink_data g_multilink;

static multilink_data* get_multilink_data(){
    return &g_multilink;
}

void new_serial_thread(const char* path, int baudrate);
void new_tcp_thread(const char* ip, int port);

#endif  //__MULTILINK_REDIRECT_H__

