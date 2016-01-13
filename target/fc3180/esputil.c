#include "esputil.h"
#include "aj_util.h"
#include <com.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "esplist.h"

List* recvlst = 0;

List* serial_recvlst()
{
	return recvlst;
}

#define __SERIAL_BUFFER_SIZE 4096

int serial_init(int port, int baudrate) {
	comInit();
	return comOpen(port, baudrate, DATABITS_8, STOPBITS_1, PARITY_NONE, FLOWCONTROL_NONE, COM_READWRITE, __SERIAL_BUFFER_SIZE);
}

void strdel(char* str, char c) {
	int i = 0, j = 0;
	for (i = 0; i < strlen(str); i++) {
		if (str[i] != c) {
			str[j++] = str[i];
		}	
	}
	str[j++] = '\0';
}

void strdump(char* str) {
	int j;
	for (j = 0; j < strlen(str); j++) {
		console_write("%d ", str[j]);
	}
	console_write("|%d |%s \n", strlen(str), str);
}	 

char __console_buf[__SERIAL_BUFFER_SIZE] = "\0";
int __console = 0;
void console_write(char* fmt, ...) {
	if (!__console) {
		__console = serial_init(2, 115200);
	}
	va_list args;			 
	va_start(args, fmt);
	int len = vsprintf(__console_buf, fmt, args);
	va_end(args);
	comWrite(__console, (unsigned char*)__console_buf, len);
	delay(20);
}

int serial_proxy_read(int com, char* out) {
	int n = 0;
	if (comDataAvailable(com) > 0 && out) {
		out[0] = 0;
		n = comRead(com, (unsigned char *)out, __SERIAL_BUFFER_SIZE);
		if (n > 0) {
			out[n] = '\0';
		}
	}
	return n;
}

char __buf_a[__SERIAL_BUFFER_SIZE] = "\0";
char __buf_b[__SERIAL_BUFFER_SIZE] = "\0";
void serial_proxy(int com) {
	console_write("Serial proxy on\n");
	while (1) {
		int len_a = serial_proxy_read(__console, __buf_a);
		int len_b = serial_proxy_read(com, __buf_b);

		if (len_a > 0) {
			serial_write(com, len_a, __buf_a);
		}
		if (len_b > 0) {
			serial_write(__console, len_b, __buf_b);
		} 
	}
}

void serial_write(int com, int len, const char* data) {
	comWrite(com, (unsigned char*)data, len);
}

char __serial_buf[__SERIAL_BUFFER_SIZE] = "\0";
char __serial_buf2[__SERIAL_BUFFER_SIZE] = "\0";
void serial_write2(int com, char* fmt, ...) {
	va_list args;			 
	va_start(args, fmt);
	int len = vsprintf(__serial_buf, fmt, args);
	va_end(args);
	comWrite(com, (unsigned char*)__serial_buf, len);
}

void delay(int msec) {
//	console_write("DELAY%d\n", sec);
	AJ_Sleep(msec);
}

int serial_read(int com, char* out) {
	int n = 0;
	if (comDataAvailable(com) > 0) {
		n = comRead(com, (unsigned char *)__serial_buf, __SERIAL_BUFFER_SIZE);
		if (n > 0) {
			__serial_buf[n] = '\0';
			if (out) {
				strcpy(out, __serial_buf);
			}
			if (recvlst) {
				// copy incoming data to recvlst
				char* x = strtok(__serial_buf, "\n");
				while (x != 0) {
					if (strstr(x, "+CIPRD") == x) {
						int len = strlen(x);
						int k = 7, l = 0, m = 0;
						char z1[4], z2[16];
						while (l < 2) {
							char fc = x[k];
							if (x[k] == ',' || x[k] == 0) {
								if (l == 0) {
									z1[m++] = 0;
								}
								if (l == 1) {
									z2[m++] = 0;
								}
								l++;
								m = 0;	
							} else {
								if (l == 0) {
									z1[m++] = x[k];
								}
								if (l == 1) {
									z2[m++] = x[k];
								}
							}
							k++; 
						}
						int id = (uint8_t)strtol(z1, 0, 10);
						int ln = (uint16_t)strtol(z2, 0, 10);
						if (ln > 0) {
							esp_recv_data *rd = (esp_recv_data*)malloc(sizeof(esp_recv_data));
							rd->id = id;
							rd->len = ln;
							rd->data = (char*)malloc(rd->len+1);
							memcpy(rd->data, x+k+2, rd->len);
							rd->data[rd->len] = 0;
							List_push(recvlst, rd);
						} 
					}
					x = strtok(0, "\n");
				}
			}
		}
	}
	return n;
}

