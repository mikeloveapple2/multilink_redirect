#ifndef __TCP_REDIRECT_H__
#define __TCP_REDIRECT_H__

typedef struct _tcp_data {
    char address[64];
    int  port;
} tcp_data;


int tcp_conn(int destport, const char* destip);
void* tcp_thread(void* arg);
void tcp_start(int fd);


#endif  //__TCP_REDIRECT_H__
