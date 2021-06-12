#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tuya_cloud_types.h>
#include <uni_base64.h>
#include "uni_log.h"
#include "ty_time.h"
#include "tuya_ipc_api.h"
#include "tuya_hal_network.h"
#include "tuya_hal_fs.h"
#include "inet.h"
#include "ty_keepalive.h"
#include "ty_user_devinter_proto.h"
#include "tuya_devinter_demo_sys.h"
#include "tuya_devinter_demo_media.h"
#include "tuya_devinter_demo_net.h"
#include "tuya_devinter_demo_dp.h"

#define SW_VERSION_DEMO  "1.0.0"

extern UNW_IP_ADDR_T demo_gw_addr;

// Implement bind function
int tuya_devinter_bind_cb(TY_USER_GW_CFG_T* gw_cfg)
{    
	/* TODO:
     * when binding, ipc will get aeskey, uuid of gw, wakeup pattern and keepalive pattern.
     * ipc must save those parameters and use it in other position. for example,when used follow function:
     * int tuya_device_sleep_establish_keepalive_to_gw(char *server, int port, char *mac, uint8_t* pkey, int version);
     * gw_cfg->aeskey can be used to fill the "pkey" para , gw_cfg->keepalive_pattern can be used to fill the "mac" para 
     */
	demo_write_config(true,gw_cfg->aeskey, gw_cfg->bind_gw_uuid, gw_cfg->wakeup_pattern, gw_cfg->keepalive_pattern);
    PR_DEBUG("BIND GW OK");
    return 0;
}

// Handle bind failure situation
int tuya_devinter_bind_fail_cb(int errorCode)
{
    PR_DEBUG("==== bind fail ====");
	
    /* TODO:
     * when bind fail, ipc must erase wifi config(ssid and passwd).
     */
    remove("./devinter_demo.cfg");
    return 0;
}

// Implement unbind function
int tuya_devinter_unbind_cb(void)
{
    PR_DEBUG("==== reset config and unbind... ====");
    /* TODO:
     * when unbinding, ipc must erase all parameters saved by tuya_devinter_bind_cb() and reboot.
     * Then ipc must enter wifi-config mode after reboot. (call ty_user_paring_start())
     */
    remove("./devinter_demo.cfg");
    return 0;
}

int tuya_devinter_deepsleep_cb(TY_USER_DEVINTER_SLEEP_REASON_E reason)
{
    //RESERVED and UNUSED.
    return 0;
}

TY_USER_DEVINTER_WAKE_REASON_E tuya_devinter_get_wake_reason_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
}

int tuya_devinter_gw_disconn_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
}

int tuya_devinter_gw_reconn_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
} 
int tuya_devinter_ota_stat_cb(int stat, int otaFwSize)
{
	int ret = 0;
    PR_DEBUG("stat:%d otaFwSize:%d\n", stat, otaFwSize);
	if(stat == 0) {
		/* TODO:
		 * This callback will be invoked when failed to reply "start ota" cmd to gw.
		 * Customer should Stop OTA process
		 * tuya_devinter_sleep_cb will be called after this callback returned.
 		 */
	} else {
		/* TODO:
		 * This callback will be invoked when endusers launch upgrade function from APP.
		 * Customer can perform some actions to prepare OTA process, for example:
		 * Check battery life
		 * Malloc memory
		 * Stop av process 
		 * after this callback return successfully:
		 * OTA data will be transfered by tuya_devinter_ota_data_cb
		 * devinter_proto system status will be set to "DEVINTER_STATUS_OTA"
		 */
		 
		//ret = customer_ipc_start_ota(otaFwSize);
	}
    
    return ret;
}

