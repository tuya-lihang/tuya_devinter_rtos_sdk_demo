#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tuya_cloud_com_defs.h>
#include "cJSON.h"
#include "uni_log.h"
#include "tuya_devinter_demo_dp.h"

#ifdef TUYA_DP_LIGHT
/* 状态指示灯,BOOL类型,true打开,false关闭 */
void tuya_devinter_set_light_onoff(int light_on_off)
{
    PR_DEBUG("set light_on_off:%d \r\n", light_on_off);
    /* TODO:
     * handle this dp and save to flash.
     */
}

int tuya_devinter_get_light_onoff(void)
{
    int light_on_off = TRUE;
    /* TODO:
     * get from flash.
     */
    return light_on_off;
}
#endif

#ifdef TUYA_DP_FLIP
/* 画面翻转,BOOL类型,true打开,false关闭 */
void tuya_devinter_set_flip_onoff(int flip_on_off)
{
    PR_DEBUG("set_flip_onoff: %d \r\n",flip_on_off);
    /* TODO:
     * handle this dp and save to flash.
     */
}

int tuya_devinter_get_flip_onoff(void)
{
    int flip_on_off = FALSE;
    /* TODO:
     * get from flash.
     */
    return flip_on_off;
}
#endif

#ifdef TUYA_DP_WATERMARK
/* 是否打开水印,BOOL类型,true打开,false关闭 */
void tuya_devinter_set_watermark_onoff(int watermark_on_off)
{
    PR_DEBUG("set_watermark_on_off: %d \r\n",watermark_on_off);
    /* TODO:
     * handle this dp and save to flash.
     */
}

int tuya_devinter_get_watermark_onoff(void)
{
    int watermark_on_off = TRUE;
    /* TODO:
     * get from flash.
     */
    return watermark_on_off;
}
#endif

#ifdef TUYA_DP_ALARM_FUNCTION
/* 是否打开报警,BOOL类型,true打开,false关闭 */
void tuya_devinter_set_alarm_function_onoff(int alarm_on_off)
{
    PR_DEBUG("set alarm_on_off:%d \r\n", alarm_on_off);
    /* TODO:
     * handle this dp and save to flash.
     */
    return;
}

int tuya_devinter_get_alarm_function_onoff(void)
{
	int alarm_on_off = TRUE;
    /* TODO:
     * get from flash.
     */
	return alarm_on_off;
}
#endif

#ifdef TUYA_DP_ALARM_SENSITIVITY
/* 移动侦测报警灵敏度,ENUM类型,0为低灵敏度,1为中灵敏度,2为高灵敏度*/
void tuya_devinter_set_alarm_sensitivity(char *p_sensitivity)
{
    if (NULL == p_sensitivity){
        PR_ERR("%s[%d] input failed\n",__FUNCTION__,__LINE__);
        return ;
    }
    int sens = atoi(p_sensitivity);
    PR_DEBUG("set alarm_sensitivity:%d \r\n", sens);
    /* TODO:
     * handle this dp and save to flash.
     */
}

static char alarm_sensitivity[4] = {0};
char *tuya_devinter_get_alarm_sensitivity(void)
{
    int sens = 0;
    /* TODO:
     * get from flash.
     */
    snprintf(alarm_sensitivity, 4, "%d",sens);
    return alarm_sensitivity;
}
#endif

#ifdef TUYA_DP_NIGHT_MODE
/* 红外夜视功能,ENUM类型,0-自动 1-关 2-开 */  
void tuya_devinter_set_night_mode(char *p_night_mode)
{
    if (NULL == p_night_mode){
        PR_ERR("%s[%d] input failed\n",__FUNCTION__,__LINE__);
        return ;
    }
    int mode = atoi(p_night_mode);
    PR_DEBUG("set night_mode:%d \r\n", mode);
    /* TODO:
     * handle this dp and save to flash.
     */
}

static char s_dn_mode[4];
char *tuya_devinter_get_night_mode(void)
{
    int mode = 0;
    /* TODO:
     * get from flash.
     */
    snprintf(s_dn_mode, 4, "%d",mode);
    PR_DEBUG("curr dn mode:%s \r\n", s_dn_mode);
    return s_dn_mode;
}
#endif

#ifdef TUYA_DP_ELECTRICITY
/* 电池电量(百分比),VALUE类型,[0-100] */
int tuya_devinter_get_battery_percent(void)
{
    //TODO

    return 80;
}
#endif

