#ifndef __SERIAL_REDIRECT__
#define __SERIAL_REDIRECT__

// return file descriptor
int init_uart(const char* port, int baud);

int setup_uart(int fd, int baud);

void* serial_thread_func(void* arg);

void start(int fd);

// foolish
void checkSerialExist();

void serial_init_handle();
void serial_running_handle();
void serial_waiting_handle();
void serial_timeout_handle();

#endif //__SERIAL_REDIRECT__