// Implement OTA function
int tuya_devinter_ota_data_cb(unsigned char *fw_data, unsigned int fw_data_len)
{
	INT_T ret = 0;

	/*
	 * TODO:
	 * ret = customer_process_ota(fw_data, fw_data_len);
	 * | return |  |
	 * | ------ | ------ |
	 * | 0	| get fw_data ok, ready to get next slice |
	 * | 1	| get fw_data ok, and whole fw data is received. ready to write flash |
	 * | -1 | error occued. Upgraded process aborted |
	 */

	switch (ret)
	{
		case 0:
			//nothing to do. Just continue to precess next ota data
			ret = 0;
			break;
		case 1:
			//it's going to write flash. so let's stop ctrl process to maintain system stable.
			ty_user_devinter_proto_uninit();
			ret = 0;
			break;
		case -1:
		default:
			ret = -1;
			break;
	}
	return ret;
}


int tuya_devinter_pingpong_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
} 

int tuya_devinter_query_cb(TY_USER_DEVINTER_QUERY_INFO_S *query_info)
{
	int ret = 0;
    if(query_info == NULL){
        PR_ERR("query_info is null\n");
        return -1;
    }
	
	IPC_MEDIA_INFO_S media_info;
	memset(&media_info,0,sizeof(media_info));
	demo_media_info_init(&media_info);
	
    memset(query_info, 0, sizeof(TY_USER_DEVINTER_QUERY_INFO_S));
    //main video stream
    query_info->video[E_CHANNEL_VIDEO_MAIN].width = media_info.video_width[E_CHANNEL_VIDEO_MAIN];
    query_info->video[E_CHANNEL_VIDEO_MAIN].height = media_info.video_height[E_CHANNEL_VIDEO_MAIN];
    query_info->video[E_CHANNEL_VIDEO_MAIN].codec = media_info.video_codec[E_CHANNEL_VIDEO_MAIN];
    query_info->video[E_CHANNEL_VIDEO_MAIN].fps = media_info.video_fps[E_CHANNEL_VIDEO_MAIN];
    query_info->video[E_CHANNEL_VIDEO_MAIN].bps = media_info.video_bitrate[E_CHANNEL_VIDEO_MAIN] * 1024;
    query_info->video[E_CHANNEL_VIDEO_MAIN].gop = media_info.video_gop[E_CHANNEL_VIDEO_MAIN];

    //sub video stream
    query_info->video[E_CHANNEL_VIDEO_SUB].width = media_info.video_width[E_CHANNEL_VIDEO_SUB];
    query_info->video[E_CHANNEL_VIDEO_SUB].height = media_info.video_height[E_CHANNEL_VIDEO_SUB];
    query_info->video[E_CHANNEL_VIDEO_SUB].codec = media_info.video_codec[E_CHANNEL_VIDEO_SUB];
    query_info->video[E_CHANNEL_VIDEO_SUB].fps = media_info.video_fps[E_CHANNEL_VIDEO_SUB];
    query_info->video[E_CHANNEL_VIDEO_SUB].bps = media_info.video_bitrate[E_CHANNEL_VIDEO_SUB] * 1024;
    query_info->video[E_CHANNEL_VIDEO_SUB].gop = media_info.video_gop[E_CHANNEL_VIDEO_SUB];

    query_info->audio[TUYA_IPCSET_MIC].codec = TUYA_CODEC_AUDIO_PCM;
    query_info->audio[TUYA_IPCSET_MIC].channel = TUYA_AUDIO_CHANNEL_MONO;
    query_info->audio[TUYA_IPCSET_MIC].sample_rate = TUYA_AUDIO_SAMPLE_8K;
    query_info->audio[TUYA_IPCSET_MIC].sample_size = TUYA_AUDIO_DATABITS_16;

    query_info->audio[TUYA_IPCSET_SPEAKER].codec = TUYA_CODEC_AUDIO_G711A;
    query_info->audio[TUYA_IPCSET_SPEAKER].channel = TUYA_AUDIO_CHANNEL_MONO;
    query_info->audio[TUYA_IPCSET_SPEAKER].sample_rate = TUYA_AUDIO_SAMPLE_8K;
    query_info->audio[TUYA_IPCSET_SPEAKER].sample_size = TUYA_AUDIO_DATABITS_16;

    query_info->snapshot_codec = media_info.video_codec[E_CHANNEL_VIDEO_SNAP];
    query_info->snapshot_width = media_info.video_width[E_CHANNEL_VIDEO_SNAP];
    query_info->snapshot_height = media_info.video_height[E_CHANNEL_VIDEO_SNAP];

    query_info->isp_fps = 30;
    strncpy(query_info->sw_version, SW_VERSION_DEMO, sizeof(query_info->sw_version));
    query_info->wifi_strength = tuya_devinter_get_wifi_strength();
	ret = dp_sync_json_create(&query_info->dp_array);
	return ret;
}