#ifdef TUYA_DP_POWERMODE
/* 供电方式,ENUM类型,"0"为电池供电状态,"1"为插电供电状态(或电池充电状态) */
static char s_pow_mode[4] = {0};
char *tuya_devinter_get_power_mode(void)
{
    int pow_mode = 0;
    //TODO
    snprintf(s_pow_mode,4,"%d",pow_mode);
    PR_DEBUG("report power mode[%s]\n",s_pow_mode);

    return s_pow_mode;
}
#endif

#ifdef TUYA_DP_LOWELECTRIC
/* 低电量阈值设置 */
void tuya_devinter_set_low_electric(int value)
{
    PR_DEBUG("set low electric:%d\n",value);

    /* TODO:
     * handle this dp and save to flash.
     */
}

int tuya_devinter_get_low_electric(void)
{
    int value = 20;
    /* TODO:
     * get from flash.
     */
    return value;
}
#endif

#ifdef TUYA_DP_ELECTRICITY_CAP
/*电池电量精准上报能力,0: 无法精准上报, 1:可精准上报电量*/
int tuya_devinter_get_accuracy_electric(void)
{
	int value = 1;
    //TODO
	return value;
}
#endif

#ifdef TUYA_DP_ALARM_ZONE_ENABLE
/* 移动侦测区域设置开关使能,BOOL类型,true打开,false关闭*/
void tuya_devinter_set_alarm_zone_onoff(int alarm_zone_on_off)
{
    PR_DEBUG("set alarm_zone_on_off:%d \r\n", alarm_zone_on_off);
    /* TODO:
     * handle this dp and save to flash.
     */
    return ;
}

int tuya_devinter_get_alarm_zone_onoff(void)
{
    int alarm_zone_on_off = TRUE;
    /* TODO:
     * get from flash.
     */
    return alarm_zone_on_off;
}
#endif

#ifdef TUYA_DP_ALARM_ZONE_DRAW
/* 移动侦测区域设置,JSON类型,格式如下：
"{\\\"num\\\":1,\\\"region0\\\":{\\\"x\\\":34,\\\"y\\\":36,\\\"xlen\\\":66,\\\"ylen\\\":64}}"
*/
void tuya_devinter_set_alarm_zone_draw(char * p_alarm_zone)
{
    if (NULL == p_alarm_zone){
        return ;
    }
	
    PR_DEBUG("set alarm_zone_set:%s \r\n",p_alarm_zone);
    /* TODO:
     * handle this dp and save to flash.
     */

    return ;
}

static char s_alarm_zone[256] = {0};
char * tuya_devinter_get_alarm_zone_draw(void)
{
    /* TODO:
     * get from flash.
     */
    return s_alarm_zone;
}
#endif

#ifdef TUYA_DP_HUM_FILTER
/* 人形检测开关,BOOL类型,true打开,false关闭 */
void tuya_devinter_set_human_filter(int filter_enable)
{
    PR_DEBUG("human filter_enable %d\r\n",filter_enable);
    /* TODO:
     * handle this dp and save to flash.
     */
}
int tuya_devinter_get_human_filter(void)
{
    int filter_enable = FALSE;
    /* TODO:
     * get from flash.
     */
    return filter_enable;
}
#endif

int dp_sync_json_create(cJSON ** dp_array)
{
	cJSON *dp = NULL;
	*dp_array = cJSON_CreateArray();
    if (*dp_array == NULL) {
        PR_DEBUG("cJSON_CreateObject failed.\n");
        return -1;
    }

#ifdef TUYA_DP_LIGHT
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_LIGHT));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(tuya_devinter_get_light_onoff()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_SLEEP_MODE
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_SLEEP_MODE));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(0));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_FLIP
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_FLIP));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(tuya_devinter_get_flip_onoff()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_WATERMARK
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_WATERMARK));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(tuya_devinter_get_watermark_onoff()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_ALARM_FUNCTION
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_ALARM_FUNCTION));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(tuya_devinter_get_alarm_function_onoff()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_ALARM_SENSITIVITY
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_ALARM_SENSITIVITY));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_ENUM));
	cJSON_AddItemToObject(dp, "dp_value_str", cJSON_CreateString(tuya_devinter_get_alarm_sensitivity()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_NIGHT_MODE
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_NIGHT_MODE));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_ENUM));
	cJSON_AddItemToObject(dp, "dp_value_str", cJSON_CreateString(tuya_devinter_get_night_mode()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_LOWELECTRIC
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_LOWELECTRIC));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_VALUE));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateNumber(tuya_devinter_get_low_electric()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_CONTINUE_WORK
	char val_enum[2] = {0};
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_CONTINUE_WORK));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_ENUM));
	val_enum[0] = '0';
	cJSON_AddItemToObject(dp, "dp_value_str", cJSON_CreateString(val_enum));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_ELECTRICITY_CAP
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_ELECTRICITY_CAP));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_VALUE));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateNumber(tuya_devinter_get_accuracy_electric()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_ALARM_ZONE_ENABLE
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_ALARM_ZONE_ENABLE));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(tuya_devinter_get_alarm_zone_onoff()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

#ifdef TUYA_DP_HUM_FILTER
	dp = cJSON_CreateObject();
	cJSON_AddItemToObject(dp, "dp_id", cJSON_CreateNumber(TUYA_DP_HUM_FILTER));
	cJSON_AddItemToObject(dp, "dp_prop", cJSON_CreateNumber(PROP_BOOL));
	cJSON_AddItemToObject(dp, "dp_value_int", cJSON_CreateBool(tuya_devinter_get_human_filter()));
	cJSON_AddItemToArray(*dp_array, dp);
#endif

	return 0;
}

