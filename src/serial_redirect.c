/* serial_redirect.c
 *
 * @author 
 * 
 * date: 2017/10/11
 * changelog:
 *
 */

#include "serial_redirect.h"
#include "multilink_redirect.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

#define   DEBUG_SERIAL_RECV (1)

int init_uart(const char* port, int baud)
{
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if( fd == -1 ){
        return -1;
    }else{
        fcntl(fd, F_SETFL, 0);
    }

    if(setup_uart(fd, baud) < 0){
        return -1;
    }

    return fd;
}

int setup_uart(int fd, int baud)
{

    if(!isatty(fd))
    {
        fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
        return -1;
    }

    struct termios config;

    if(tcgetattr(fd, &config) < 0)
    {
        fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
        return -1;
    }

    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
            INLCR | PARMRK | INPCK | ISTRIP | IXON);

    config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
            ONOCR | OFILL | OPOST);

#ifdef OLCUC
    config.c_oflag &= ~OLCUC;
#endif

#ifdef ONOEOT
    config.c_oflag &= ~ONOEOT;
#endif

    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8;

    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 10;

    switch (baud)
    {
        case 1200:
            if (cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0)
            {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 1800:
            cfsetispeed(&config, B1800);
            cfsetospeed(&config, B1800);
            break;
        case 9600:
            cfsetispeed(&config, B9600);
            cfsetospeed(&config, B9600);
            break;
        case 19200:
            cfsetispeed(&config, B19200);
            cfsetospeed(&config, B19200);
            break;
        case 38400:
            if (cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0)
            {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 57600:
            if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0)
            {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 115200:
            if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0)
            {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;

        case 460800:
            if (cfsetispeed(&config, B460800) < 0 || cfsetospeed(&config, B460800) < 0)
            {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        case 921600:
            if (cfsetispeed(&config, B921600) < 0 || cfsetospeed(&config, B921600) < 0)
            {
                fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
                return -1;
            }
            break;
        default:
            fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
            return -1;
            break;
    }

    if(tcsetattr(fd, TCSAFLUSH, &config) < 0)
    {
        fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
        return -1;
    }
    // ele1000 add to flush buff
    //    tcflush(fd,TCIOFLUSH);

    return 0;

}

void* serial_thread_func(void* arg)
{

    multilink_data_t* multilink = get_multilink_data();
    pthread_setname_np(pthread_self(), "serial_thread");

    bool event_loop_run = true;
    link_status_t current_status = multilink->status.serial_status;

    while(event_loop_run){
        current_status = multilink->status.serial_status;
        switch(current_status){
            case RUNNING_STATUS:{
                                    serial_running_handle();
                                    break;
                                }
            case INIT_STATUS:{
                                 serial_init_handle();
                                 break;
                             }

            case WAITING_STATUS:
            default            :{
                                    serial_waiting_handle();
                                    break;
                                }
        }// switch current_status
        usleep(100);
    }// while

    pthread_exit(NULL);
}


void start(int fd)
{
    bool run = true;
    int  _fd = fd;
    multilink_data_t* multilink = get_multilink_data();
    while(run){
        // read Serial
        fd_set fds;							//文件描述符集合
        struct timeval timeout = {0,0};
        char read_buf[2];
        FD_ZERO(&fds);				        //清空集合
        FD_SET(_fd, &fds);		            //添加文件描述符入集合

        timeout.tv_sec = 5;		            //设置阻塞超时时间，5秒
        timeout.tv_usec = 0;
        int ret = select(_fd + 1, &fds, NULL, NULL, &timeout);
        switch( ret )
        {
            //错误，断开串口连接
            case -1: {
                         fprintf(stderr, "serial select error !\n");
                         run = false;
                         serial_timeout_handle();
                         break;
                     }
                     //超时，断开串口连接
            case 0: {
                        fprintf(stderr, "serial select timeout\n");
                        run = false;
                        serial_timeout_handle();
                        break;
                    }
                    //有可读的数据
            default :{
                         if (FD_ISSET(_fd, &fds)){
                             if(read(_fd, read_buf,1) == 1){
#if DEBUG_SERIAL_RECV > 0
                                  printf("%c", read_buf[0]);
                                  fflush(stdout);
#endif
                                 // emit bytesReceived(data);
                             }else{
                                 fprintf(stderr, "!read failed");
                                 run = false;
                                 serial_timeout_handle();
                             } // if read
                         }else{
                                 fprintf(stderr, "!FD_ISSET");
                                 run = false;
                                 serial_timeout_handle();
                         } // if FD_ISSET
                         break;
                     } 
        }// switch ret
        checkSerialExist();

        usleep(10);
    } // while run
}

void checkSerialExist()
{

}

void serial_init_handle()
{
    multilink_data_t* multilink = get_multilink_data();
    const char* serial_path = multilink->props.serial_path;
    int         baudrate    = multilink->props.serial_baud;

    printf("prepare %s, %d\n", serial_path, baudrate);
    int serial_fd = init_uart(serial_path, baudrate);
    if( serial_fd > -1 ){
        multilink->serial_fd = serial_fd;
        multilink->status.serial_status = RUNNING_STATUS;
    }else{
        multilink->serial_fd = -1;
        perror("init uart error");
        multilink->status.serial_status = WAITING_STATUS;
    }
}

void serial_running_handle()
{
    multilink_data_t* multilink = get_multilink_data();
    start(multilink->serial_fd);
}

void serial_waiting_handle()
{
    sleep(1);
}

void serial_timeout_handle()
{
    multilink_data_t* multilink = get_multilink_data();
    if(multilink->serial_fd > -1){
        close(multilink->serial_fd);
    }

    multilink->serial_fd = -1;
    multilink->status.serial_status = WAITING_STATUS;
}