int tuya_devinter_reboot_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
} 

// Implement two-way audio function
// Audio format is G711 ulaw
int tuya_devinter_recv_audio_cb(unsigned char *audio_data, unsigned int audio_data_len)
{
    int ret = 0;
	if(!audio_data || audio_data_len <= 0) {
		PR_ERR("audio_data is null or audio_data_len error:%d\n", audio_data_len);
		return -1;
	}
    PR_DEBUG("audio_data_len:%d\n", audio_data_len);
	
    return ret;
}

int tuya_devinter_set_speaker_cb(int enable)
{
    PR_DEBUG("enable:%d\n", enable);
    /* TODO:
        This callback will be invoked when APP enable or disable two-way audio function.
        Customer should enable or disable speaker according parameter.
    */
    return 0;
}

//避免高频操作
static int s_set_utc_times = 0;
static int s_set_timezone_times = 0;

int tuya_devinter_set_timezone_cb(int timenoze)
{
    if (s_set_timezone_times++ >= 1){
        return 0;
    }
    PR_DEBUG("timenoze:%d\n", timenoze);
    /* TODO:
     * set system timezone 
    */
    return 0;
}
    
int tuya_devinter_set_utctime_cb(unsigned int utc_time)
{
    if (s_set_utc_times++ >= 1){
        return 0;
    }
    PR_DEBUG("utc_time:%u\n", utc_time);
	//设置系统时间
	ty_time_set_systime(utc_time);
	//设置sdk时间
	tuya_ipc_set_service_time(utc_time);
    /* TODO:
	 * update rtc time 
     */
    return 0;
}

int tuya_devinter_set_wifi_pattern_cb(char *wakeup_pattern, unsigned int wakeup_pattern_len, 
                                                    char *keepalive_pattern, unsigned int keepalive_pattern_len)
{
    //RESERVED and UNUSED.
    return 0;
}

//keepalive event handle callback function.													
int keepalive_event_cb(KEEPALIVE_EVENT_TYPE_E evt_type) 
{
	if (evt_type == WAKEUP_PKG) {
		tuya_device_wakeup_from_keepalive();
		// TODO: do something after wakeup 
	} else if (evt_type == CONNECT_ERROR){
		// TODO: reconnect gateway
	} else {
		// TODO: other event
	}
	return 0;
}

//set the utc time in rtc
int rtc_set_cb(unsigned long long utc_time)
{
	PR_DEBUG("utc_time:%llu from gateway\r\n",utc_time);
	// TODO: Set UTC time from gateway into RTC
}
													
