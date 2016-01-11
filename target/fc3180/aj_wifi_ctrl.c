#include "aj_wifi_ctrl.h"
#include "aj_debug.h"
#include "aj_target.h"
#include "esp8266.h"
#include <effs.h>

AJ_WiFiConnectState AJ_GetWifiConnectState(void) {
  esp_connection_state state = esp_wifi_connection_state();
  switch (state) {
  case ESP_WIFI_IDLE:
    return AJ_WIFI_IDLE;
  case ESP_WIFI_CONNECTING:
    return AJ_WIFI_CONNECTING;
  case ESP_WIFI_CONNECT_OK:
    return AJ_WIFI_CONNECT_OK;
  case ESP_WIFI_CONNECT_FAILED:
    return AJ_WIFI_CONNECT_FAILED;
  }
  return AJ_WIFI_IDLE;
}

AJ_Status AJ_ConnectWiFi(const char *ssid, AJ_WiFiSecurityType secType,
                         AJ_WiFiCipherType cipherType, const char *passphrase) {
  return esp_wifi_connect(ssid, passphrase) == ESP_OK ? AJ_OK : AJ_ERR_NULL;
}

AJ_Status AJ_PrintFWVersion() { AJ_AlwaysPrintf(("FC3180+ESP8266\n")); }

AJ_Status AJ_DisconnectWiFi(void) {
  return esp_wifi_disconnect() == ESP_OK ? AJ_OK : AJ_ERR_NULL;
}

AJ_Status AJ_EnableSoftAP(const char *ssid, uint8_t hidden,
                          const char *passphrase, uint32_t timeout) {
  return esp_wifi_enable_soft_ap(ssid, passphrase) == ESP_OK ? AJ_OK
                                                             : AJ_ERR_NULL;
}

AJ_Status AJ_WiFiScan(void *context, AJ_WiFiScanResult callback,
                      uint8_t maxAPs) {
  return esp_wifi_scan() == ESP_OK ? AJ_OK : AJ_ERR_NULL;
}

AJ_Status AJ_ResetWiFi(void) {
  return esp_wifi_reset() == ESP_OK ? AJ_OK : AJ_ERR_NULL;
}

AJ_Status AJ_AcquireIPAddress(uint32_t *ip, uint32_t *mask, uint32_t *gateway,
                              int32_t timeout) {
  *ip = *mask = *gateway = 0;
  int res = esp_ip_acquire(ip, mask, gateway, 20000);
  return res == ESP_OK ? AJ_OK : AJ_ERR_DHCP;
}

AJ_Status AJ_GetIPAddress(uint32_t *ip, uint32_t *mask, uint32_t *gateway) {
  *ip = *mask = *gateway = 0;
  int res = esp_ip_get(ip, mask, gateway);
  return res == ESP_OK ? AJ_OK : AJ_ERR_NULL;
}

AJ_Status AJ_SetIPAddress(uint32_t ip, uint32_t mask, uint32_t gateway) {
  int res = esp_ip_set(&ip, &mask, &gateway);
  return res == ESP_OK ? AJ_OK : AJ_ERR_NULL;
}
