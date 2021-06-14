#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <tuya_iot_com_api.h>
#include <tuya_cloud_com_defs.h>
#include <tuya_os_adapter.h>
#include "wifi_constants.h"
#include "lwip_netconf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uni_log.h"
#include "tuya_ipc_media.h"
#include "ty_user_devinter_proto.h"
#include "ty_user_paring.h"
#include "ty_sys_msg.h"
#include "tuya_ipc_api.h"
#include "tuya_devinter_demo_sys.h"
#include "tuya_devinter_demo_net.h"
#include "tuya_devinter_demo_media.h"

#define TUYA_GW_SSID_PREFIX     ("ty") //TODO: 按照不同客户作调整,2个字符
#define SCAN_INTERVAL           (2000)
#define PROTO_VERSION           "0.0.1"

//TODO: get pid from TUYA PM manager
STATIC CHAR_T *demo_pid = "a7bl3y36lqavkozt";

//TODO: get uuid from TUYA PM manager
STATIC CHAR_T *demo_uuid = "tuya6ef03f30230f7f1d";

//TODO: get dev bind stat
STATIC BOOL_T demo_binded = FALSE;

//TODO: get aeskey from system flash after finishing binding process
STATIC BYTE_T demo_aeskey[16] = {0};

//TODO: get uuid of gw from system after finishing binding process
STATIC CHAR_T demo_gw_uuid[32];

//TODO: get wakeup pattern from system flash after finishing binding process
STATIC CHAR_T demo_wakeup_pattern[32];

//TODO: get keerpalive pattern from system flash after finishing binding process
STATIC CHAR_T demo_keerpalive_pattern[32];

//TODO: get gateway ip from system,host byte order
UNW_IP_ADDR_T demo_gw_addr = 0xa0610fe;//10.6.16.254

//TODO: get mac address from system
STATIC BYTE_T demo_mac_addr[6] = {0x00, 0x11, 0x09, 0x01, 0x02, 0x03};

STATIC CHAR_T demo_ssid[32+1] = {0};
STATIC CHAR_T demo_passwd[64+1] = {0};

//TODO: get utc time from rtc
STATIC LONG_T demo_utc = 1622995200;

//callback when got ssid and passwd frmo gateway
int tuya_devinter_get_ssid_cb(char *ssid, char *passwd)
{
    if(ssid == NULL || passwd == NULL){
        PR_ERR("ssid or passwd is NULL\n");
        return -1;
    }
	
    PR_DEBUG("ssid:%s passwd:%s\n", ssid, passwd);
	/* TODO:
	 *	ssid and passwd should be saved in wifi module .
	 */
	strncpy(demo_ssid,ssid,sizeof(demo_ssid));
	strncpy(demo_passwd,passwd,sizeof(demo_passwd));
    return 0;
}

//callback to start scan wifi
int tuya_devinter_wifi_scan_cb(void)
{	
	/* TODO:
	 * inform wifi module to scan ssid and pasword from gw
	 */
    return 0;
}

 
int tuya_devinter_connect_gw(char* ssid, char*passwd)
{
	int ret = 0;
    if(ssid == NULL || passwd == NULL){
        PR_ERR("ssid or passwd is NULL\n");
        return -1;
    }
	
	/* TODO:
	 *	developer must call wifi API to connect to TUYA base station gw.
	 */
	
	/*	1. Disable Wi-Fi			
	*********************************************************************************/
	printf("\n\r Disable Wi-Fi\n");
	wifi_off();
	vTaskDelay(20); 
	
	
	/*********************************************************************************
	*	2. Enable Wi-Fi with STA mode			
	*********************************************************************************/
	printf("\n\r Enable Wi-Fi with STA mode\n"); 
	if(wifi_on(RTW_MODE_STA) < 0){
		printf("\n\r ERROR: wifi_on failed\n");
		return -1;
	}
	
	/*********************************************************************************
	*	3. Connect to AP using STA mode	
	*********************************************************************************/	
	printf("\n\r Connect to AP using STA mode\n");
	if (0 == wifi_connect(ssid,RTW_SECURITY_WPA2_AES_PSK,passwd,strlen(ssid),strlen(passwd),-1,NULL))
	{
		LwIP_DHCP(0, DHCP_START);
	} else {
		printf("\n\r wifi connect %s failed\r\n",ssid);
		return -1;
	}

	return ret;
}

