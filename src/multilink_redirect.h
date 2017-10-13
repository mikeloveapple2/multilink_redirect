#ifndef __MULTILINK_REDIRECT_H__
#define __MULTILINK_REDIRECT_H__

#include <assert.h>
#include <pthread.h>

#define PATH_LEN   (64)

typedef enum _link_status_t {
    WAITING_STATUS,
    INIT_STATUS,
    RUNNING_STATUS,

    END_OF_STATUS
} link_status_t;

typedef struct _multilink_status {
    link_status_t serial_status;
    link_status_t tcp_status;
    link_status_t p2p_status;
} multilink_status_t;

typedef struct _multilink_property {
    char serial_path[PATH_LEN];
    int  serial_baud;

    char tcp_addr[PATH_LEN];
    int  tcp_port;
}multilink_property_t;

typedef struct _multilink_data {
    int serial_fd;
    int tcp_fd;
    int p2p_fd;
    int server_fd;

    pthread_t server_thread;
    pthread_t serial_thread;
    pthread_t tcp_thread;
    pthread_t p2p_thread;

    multilink_status_t   status;
    multilink_property_t props;
} multilink_data_t;


multilink_data_t* g_multilink;

static multilink_data_t* get_multilink_data(){
    assert(g_multilink);
    return g_multilink;
}

void init_multilink();

void new_serial_thread();
void new_tcp_thread();

#endif  //__MULTILINK_REDIRECT_H__

