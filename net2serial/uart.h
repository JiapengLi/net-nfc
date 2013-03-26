#ifndef __UART_H
#define __UART_H

#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

int uart_open(char *dev, int br);
int uart_read(int fd, uint8_t *buf, const size_t len, int timeout);
int uart_write(int fd, uint8_t *buf, const size_t len, int timeout);
void uart_send_pkt(int fd, uint8_t *cmd, int len);

#endif 