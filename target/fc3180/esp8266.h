#ifndef __ESP8266__H
#define __ESP8266__H

//
// CHANGELOG
// 
// 0.1.0 Henrik Juvonen - Enumerations and function declarations
// 0.2.0 Henrik Juvonen - Implemented functions
//                      - Added access point struct
//                      - Removed functions and parameters
//

//
// This header file declares a set of public functions. The functions are
// used to control ESP8266 module's Wi-Fi connection, SoftAP functionality,
// and TCP/IP socket communication.
//

#include "stdint.h"

//! Struct for access point
typedef struct {
	int8_t ecn;
	char* ssid;
	int8_t rssi;
} esp_wifi_ap;

//! Struct for received data
typedef struct {
	uint8_t id;
	uint32_t len;
	char *data;
} esp_recv_data;

//! Enumeration for status codes
typedef enum {
    ESP_OK,
    ESP_ERR_NULL,
    ESP_ERR_TIMEOUT
} esp_status;

//! Enumeration for wifi connection states
typedef enum {
    ESP_WIFI_IDLE,			// Not connected
    ESP_WIFI_CONNECTING,    // Connecting to an AP
    ESP_WIFI_CONNECT_OK,    // Connected to an AP
    ESP_WIFI_CONNECT_FAILED // Connection to an AP failed
} esp_connection_state;

//! Enumeration for wifi security types
typedef enum {
    ESP_SECURITY_NONE         = 0x00,
    ESP_SECURITY_WEP          = 0x01,
    ESP_SECURITY_WPA_PSK      = 0x02,
    ESP_SECURITY_WPA2_PSK     = 0x03,
    ESP_SECURITY_WPA_WPA2_PSK = 0x04
} esp_security_type;

//! Initialize the module
esp_status esp_init(int baudrate);

//! Get the current connection state
esp_connection_state esp_wifi_connection_state();

//! Connect to a wifi network
esp_status esp_wifi_connect(const char *ssid, const char *passphrase);

//! Disconnects from the current wifi network
esp_status esp_wifi_disconnect();

//! Enable soft-AP mode
esp_status esp_wifi_enable_soft_ap(const char *ssid, const char *passphrase);

//! Initiate a wifi scan and populate a list of ECN/SSID/RSSI
esp_status esp_wifi_scan();

//! Reset wifi driver
esp_status esp_wifi_reset();

//! Get current ip address
esp_status esp_ip_get(uint32_t *addr, uint32_t *netmask, uint32_t *gateway);

//! Set current ip address
esp_status esp_ip_set(uint32_t *addr, uint32_t *netmask, uint32_t *gateway);

//! Acquire current ip address from dhcp within timeout
esp_status esp_ip_acquire(uint32_t *addr, uint32_t *netmask, uint32_t *gateway, uint32_t timeout);

//! Create TCP/UDP connection to addr:port
int32_t esp_ip_connect(const char* type, const char* addr, uint32_t port);

//! Close TCP/UDP connection
int32_t esp_ip_close(int32_t fd);

//! Send data with length
int32_t esp_ip_send(int32_t fd, const char* data, uint32_t length);

//! Check if there is any data available once
int32_t esp_ip_available(int32_t fd);

//! Check if there is any data available with timeout
int32_t esp_ip_poll(int32_t fd, uint32_t timeout);

//! Receive data, returns length of data
int32_t esp_ip_recv(int32_t fd, char* data);

//! Join multicast group
int32_t esp_ip_group_join(const char* addr);

//! Leave multicast group
int32_t esp_ip_group_leave(const char* addr);

//! Proxy serial port, for firmware uploading and debugging
void esp_serial_proxy();

#endif