int serial_read2(int com, int timeout, char* buf) {
	if (!recvlst) {
		recvlst = List_create();
	}
	int i;
	for (i = 0; i <= timeout * 10; i++) {
		if (serial_read(com, buf)) {
			if (buf) {
				console_write(buf);
			}
			break;
		}
		delay(100);
	}
	return strlen(buf);
}
		   
int serial_find(int com, int timeout, char* ack1, char* ack2) {
	if (!recvlst) {
		recvlst = List_create();
	}
	int i;
	for (i = 0; i <= timeout * 10; i++) {
		if (!serial_read(com, __serial_buf2)) {
			delay(100);
			continue;
		}	
		console_write(__serial_buf2);

		char* x = strtok(__serial_buf2, "\n");
		while (x != 0) {
			if (ack1 && strstr(x, ack1) == x) {
				return 1;	 
			}
			if (ack2 && strstr(x, ack2) == x) {
				return 1;	 
			}
			x = strtok(0, "\n");
		}						
	}
//	console_write("ERROR/TIMEOUT\n");
	return 0;
}

void serial_writeline(int com, char* cmd) {
	int len = strlen(cmd)+1;
	char* str = (char*)malloc(len+1);
	strcpy(str, cmd);
	str[len-1] = '\r';
	str[len] = '\0';
//	console_write("%s\n", str);				  
	comWrite(com, (unsigned char *)str, len);
	free(str);
	delay(100);								
}

int swreset(int com) {
	delay(2000);
	serial_writeline(com, "ATZ");
	delay(2000);
	return serial_find(com, 5, "+IREADY", 0);	
}

int cwmode(int com, int n) {
	serial_write2(com, "AT+CWMODE=%d\r", n);
	return serial_find(com, 5, "OK", 0);	
}

int listap(int com, esp_wifi_ap* aps) {
	serial_writeline(com, "AT+CWLAP");
	delay(2000);
	int len = serial_read2(com, 5, __serial_buf2);
	char* x = strtok(__serial_buf2, "\n");
	int j = 0;
	while (x != 0) {
		if (strcmp(x, "AT+CWLAP") == 0) {
			continue;
		}
		if (strcmp(x, "OK") == 0) {
			return 1;
		}
		if (strlen(x) > 0) {
			int k, l = 0, m = 0;
			char z1[16], z2[128], z3[16];
			for (k = 8; k < strlen(x); k++) {
				if (x[k] != ',') {
					if (l == 0) {
						z1[m++] = x[k];
					}
					if (l == 1) {
						z2[m++] = x[k];
					}
					if (l == 2) {
						z3[m++] = x[k];
					}
				}
				else {
					if (l == 0) {
						z1[m++] = 0;
						k++;
					}
					if (l == 1) {
						z2[m-1] = 0;
					}
					if (l == 2) {
						z3[m++] = 0;
					}
					l++;
					m = 0;	
				}	
			}
			if (aps[j].ssid != 0) {
				free(aps[j].ssid);
			}
			aps[j].ecn = (int)strtol(z1, 0, 10);
			aps[j].ssid = (char*)malloc(strlen(z2)+1);
			aps[j].rssi = (int)strtol(z3, 0, 10);
			strcpy(aps[j].ssid, z2);
			j++;
		}
		x = strtok(0, "\n");
	}
	return 0;	
}

int getipaddr(int com, char* addr, char* netmask, char* gateway) {
	addr[0] = '\0';
	netmask[0] = '\0';
	gateway[0] = '\0';
	serial_writeline(com, "AT+CIFSR");
	int len = serial_read2(com, 5, __serial_buf2);
	char* x = strtok(__serial_buf2, "\n");
	while (x != 0) {
		if (strstr(x, "+CIPSTA") == x) {
			int k, l = 0, m = 0;
			for (k = 8; k < strlen(x); k++) {
				if (x[k] == '"') {
					continue;
				}
				else if (x[k] != ',') {
					if (l == 0) {
						addr[m++] = x[k];
					}
					if (l == 1) {
						netmask[m++] = x[k];
					}
					if (l == 2) {
						gateway[m++] = x[k];
					}
				}
				else {
					if (l == 0) {
						addr[m++] = 0;
					}
					if (l == 1) {
						netmask[m++] = 0;
					}
					if (l == 2) {
						gateway[m++] = 0;
					}
					l++;
					m = 0;	
				}	
			}
		}
		x = strtok(0, "\n");
	}
	return strlen(addr) > 0;	
}

int setipaddr(int com, const char* addr, const char* netmask, const char* gateway) {
	serial_write2(com, "AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r", addr, netmask, gateway);
	return serial_find(com, 5, "OK", 0);	
}

int acquireipaddr(int com) {
	serial_writeline(com, "AT+CIPSTA=0");
	return serial_find(com, 5, "OK", 0);
}

