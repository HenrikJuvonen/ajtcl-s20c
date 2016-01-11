#ifndef __ESPUTIL__H
#define __ESPUTIL__H

#include "esp8266.h"
#include "esplist.h"

// serial

int serial_init(int port, int baudrate);
int serial_read(int com, char *out);
int serial_read2(int com, int timeout, char *buf);
int serial_find(int com, int timeout, char *ack1, char *ack2);
void serial_proxy(int com);
void serial_writeline(int com, char *cmd);
void serial_write(int com, int len, const char *data);
void serial_write2(int com, char *fmt, ...);
List *serial_recvlst();

// str

void strdel(char *str, char c);
void strdump(char *str);

// misc

void console_write(char *fmt, ...);
void delay(int msec);

// wifi

int swreset(int com);
int cwmode(int com, int n);
int listap(int com, esp_wifi_ap *aps);
int getipaddr(int com, char *addr, char *netmask, char *gateway);
int setipaddr(int com, const char *addr, const char *netmask,
              const char *gateway);
int acquireipaddr(int com);

#endif