/*休眠回调
* cmd=0时代表向系统下发休眠指令,休眠原因为reason
* cmd=1时代表向系统询问是否可以休眠,如果可以休眠返回0,如果现在还不能休眠,则返回负值
*/
int tuya_devinter_sleep_cb(int cmd,TY_USER_DEVINTER_SLEEP_REASON_E reason)
{
    //如果是强供电, 直接 return 0
	int ret = 0;
	if(cmd == 0) {
		/* TODO:
		 * 先判断是否能休眠,比如正在ota升级,直接 return 0
		*/
	    PR_DEBUG("enter to sleep,reason:%d\n",reason);  
		//关闭唤醒状态下与基站的sokcet连接以及task
		ty_user_devinter_proto_uninit();
		ty_user_devinter_close_fd();
		/* TODO:
		 * 调用休眠接口,开始进行休眠保活
		 * 以下仅演示休眠保活接口用法,调用位置需根据实际情况而定，
		 * 一般和唤醒状态下的程序不在一个芯片上
		*/
		
		TY_KEEPALIVE_CALLBACK_S keepalive_cbs={0};
		keepalive_cbs.keepalive_event_cb = keepalive_event_cb;
		keepalive_cbs.keepalive_rtc_cb = rtc_set_cb;
		tuya_keepalive_init(&keepalive_cbs); 
		
		TY_USER_GW_CFG_T demo_gw_cfg;
		BOOL_T demo_binded;
		demo_read_config(&demo_binded, demo_gw_cfg.aeskey, demo_gw_cfg.bind_gw_uuid, demo_gw_cfg.wakeup_pattern, demo_gw_cfg.keepalive_pattern);
		uint32_t gw_addr_u32 = PP_HTONL(demo_gw_addr);
	    tuya_device_sleep_establish_keepalive_to_gw(inet_ntoa(gw_addr_u32), TUYA_IPCSET_KEEPALIVE_PORT, demo_gw_cfg.keepalive_pattern, demo_gw_cfg.aeskey, 2);
	} else {
		/* TODO:
		 * 根据当前系统状态判断是否可以休眠,
		 * 如果能休眠 ret = 0
		 * 如果不能休眠 ret = -1
		 */
	}
	
	return ret;
}

int tuya_devinter_start_av_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
}

int tuya_devinter_stop_av_cb(void)
{
    //RESERVED and UNUSED.
    return 0;
}

//门铃呼叫留言回调
int tuya_devinter_leave_msg_cb(char *status)
{
    //RESERVED and UNUSED.
    return 0;
}

/*门铃呼叫接听回调
* status为"accept"时代表app已经接听
* status为"stop"时代表app接听后挂断
* status为"heartbeat"时代表app持续接听中,每10秒一次,超过10秒没收到该心跳包可认为已经挂断
*/
int tuya_devinter_doorbell_status_cb(char *status)
{
    if(status == NULL){
        PR_ERR("status is NULL\n");
        return -1;
    }
    PR_DEBUG("status:%s \n", status);
    return 0;
}

int tuya_devinter_reset_wifi_config_cb(void)
{
	/* TODO:
	 * This callback will be invoked when ipc socket connect gateway failed and not bind yet
	 * Customer should erase wifi config(ssid and passwd)
	 */
	 
	return 0;
}

//TODO: 根据不同平台差异性实作回调函式
void tuya_devinter_proto_cb_init(TY_USER_DEVINTER_CALLBACK_S * pdevinter_cbs)
{
    pdevinter_cbs->bind_cb = tuya_devinter_bind_cb;
    pdevinter_cbs->bindFail_cb = tuya_devinter_bind_fail_cb;
    pdevinter_cbs->unbind_cb = tuya_devinter_unbind_cb;
    pdevinter_cbs->deepsleep_cb = tuya_devinter_deepsleep_cb;
    pdevinter_cbs->get_wake_reason_cb = tuya_devinter_get_wake_reason_cb;
    pdevinter_cbs->gw_disconn_cb = tuya_devinter_gw_disconn_cb;
    pdevinter_cbs->gw_reconn_cb = tuya_devinter_gw_reconn_cb;
    pdevinter_cbs->ota_stat_cb =  tuya_devinter_ota_stat_cb;
    pdevinter_cbs->handle_otaData_cb = tuya_devinter_ota_data_cb;
    pdevinter_cbs->handle_pingpong_cb = tuya_devinter_pingpong_cb;
    pdevinter_cbs->query_cb = tuya_devinter_query_cb;
    pdevinter_cbs->reboot_cb = tuya_devinter_reboot_cb;
    pdevinter_cbs->recv_audio_cb = tuya_devinter_recv_audio_cb;
    pdevinter_cbs->set_speaker_cb = tuya_devinter_set_speaker_cb;
    pdevinter_cbs->set_timezone_cb = tuya_devinter_set_timezone_cb;
    pdevinter_cbs->set_utctime_cb = tuya_devinter_set_utctime_cb;
    pdevinter_cbs->set_wifi_pattern_cb = tuya_devinter_set_wifi_pattern_cb;
    pdevinter_cbs->sleep_cb = tuya_devinter_sleep_cb;
    pdevinter_cbs->start_av_cb = tuya_devinter_start_av_cb;
    pdevinter_cbs->stop_av_cb = tuya_devinter_stop_av_cb;
    pdevinter_cbs->leave_msg_cb = tuya_devinter_leave_msg_cb;
    pdevinter_cbs->doorbell_status_cb = tuya_devinter_doorbell_status_cb;
	pdevinter_cbs->handle_dp_cb = tuya_devinter_handle_dp_cb;
	pdevinter_cbs->reset_wifi_config_cb = tuya_devinter_reset_wifi_config_cb;
}

