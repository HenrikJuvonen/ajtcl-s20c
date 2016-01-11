#include "esp8266.h"
#include "esplist.h"
#include "esputil.h"
#include <socket.h>
#include <stdarg.h>
#include <string.h>

#define __esp_aps_size 64
#define __esp_fds_size 7

esp_connection_state __esp_con_state;
esp_wifi_ap __esp_aps[__esp_aps_size];
int8_t __esp_fds[__esp_fds_size];
int32_t __esp_com;
int8_t __esp_init = 0;

esp_status esp_init(int baudrate) {
  if (__esp_init) {
    return ESP_OK;
  }
  __esp_init = 1;
  __esp_com = serial_init(1, baudrate);
  int i;
  for (i = 0; i < __esp_aps_size; i++) {
    __esp_aps[i].ecn = 0;
    __esp_aps[i].ssid = 0;
    __esp_aps[i].rssi = 0;
  }
  for (i = 0; i < __esp_fds_size; i++) {
    __esp_fds[i] = 0;
  }
  return ESP_OK;
}

esp_status esp_init_115200() {
  if (__esp_init) {
    return ESP_OK;
  }
  __esp_init = 1;
  __esp_com = serial_init(1, 115200);
  int i;
  for (i = 0; i < __esp_aps_size; i++) {
    __esp_aps[i].ecn = 0;
    __esp_aps[i].ssid = 0;
    __esp_aps[i].rssi = 0;
  }
  for (i = 0; i < __esp_fds_size; i++) {
    __esp_fds[i] = 0;
  }
  return ESP_OK;
}

esp_connection_state esp_wifi_connection_state() { return __esp_con_state; }

esp_status esp_wifi_connect(const char *ssid, const char *passphrase) {
  __esp_con_state = ESP_WIFI_IDLE;
  serial_write2(__esp_com, "AT+CWJAP=\"%s\",\"%s\"\r", ssid, passphrase);
  int res = serial_find(__esp_com, 5, "OK", 0);
  if (res) {
    __esp_con_state = ESP_WIFI_CONNECTING;
    res = serial_find(__esp_com, 30000, "+CWSTAT:5", 0);

    if (res == 0) {
      esp_wifi_disconnect();
      __esp_con_state = ESP_WIFI_CONNECT_FAILED;
      res = ESP_ERR_NULL;
    } else {
      __esp_con_state = ESP_WIFI_CONNECT_OK;
      res = ESP_OK;
    }
  } else {
    res = ESP_ERR_NULL;
    __esp_con_state = ESP_WIFI_CONNECT_FAILED;
  }
  return res;
}

esp_status esp_wifi_disconnect() {
  serial_writeline(__esp_com, "AT+CWQAP");
  int res = serial_find(__esp_com, 5, "OK", 0) ? ESP_OK : ESP_ERR_NULL;
  __esp_con_state = ESP_WIFI_IDLE;
  return res;
}

esp_status esp_wifi_enable_soft_ap(const char *ssid, const char *passphrase) {
  serial_write2(__esp_com, "AT+CWSAP=\"%s\",\"%s\",6,4\r", ssid, passphrase);
  if (serial_find(__esp_com, 5, "OK", 0)) {
    return cwmode(__esp_com, 3) ? ESP_OK : ESP_ERR_NULL;
  }
  return ESP_ERR_NULL;
}

esp_status esp_wifi_scan() {
  int res = listap(__esp_com, __esp_aps);
  int i;
  for (i = 0; i < __esp_aps_size; i++) {
    if (__esp_aps[i].ssid && strcmp(__esp_aps[i].ssid, "") != 0) {
      console_write("  %d %s %d\n", __esp_aps[i].ecn, __esp_aps[i].ssid,
                    __esp_aps[i].rssi);
    }
  }
  return res == 1 ? ESP_OK : ESP_ERR_NULL;
}

esp_status esp_wifi_reset() {
  __esp_con_state = ESP_WIFI_IDLE;
  serial_writeline(__esp_com, "");
  if (cwmode(__esp_com, 1) && swreset(__esp_com)) {
    return ESP_OK;
  }
  return ESP_ERR_NULL;
}

esp_status esp_ip_get(uint32_t *addr, uint32_t *netmask, uint32_t *gateway) {
  char ip_a[16] = "\0";
  char ip_n[16] = "\0";
  char ip_g[16] = "\0";
  getipaddr(__esp_com, ip_a, ip_n, ip_g);
  if (strlen(ip_a) > 0 && strcmp(ip_a, "0.0.0.0") != 0) {
    *addr = inet_addr(ip_a);
    *netmask = inet_addr(ip_n);
    *gateway = inet_addr(ip_g);
    return ESP_OK;
  }
  return ESP_ERR_NULL;
}

static const char *AddrStr2(uint32_t addr) {
  static char txt[17];
  sprintf((char *)&txt, "%u.%u.%u.%u\0", (addr & 0xFF000000) >> 24,
          (addr & 0x00FF0000) >> 16, (addr & 0x0000FF00) >> 8,
          (addr & 0x000000FF));

  return txt;
}

esp_status esp_ip_set(uint32_t *addr, uint32_t *netmask, uint32_t *gateway) {
  return setipaddr(__esp_com, AddrStr2(*addr), AddrStr2(*netmask),
                   AddrStr2(*gateway))
             ? ESP_OK
             : ESP_ERR_NULL;
}

