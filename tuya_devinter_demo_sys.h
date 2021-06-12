/*********************************************************************************
  *Copyright(C),2017, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_devinter_demo_sys.h
**********************************************************************************/

#ifndef __TUYA_DEVINTER_DEMO_SYS_H__
#define __TUYA_DEVINTER_DEMO_SYS_H__

#include <ty_user_devinter_proto.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void tuya_devinter_proto_cb_init(TY_USER_DEVINTER_CALLBACK_S * pdevinter_cbs);
void demo_read_config(BOOL_T* demo_binded, BYTE_T *demo_aeskey, CHAR_T *demo_gw_uuid, CHAR_T *demo_wakeup_pattern, CHAR_T *demo_keerpalive_pattern);
void demo_write_config(BOOL_T demo_binded,BYTE_T *demo_aeskey, CHAR_T *demo_gw_uuid, CHAR_T *demo_wakeup_pattern, CHAR_T *demo_keerpalive_pattern);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __TUYA_DEVINTER_DEMO_SYS_H__ */
