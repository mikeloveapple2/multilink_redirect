#ifndef __TCP_REDIRECT_H__
#define __TCP_REDIRECT_H__

void* tcp_thread_func(void* arg);
void tcp_start(int fd);

void tcp_init_handle();
void tcp_running_handle();
void tcp_waiting_handle();
void tcp_timeout_handle();

#endif  //__TCP_REDIRECT_H__
