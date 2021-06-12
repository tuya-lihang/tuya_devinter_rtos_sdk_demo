/*********************************************************************************
  *Copyright(C),2017, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_devinter_demo_net.h
**********************************************************************************/

#ifndef __TUYA_DEVINTER_DEMO_NET_H__
#define __TUYA_DEVINTER_DEMO_NET_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int tuya_devinter_get_wifi_strength(void);
int tuya_devinter_wifi_all_ap_scan(char* scan_result,int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __TUYA_DEVINTER_DEMO_NET_H__ */