esp_status esp_ip_acquire(uint32_t *addr, uint32_t *netmask, uint32_t *gateway,
                          uint32_t timeout) {
  int i;
  for (i = 0; timeout == 0 || i < timeout / 100; i++) {
    if (acquireipaddr(__esp_com) &&
        esp_ip_get(addr, netmask, gateway) == ESP_OK) {
      return ESP_OK;
    }
    delay(100);
  }
  return ESP_ERR_TIMEOUT;
}

int32_t esp_ip_connect(const char *type, const char *addr, uint32_t port) {
  int i;
  char buf[512] = "\0";
  for (i = 0; i < __esp_fds_size; i++) {
    serial_write2(__esp_com, "AT+CIPCREATE=\"%s\"\r", type);
    delay(500);
    int len = serial_read2(__esp_com, 10, buf);
    char *x = strtok(buf, "\n");
    while (x != 0) {
      if (strstr(x, "+CIPCREATE:") == x) {
        char z1[2];
        z1[0] = x[11];
        z1[1] = 0;

        int fd = (int)strtol(z1, 0, 10);
        __esp_fds[fd] = 1;

        serial_write2(__esp_com, "AT+CIPCONNECT=%d,\"%s\",%d\r", fd, addr,
                      port);
        delay(500);
        if (strcmp(type, "UDP") == 0) {
          return fd;
        }
        if (strcmp(type, "TCP") == 0) {
          if (serial_find(__esp_com, 20, "+CIPCONNECT", "+CIPRECONNECT")) {
            return fd;
          }
          esp_ip_close(fd);
          return -1;
        }
      }
      x = strtok(0, "\n");
    }
  }
  return -1;
}

int32_t esp_ip_close(int32_t fd) {
  if (fd >= 0 && fd < __esp_fds_size) {
    delay(250);
    serial_write2(__esp_com, "AT+CIPDISCONNECT=%d\r", fd);
    delay(250);
    serial_write2(__esp_com, "AT+CIPCLOSE=%d\r", fd);
    __esp_fds[fd] = 0;
    delay(500);
    esp_wifi_reset();
    if (serial_find(__esp_com, 30, "+CWSTAT:5", 0)) {
      return fd;
    }
    return 0;
  }
  return -1;
}

int32_t mysprintf(char *str, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int len = vsprintf(str, fmt, args);
  va_end(args);
  return len;
}

char __esp_ip_send_buf[4096] = "\0";
int32_t esp_ip_send(int32_t fd, const char *data, uint32_t length) {
  if (fd >= 0 && fd < __esp_fds_size && __esp_fds[fd]) {
    char hex[4096] = "\0";
    int i;
    for (i = 0; i < length; i++) {
      mysprintf(hex, "%s%02X", hex, (unsigned char)data[i]);
    }

    int len = mysprintf(__esp_ip_send_buf, "AT+CIPSENDB=%d,%u,'%s'\r", fd,
                        length, hex);
    serial_write(__esp_com, len, __esp_ip_send_buf);
    delay(100);
    if (serial_find(__esp_com, 5, "+CIPSENDI", 0)) {
      return length;
    }
  }
  return -1;
}

int32_t esp_ip_available(int32_t fd) {
  if (fd >= 0 && fd < __esp_fds_size && __esp_fds[fd]) {
    serial_write2(__esp_com, "AT+CIPRD=%d\r", fd);
    serial_read2(__esp_com, 0, 0);
    List *list = serial_recvlst();
    LIST_FOREACH(list, first, next, cur) {
      esp_recv_data *rd = (esp_recv_data *)cur->value;
      if (rd->id == fd) {
        return 1;
      }
    }
  }
  return 0;
}

int32_t esp_ip_poll(int32_t fd, uint32_t timeout) {
  int i;
  for (i = 0; i < timeout / 10; i++) {
    if (esp_ip_available(fd)) {
      return 1;
    }
    delay(100);
  }
  return 0;
}

// TODO: Add length to parameters
int32_t esp_ip_recv(int32_t fd, char *buf) {
  if (esp_ip_available(fd)) {
    int32_t len = 0;
    int32_t pos = 0;
    buf[0] = 0;

    List *list = serial_recvlst();

    // concat data to buf
    LIST_FOREACH(list, first, next, cur) {
      esp_recv_data *rd = (esp_recv_data *)cur->value;
      if (rd->id == fd) {
        List_remove(list, cur);
        memcpy(buf + pos, rd->data, rd->len);
        pos += rd->len;
        len += rd->len;
        free(rd->data);
        free(rd);
      }
    }
    return len;
  }
  return -1;
}

int32_t esp_ip_group_join(const char *addr) {
  delay(500);
  serial_write2(__esp_com, "AT+CIPJOINGROUP=\"%s\"\r", addr);
  delay(500);
  return serial_find(__esp_com, 5, "OK", 0) ? 0 : -1;
}

int32_t esp_ip_group_leave(const char *addr) {
  delay(500);
  serial_write2(__esp_com, "AT+CIPLEAVEGROUP=\"%s\"\r", addr);
  delay(500);
  return serial_find(__esp_com, 5, "OK", 0) ? 0 : -1;
}

void esp_serial_proxy() { serial_proxy(__esp_com); }