//for demo, implement a simple config manager
void demo_read_config(BOOL_T* demo_binded, BYTE_T *demo_aeskey, CHAR_T *demo_gw_uuid, CHAR_T *demo_wakeup_pattern, CHAR_T *demo_keerpalive_pattern)
{
	TUYA_FILE *f = tuya_hal_fopen("./devinter_demo.cfg", "r");
	if (f == NULL) {
		PR_ERR("failed to open config file.");
		return;
	}

	CHAR_T buf[2048] = {0};
	tuya_hal_fread(buf, sizeof(buf), f);
	cJSON *root = cJSON_Parse(buf);
	if (root) {
		cJSON *obj = cJSON_GetObjectItem(root,"binded");
		if(obj) {
			*demo_binded = obj->valueint;
		}
	
		obj = cJSON_GetObjectItem(root, "aeskey");
		if (obj) {
			tuya_base64_decode(obj->valuestring, (BYTE_T*) demo_aeskey);
		}

		obj = cJSON_GetObjectItem(root, "gw_uuid");
		if (obj) {
			strncpy(demo_gw_uuid, obj->valuestring, 32);
		}

		obj = cJSON_GetObjectItem(root, "wakeup_pattern");
		if (obj) {
			strncpy(demo_wakeup_pattern, obj->valuestring, 32);
		}

		obj = cJSON_GetObjectItem(root, "keerpalive_pattern");
		if (obj) {
			strncpy(demo_keerpalive_pattern, obj->valuestring, 32);
		}

		cJSON_Delete(root);
	} else {
		PR_ERR("failed to parse config file.");
	}

	tuya_hal_fclose(f);
}

void demo_write_config(BOOL_T demo_binded,BYTE_T *demo_aeskey, CHAR_T *demo_gw_uuid, CHAR_T *demo_wakeup_pattern, CHAR_T *demo_keerpalive_pattern)
{
	TUYA_FILE *f = tuya_hal_fopen("./devinter_demo.cfg", "w");
	if (f == NULL) {
		PR_ERR("failed to open config file.");
		return;
	}

	cJSON *root = cJSON_CreateObject();
	if (root) {
		char tmp[64] = {0};
		cJSON_AddItemToObject(root,"binded",cJSON_CreateNumber(demo_binded));
		tuya_base64_encode(demo_aeskey, tmp, 16);
		cJSON_AddItemToObject(root, "aeskey", cJSON_CreateString(tmp));
		cJSON_AddItemToObject(root, "gw_uuid", cJSON_CreateString(demo_gw_uuid));
		cJSON_AddItemToObject(root, "wakeup_pattern", cJSON_CreateString(demo_wakeup_pattern));
		cJSON_AddItemToObject(root, "keerpalive_pattern", cJSON_CreateString(demo_keerpalive_pattern));
		char *cfg = cJSON_Print(root);
		if (cfg) {
			tuya_hal_fwrite(cfg, strlen(cfg) + 1, f);
			tuya_hal_fclose(f);
			free(cfg);
			cJSON_Delete(root);
			return;
		}
		cJSON_Delete(root);
	}

	PR_ERR("failed to write config file.");
}