void demo_main_task(void* not_used)
{
	OPERATE_RET ret;
	
	//IMPORTANT!! init tuya iot sdk. Don't remove or modify this code.
	TY_INIT_PARAMS_S init_param = {0};
    init_param.init_db = FALSE;
    strcpy(init_param.sys_env, TARGET_PLATFORM);
    tuya_iot_init_params(NULL, &init_param);
	//===============================================================

	printf("devinter_proto sdk version:%s\r\n",ty_user_devinter_get_version());
	
	//set system and sdk time
	ty_time_set_systime(demo_utc);
	tuya_ipc_set_service_time(demo_utc);

	//log level 
	tuya_ipc_set_log_attr(4,NULL);
	
    //msg_init
    ret = ty_sys_msg_queue_init();
    if (0 != ret){
        PR_ERR("####failed\n");
		vTaskDelete(NULL);
    }
	
    //init sd card
	ret = demo_sd_init();
    if (0 != ret){
        PR_ERR("####failed\n");
    }

	//stream buffer create
	tuya_devinter_ring_buffer_create();
	
	//put av data to ring  buffer
	tuya_devinter_av_start();

	//init devinter_proto sdk
    TY_USER_DEVINTER_CALLBACK_S devinter_cbs={0};
	tuya_devinter_proto_cb_init(&devinter_cbs);
    ty_user_devinter_proto_init(&devinter_cbs);

	demo_read_config(&demo_binded, demo_aeskey, demo_gw_uuid, demo_wakeup_pattern, demo_keerpalive_pattern);

	if(!demo_binded) {
		//not bind, start binding process.
		TY_USER_PARING_CALLBACK_S paring_cb={0};
		paring_cb.get_cfg_cb = tuya_devinter_get_ssid_cb;
		paring_cb.wifi_scan_cb = tuya_devinter_wifi_scan_cb;
		ty_user_paring_init(&paring_cb);
		
		// start to scan ap from gateway
		ty_user_paring_start(TUYA_GW_SSID_PREFIX);

		char scan_result[4096] = {0};
		//wait for getting the ssid and password of the gateway.
        do {
			if(0 == tuya_devinter_wifi_all_ap_scan(scan_result,sizeof(scan_result))) {
				ty_user_paring_get_wifi_scan_result(scan_result);
			}
            PR_DEBUG("Scanning AP List...\n");
			tuya_hal_system_sleep(SCAN_INTERVAL);
        } while (ty_user_paring_get_status() != TY_USER_MSSID_OVER);
		
		ty_user_paring_stop();

		//now has got the ssid and password of the gateway,start to connect it
		ret = tuya_devinter_connect_gw(demo_ssid,demo_passwd);
		if(ret == 0) {
			PR_DEBUG("Connect to gw wifi successful\n");
		} else {
			PR_ERR("Connect gw wifi failed,exit!!!\n");
			vTaskDelete(NULL);
			return;
		}
	} 
	
	//start the conversation between sub-device and gateway
	TY_USER_DEVINTER_CFG_T devinter_cfg;
	memset(&devinter_cfg,0,sizeof(devinter_cfg));
	strncpy(devinter_cfg.pid,demo_pid,sizeof(devinter_cfg.pid));
	strncpy(devinter_cfg.uuid,demo_uuid,sizeof(devinter_cfg.uuid));
	devinter_cfg.gw_addr = demo_gw_addr;
	memcpy(devinter_cfg.mac_addr,demo_mac_addr,sizeof(devinter_cfg.mac_addr));
	
	printf("gw_addr:%u,mac_addr: ",devinter_cfg.gw_addr);
	for(unsigned int i=0;i < sizeof(devinter_cfg.mac_addr);i++) {
		printf("%02x ",devinter_cfg.mac_addr[i]);
	}
	printf("\r\n");
	strcpy(devinter_cfg.ver,PROTO_VERSION);
	
    //encrypt I-frame only to reduce gw CPU loading
	devinter_cfg.encrypt_Iframe_only = 1;
	memcpy(devinter_cfg.aeskey,demo_aeskey,sizeof(devinter_cfg.aeskey));
	devinter_cfg.aeskey_len = 16;
	devinter_cfg.binded = demo_binded;
	
	ty_user_devinter_proto_start(&devinter_cfg);
	
    int loopCnt = 0;
	do {
        if(loopCnt%50 == 0){
            PR_DEBUG("wait for connect gw socket\n");
        }
        tuya_hal_system_sleep(20);
        loopCnt++;  
	}while(ty_devinter_get_connect_stat() != DEVINTER_STATUS_CONNECT);

	PR_DEBUG("connect to gw socket success\n");

	tuya_devinter_event_start();
	vTaskDelete(NULL);
}

int main(void)
{
    printf("enter main\n");
	/* Initialize log uart and at command service */
    console_init();	
	/* Initilaize the LwIP stack */
	LwIP_Init();
    xTaskCreate(demo_main_task, "demo_main_task", 8192, NULL,3, NULL);
    /*Enable Schedule, Start Kernel*/

    vTaskStartScheduler();
    printf("schedule error\n");
    for(;;)
    ;
}
