#ifndef __SERIAL_REDIRECT__
#define __SERIAL_REDIRECT__

typedef struct _serial_data {
    char filepath[64]; // eg. /dev/ttyUSB0
    int  baudrate;
} serial_data;

// return file descriptor
int init_uart(const char* port, int baud);

int setup_uart(int fd, int baud);

void* serial_thread(void* arg);

void start(int fd);

// foolish
void checkSerialExist();


#endif //__SERIAL_REDIRECT__
