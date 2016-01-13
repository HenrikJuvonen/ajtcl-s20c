#define AJ_MODULE ALLJOYN
#include "aj_target.h"
#include "aj_wifi_ctrl.h"
#include "aj_debug.h"

AJ_EXPORT uint8_t dbgALLJOYN = 0;

AJ_Status ConfigureWifi() {
	if (AJ_ResetWiFi() == AJ_OK) {
		return AJ_ConnectWiFi("Teleperse", 0, 0, "sg15gu25");
	}
	return AJ_ERR_NULL;
}

AJ_Status ConfigureSoftAP() {
	return AJ_OK; //AJ_EnableSoftAP("esp000", 0, "dt01td01", 0);
}

static void ScanResult(void* context, const char* ssid, const uint8_t mac[6], uint8_t rssi, AJ_WiFiSecurityType secType, AJ_WiFiCipherType cipherType)
{
    static const char* const sec[] = { "OPEN", "WEP", "WPA", "WPA2" };
    static const char* const typ[] = { "", ":TKIP", ":CCMP", ":WEP" };

    AJ_AlwaysPrintf(("SSID %s [%02x:%02X:%02x:%02x:%02x:%02x] RSSI=%d security=%s%s\n", ssid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], rssi, sec[secType], typ[cipherType]));
}

void AllJoyn_Start() {
	AJ_Status status = AJ_OK;

    AJ_AlwaysPrintf(("AllJoyn Version %s\n", AJ_GetVersion()));
	
//	status = AJ_WiFiScan(NULL, ScanResult, 32);
//    if (status != AJ_OK) {
//        AJ_AlwaysPrintf(("WiFi scan failed\n"));
//    }

//	status = ConfigureSoftAP();
	esp_init(115200);

	status = ConfigureWifi();

//	esp_serial_proxy();
	
    if (status == AJ_OK) {
//		AJ_Initialize();
        AJ_Main();
    }

    AJ_AlwaysPrintf(("Quitting\n"));
    while (TRUE) {
    }
}
