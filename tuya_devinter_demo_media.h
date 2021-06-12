/*********************************************************************************
  *Copyright(C),2017, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_devinter_demo_media.h
**********************************************************************************/

#ifndef __TUYA_DEVINTER_DEMO_MEDIA_H__
#define __TUYA_DEVINTER_DEMO_MEDIA_H__

#include "tuya_ipc_media.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//for demo
#define  E_CHANNEL_VIDEO_SNAP 4
int demo_sd_init(void);
void demo_media_info_init(IPC_MEDIA_INFO_S* pMediaInfo);
int tuya_devinter_ring_buffer_create(void);
int tuya_devinter_av_start(void);
int tuya_devinter_event_start(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __TUYA_DEVINTER_DEMO_MEDIA_H__ */