int dp_response_json_create(cJSON **param, int dp_id, void *value, int prop, int write_dp)
{
    *param = cJSON_CreateObject();
    if (*param == NULL) {
        PR_DEBUG("cJSON_CreateObject failed.\n");
        return -1;
    }

    cJSON_AddItemToObject(*param, "TUYA_DP_ID", cJSON_CreateNumber(dp_id));

    //handle read dp case
    if (write_dp == 0) {
        switch (prop) {
        case PROP_BOOL:
            cJSON_AddItemToObject(*param, "prop", cJSON_CreateNumber(PROP_BOOL));
            cJSON_AddItemToObject(*param, "value", cJSON_CreateBool((long)value));
            break;
        case PROP_VALUE:
            cJSON_AddItemToObject(*param, "prop", cJSON_CreateNumber((int)PROP_VALUE));
            cJSON_AddItemToObject(*param, "value", cJSON_CreateNumber((long)value));
            break;
        case PROP_STR:
            cJSON_AddItemToObject(*param, "prop", cJSON_CreateNumber((int)PROP_STR));
            cJSON_AddItemToObject(*param, "value", cJSON_CreateString((char*)value));
            break;
        case PROP_ENUM:
            cJSON_AddItemToObject(*param, "prop", cJSON_CreateNumber((int)PROP_ENUM));
            cJSON_AddItemToObject(*param, "value", cJSON_CreateString((char*)value));
            break;
        case PROP_BITMAP:
            PR_DEBUG("unsupport dp type\n");
            break;
        }
    }

    return 0;
}

