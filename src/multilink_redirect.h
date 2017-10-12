#ifndef __MULTILINK_REDIRECT_H__
#define __MULTILINK_REDIRECT_H__

typedef struct _multilink_data {
    int serial_fd;
    int tcp_fd;
    int p2p_fd;
    int out_fd;
} multilink_data;

multilink_data g_multilink_data;

void new_serial_thread(const char* path, int baudrate);

#endif  //__MULTILINK_REDIRECT_H__

