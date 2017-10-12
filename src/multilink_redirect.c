#include "multilink_redirect.h"
#include "serial_redirect.h"

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

serial_data th_serial_data;
extern pthread_t g_thread_serial;

void new_serial_thread(const char* path, int baudrate)
{
    puts("init new_serial_thread");
    th_serial_data.baudrate = baudrate;
    memset(&th_serial_data.filepath, '\0', strlen(path));
    memcpy(&th_serial_data.filepath, path, strlen(path));
    pthread_create(&g_thread_serial, NULL, serial_thread,  &th_serial_data);
    pthread_detach(g_thread_serial);
}
