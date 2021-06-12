#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cJSON.h>
#include <tuya_hal_wifi.h>
#include <tuya_os_adapter.h>
#include <uf_file.h>
#include "uni_log.h"

#define MAX_AP_SEARCH       (30)
#define WIFI_LIST_STR       "wifiList"

int tuya_devinter_get_wifi_strength(void)
{
	int wifi_strength = 99;
    /* TODO:
     * get wifi strength
     */
	return wifi_strength;
}

int tuya_devinter_wifi_all_ap_scan(char* result_buffer,int buf_size)
{
	int ret = 0;
	cJSON *ap_array = NULL;
	cJSON *wifi_list = NULL;
	if(result_buffer == NULL || buf_size <= 0) {
		PR_ERR("para error,result_buffer:%p,buf_size:%d\n",result_buffer,buf_size);
		return -1;
	}

    AP_IF_S s_aps[MAX_AP_SEARCH];
    memset(s_aps, 0, sizeof(s_aps));
    int aplist_len = 0;

    //for demo, we made fake ap list
    aplist_len = 2;
    s_aps[0].channel = 11;
    s_aps[0].rssi = -30;
    strcpy((char*)s_aps[0].ssid, "23BFE2032FB3231D60A7DE4E6F312986");
    s_aps[0].s_len = strlen(s_aps[0].ssid);
    unsigned char mac[6] = {0x84, 0xe2, 0x34, 0x74, 0x79, 0x00};
    memcpy(s_aps[0].bssid, mac, sizeof(mac));

    s_aps[1].channel = 11;
    s_aps[1].rssi = -30;
    strcpy((char*)s_aps[1].ssid, "DD3CC839D81D14133DD4CD6E1CE03878");
    s_aps[1].s_len = strlen(s_aps[1].ssid);
    unsigned char mac2[6] = {0xb0, 0xe2, 0x34, 0x74, 0x79, 0x00};
    memcpy(s_aps[1].bssid, mac2, sizeof(mac2));

    /*
     *  Please scan ap and return ap list and number.
     *  
     */
     
	ap_array = cJSON_CreateArray();
	if (ap_array == NULL) {
		PR_ERR("cJSON_CreateObject failed.\n");
		ret = -1;
		goto over;
	}

	for(int i =0; i < aplist_len; i++) {
		cJSON *ap = cJSON_CreateObject();
		if(ap == NULL) {
			PR_ERR("cJSON_CreateObject failed.\n");
			ret = -1;
			goto over;
		}
		cJSON_AddItemToObject(ap, "channel", cJSON_CreateNumber(s_aps[i].channel));
		cJSON_AddItemToObject(ap, "rssi", cJSON_CreateNumber(s_aps[i].rssi));
		cJSON_AddItemToObject(ap, "essid", cJSON_CreateString(s_aps[i].ssid));
		char bssid_str[64] = {0};
		for(int j = 0; j < sizeof(s_aps[i].bssid);j++) {
			if(j == 0) {
				snprintf(bssid_str,sizeof(bssid_str),"%02x",s_aps[i].bssid[j]);
			} else {
				snprintf(bssid_str+strlen(bssid_str),sizeof(bssid_str)-strlen(bssid_str),":%02x",s_aps[i].bssid[j]);
			}
		}
		cJSON_AddItemToObject(ap, "bssid", cJSON_CreateString(bssid_str));
		cJSON_AddItemToArray(ap_array, ap);
	}

	wifi_list = cJSON_CreateObject();
	if(wifi_list == NULL) {
		PR_ERR("cJSON_CreateObject failed.\n");
		ret = -1;
		goto over;
	}
	
	cJSON_AddItemToObject(wifi_list, WIFI_LIST_STR, ap_array);
    char *pjson = cJSON_Print(wifi_list);
	if(pjson) {
		if(strlen(pjson) >= buf_size) {
			PR_ERR("buf_size is too short:%d,json size:%d.\n",buf_size,strlen(pjson));
			ret = -1;
		} else {
			strncpy(result_buffer,pjson,buf_size);
			//PR_DEBUG("scan ap list:\r\n%s\r\n",result_buffer);
		}
		free(pjson);
	} else {
		PR_ERR("cJSON_Print failed.\n");
		ret = -1;
	}

over:
	if(wifi_list) {
		cJSON_Delete(wifi_list);
	} else if(ap_array) {
		cJSON_Delete(ap_array);
	}

	return ret;
}