int tuya_devinter_handle_dp_cb(int dp_id, int write_dp, void *value, cJSON** param)
{
	if(!param || (write_dp && !value))  {
        PR_ERR("value or param is NULL\n");
        return -1;
	}
	
    PR_DEBUG("process_dp %d, write_dp:%d\n", dp_id, write_dp);
	
    switch (dp_id) {
#ifdef TUYA_DP_LIGHT
    case TUYA_DP_LIGHT:
        if (write_dp) {
            int onoff = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_LIGHT %d\n", onoff);
            tuya_devinter_set_light_onoff(onoff);
            dp_response_json_create(param, TUYA_DP_LIGHT, NULL, PROP_BOOL, write_dp);
        } else {
            long val = tuya_devinter_get_light_onoff();
            dp_response_json_create(param, TUYA_DP_LIGHT, (void*)val, PROP_BOOL, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_FLIP
    case TUYA_DP_FLIP:
        if (write_dp) {
            int onoff = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_FLIP %d\n", onoff);
            tuya_devinter_set_flip_onoff(onoff);
            dp_response_json_create(param, TUYA_DP_FLIP, NULL, PROP_BOOL, write_dp);
        } else {
            long val = tuya_devinter_get_flip_onoff();
            dp_response_json_create(param, TUYA_DP_FLIP, (void*)val, PROP_BOOL, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_NIGHT_MODE
    case TUYA_DP_NIGHT_MODE:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_NIGHT_MODE %d\n", val);
			char str_val[32] = {0};
			snprintf(str_val,sizeof(str_val)-1,"%d",val);
			tuya_devinter_set_night_mode(str_val);
            dp_response_json_create(param, TUYA_DP_NIGHT_MODE, NULL, PROP_ENUM, write_dp);
        } else {
            char str_val[2] = {0};
			int val = atoi(tuya_devinter_get_night_mode());
            str_val[0] = '0' + val;
            PR_DEBUG("TUYA_DP_NIGHT_MODE read_dp %s\n", str_val);
            dp_response_json_create(param, TUYA_DP_NIGHT_MODE, (void*)str_val, PROP_ENUM, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_WATERMARK
    case TUYA_DP_WATERMARK:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_WATERMARK %d\n", val);
            tuya_devinter_set_watermark_onoff(val);
            dp_response_json_create(param, TUYA_DP_WATERMARK, NULL, PROP_BOOL, write_dp);
        } else {
            long val = tuya_devinter_get_watermark_onoff();
            dp_response_json_create(param, TUYA_DP_WATERMARK, (void*)val, PROP_BOOL, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_ALARM_FUNCTION
    case TUYA_DP_ALARM_FUNCTION:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_ALARM_FUNCTION %d\n", val);
            tuya_devinter_set_alarm_function_onoff(val);
            dp_response_json_create(param, TUYA_DP_ALARM_FUNCTION, NULL, PROP_BOOL, write_dp);
        } else {
            long val = tuya_devinter_get_alarm_function_onoff();
            dp_response_json_create(param, TUYA_DP_ALARM_FUNCTION, (void*)val, PROP_BOOL, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_ALARM_SENSITIVITY
    case TUYA_DP_ALARM_SENSITIVITY:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_ALARM_SENSITIVITY %d\n", val);
			char str_val[32] = {0};
			snprintf(str_val,sizeof(str_val),"%d",val);
            tuya_devinter_set_alarm_sensitivity(str_val);
            dp_response_json_create(param, TUYA_DP_ALARM_SENSITIVITY, NULL, PROP_ENUM, write_dp);
        } else {
			int val = atoi(tuya_devinter_get_alarm_sensitivity());
            char str_val[2] = {0};
            str_val[0] = '0' + val;
            PR_DEBUG("TUYA_DP_ALARM_SENSITIVITY read_dp %s\n", str_val);
            dp_response_json_create(param, TUYA_DP_ALARM_SENSITIVITY, (void*)str_val, PROP_ENUM, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_LOWELECTRIC
    case TUYA_DP_LOWELECTRIC:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_LOWELECTRIC %d\n", val);
            tuya_devinter_set_low_electric(val);
            dp_response_json_create(param, TUYA_DP_LOWELECTRIC, NULL, PROP_VALUE, write_dp);
        } else {
            long val = tuya_devinter_get_low_electric();
            PR_DEBUG("TUYA_DP_LOWELECTRIC read_dp %d\n", val);
            dp_response_json_create(param, TUYA_DP_LOWELECTRIC, (void*)val, PROP_VALUE, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_ALARM_ZONE_ENABLE
    case TUYA_DP_ALARM_ZONE_ENABLE:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_ALARM_ZONE_ENABLE %d\n", val);
            tuya_devinter_set_alarm_zone_onoff(val);
            dp_response_json_create(param, TUYA_DP_ALARM_ZONE_ENABLE, NULL, PROP_BOOL, write_dp);
        } else {
            long val = tuya_devinter_get_alarm_zone_onoff();
            PR_DEBUG("TUYA_DP_ALARM_ZONE_ENABLE read_dp %d\n", val);
            dp_response_json_create(param, TUYA_DP_ALARM_ZONE_ENABLE, (void*)val, PROP_BOOL, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_ALARM_ZONE_DRAW
    case TUYA_DP_ALARM_ZONE_DRAW:
        if (write_dp) {
            PR_DEBUG("TUYA_DP_ALARM_ZONE_DRAW %s\n", ((cJSON *)value)->valuestring);
            tuya_devinter_set_alarm_zone_draw(((cJSON *)value)->valuestring);
            dp_response_json_create(param, TUYA_DP_ALARM_ZONE_DRAW, NULL, PROP_STR, write_dp);
        } else {
            PR_DEBUG("TUYA_DP_ALARM_ZONE_DRAW read_dp %s\n", tuya_devinter_get_alarm_zone_draw());
            dp_response_json_create(param, TUYA_DP_ALARM_ZONE_DRAW, (void*)tuya_devinter_get_alarm_zone_draw(), PROP_STR, write_dp);
        }
        break;
#endif
#ifdef TUYA_DP_HUM_FILTER
    case TUYA_DP_HUM_FILTER:
        if (write_dp) {
            int val = ((cJSON *)value)->valueint;
            PR_DEBUG("TUYA_DP_HUM_FILTER %d\n", val);
            tuya_devinter_set_human_filter(val);
            dp_response_json_create(param, TUYA_DP_HUM_FILTER, NULL, PROP_BOOL, write_dp);
        } else {
            long val = tuya_devinter_get_human_filter();
            PR_DEBUG("TUYA_DP_HUM_FILTER read_dp %d\n", val);
            dp_response_json_create(param, TUYA_DP_HUM_FILTER, (void*)val, PROP_BOOL, write_dp);
        }
        break;
#endif

    default:
        PR_DEBUG("Unknown dpid: %d\n", dp_id);
        return -1;
    }

    return 0;
}

