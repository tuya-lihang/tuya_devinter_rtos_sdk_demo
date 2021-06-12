#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "uni_log.h"
#include "ty_user_devinter_proto.h"
#include "tuya_ipc_media.h"
#include "tuya_ring_buffer.h"
#include "tuya_devinter_demo_media.h"
#include "tuya_hal_thread.h"
#include "tuya_hal_memory.h"
#include "tuya_hal_system.h"
#include "uni_thread.h"
#include "fatfs_sdcard_api.h"
#include "tuya_hal_fs.h"
#define AUDIO_FRAME_SIZE 640
#define AUDIO_FPS 25
#define VIDEO_BUF_SIZE	(1024 * 400) //Maximum frame

char sd_base_path[128] = {0};

THREAD_HANDLE h264_output_thread;
THREAD_HANDLE pcm_output_thread;
THREAD_HANDLE test_event_thread;

int demo_sd_init()
{
    uint32_t ret = 0;
	fatfs_sd_params_t fatfs_sd;

    if (!fatfs_sd_is_inited()) {
		ret = fatfs_sd_init();
		if (ret != 0){
			PR_ERR("\n\r[%s] fatfs_sd_init fail (%d)\r\n", __FUNCTION__, ret);
			return -1;
		}

        ret = fatfs_sd_get_param(&fatfs_sd);
        if (ret != 0){
            PR_ERR("\n\r[%s] fatfs_sd_get_param fail (%d)\n", __FUNCTION__, ret);
            return -1;
        }
		
        strcpy(sd_base_path, fatfs_sd.drv);
		PR_DEBUG("\n\rsd_base_path:%s\r\n", sd_base_path);
	}

    return 0;
}

//for demo
void demo_media_info_init(IPC_MEDIA_INFO_S* pMediaInfo)
{
    if(NULL == pMediaInfo)
    {
        PR_ERR("para is NULL\n");
        return;
    }
	
	/* 主码流 */
	pMediaInfo->channel_enable[E_CHANNEL_VIDEO_MAIN] = TRUE;
	pMediaInfo->video_fps[E_CHANNEL_VIDEO_MAIN] = 30;
	pMediaInfo->video_gop[E_CHANNEL_VIDEO_MAIN] = 30;
	pMediaInfo->video_bitrate[E_CHANNEL_VIDEO_MAIN] = 384;
	pMediaInfo->video_width[E_CHANNEL_VIDEO_MAIN] = 640;
	pMediaInfo->video_height[E_CHANNEL_VIDEO_MAIN] = 360;
	pMediaInfo->video_codec[E_CHANNEL_VIDEO_MAIN] = TUYA_CODEC_VIDEO_H264;
	
	/* 子码流 */
	pMediaInfo->channel_enable[E_CHANNEL_VIDEO_SUB] = TRUE;
	pMediaInfo->video_fps[E_CHANNEL_VIDEO_SUB] = 30;
	pMediaInfo->video_gop[E_CHANNEL_VIDEO_SUB] = 30;
	pMediaInfo->video_bitrate[E_CHANNEL_VIDEO_SUB] = 384;
	pMediaInfo->video_width[E_CHANNEL_VIDEO_SUB] = 640;
	pMediaInfo->video_height[E_CHANNEL_VIDEO_SUB] = 360;
	pMediaInfo->video_codec[E_CHANNEL_VIDEO_SUB] = TUYA_CODEC_VIDEO_H264;

	/* 抓图 */
	pMediaInfo->channel_enable[E_CHANNEL_VIDEO_SNAP] = TRUE;
	pMediaInfo->video_fps[E_CHANNEL_VIDEO_SNAP] = 3;
	pMediaInfo->video_gop[E_CHANNEL_VIDEO_SNAP] = 6;
	pMediaInfo->video_bitrate[E_CHANNEL_VIDEO_SNAP] = 384;
	pMediaInfo->video_width[E_CHANNEL_VIDEO_SNAP] = 640;
	pMediaInfo->video_height[E_CHANNEL_VIDEO_SNAP] = 360;
	pMediaInfo->video_codec[E_CHANNEL_VIDEO_SNAP] = TUYA_CODEC_VIDEO_MJPEG;
	
	/* 主音频 */
	pMediaInfo->channel_enable[E_CHANNEL_AUDIO] = TRUE;
	pMediaInfo->audio_sample[E_CHANNEL_AUDIO] = 8000;
	pMediaInfo->audio_databits[E_CHANNEL_AUDIO] = 16;
	pMediaInfo->audio_fps[E_CHANNEL_AUDIO] = 25;

}


int tuya_devinter_ring_buffer_create()
{
    OPERATE_RET ret = OPRT_OK;
    CHANNEL_E channel;
	IPC_MEDIA_INFO_S media_info;
	memset(&media_info,0,sizeof(media_info));
	demo_media_info_init(&media_info);
	
    for( channel = E_CHANNEL_VIDEO_MAIN; channel < E_CHANNEL_MAX; channel++ )
    {
        //PR_DEBUG("init ring buffer Channel:%d Enable:%d\n", channel, media_info.channel_enable[channel]);
        if(media_info.channel_enable[channel] == TRUE)
        {
            if(channel == E_CHANNEL_AUDIO)
            {
                //PR_DEBUG("audio_sample %d, audio_databits %d, audio_fps %d\n",media_info.audio_sample[E_CHANNEL_AUDIO],media_info.audio_databits[E_CHANNEL_AUDIO],media_info.audio_fps[E_CHANNEL_AUDIO]);
                ret = tuya_ipc_ring_buffer_init(channel, media_info.audio_sample[E_CHANNEL_AUDIO]*media_info.audio_databits[E_CHANNEL_AUDIO]/1024,media_info.audio_fps[E_CHANNEL_AUDIO],0,NULL);
            }
            else
            {
                //PR_DEBUG("video_bitrate %d, video_fps %d\n",media_info.video_bitrate[channel],media_info.video_fps[channel]);
                ret = tuya_ipc_ring_buffer_init(channel, media_info.video_bitrate[channel],media_info.video_fps[channel],0,NULL);
            }
            if(ret != 0)
            {
                PR_DEBUG("init ring buffer fails. %d %d\n", channel, ret);
                return OPRT_MALLOC_FAILED;
            }
            //PR_DEBUG("init ring buffer success. channel:%d\n", channel);
        }
    }

    return 0;
}

/* This is for demo only. Should be replace with real PCM encoder output */
void thread_live_audio(void *arg)
{
    char fullpath[128] = {0};
    sprintf(fullpath, "%s/resource/media/demo_audio.raw", sd_base_path);

    PR_DEBUG("start live audio using %s\n",fullpath);
    TUYA_FILE *aFp = tuya_hal_fopen(fullpath, "rb");
    if(aFp == NULL)
    {
        PR_ERR("can't read live audio file %s\n",fullpath);
        tuya_hal_thread_release(pcm_output_thread);
		return;
    }
    unsigned char audioBuf[AUDIO_FRAME_SIZE];
    MEDIA_FRAME_S pcm_frame = {0};
    pcm_frame.type = E_AUDIO_FRAME;

    while(1)
    {
        int size = tuya_hal_fread(audioBuf, AUDIO_FRAME_SIZE, aFp);
        if(size < AUDIO_FRAME_SIZE)
        {
			tuya_hal_fseek(aFp,0,TUYA_SEEK_SET);
            continue;
        }
        int frameRate = AUDIO_FPS;
        int sleepTick = 1000000/frameRate;
        static UINT64_T pts = 0;
        pts += sleepTick;
        pcm_frame.size = size;
        pcm_frame.p_buf = audioBuf;
        pcm_frame.pts = pts;
		pcm_frame.type = E_AUDIO_FRAME;

		tuya_ipc_ring_buffer_append_data(E_CHANNEL_AUDIO,pcm_frame.p_buf,pcm_frame.size,pcm_frame.type,pcm_frame.pts);
        tuya_hal_system_sleep(sleepTick/1000);
    }

    tuya_hal_thread_release(pcm_output_thread);
}

/* This is for demo only. Should be replace with real H264 encoder output */
int read_one_frame_from_demo_video_file(unsigned char *pVideoBuf,unsigned int offset,unsigned int BufSize,unsigned char *IskeyFrame,unsigned int 
*FramLen,unsigned int *Frame_start)
{
    int pos = 0;
    int bNeedCal = 0;
    unsigned char NalType=0;
    int idx=0;
    if(BufSize<=5)
    {
        PR_ERR("bufSize is too small\n");
        return -1;
    }
    for(pos=0;pos <= BufSize-5;pos++)
    {
        if(pVideoBuf[pos]==0x00
            &&pVideoBuf[pos+1]==0x00
            &&pVideoBuf[pos+2]==0x00
            &&pVideoBuf[pos+3]==0x01)
        {
            NalType = pVideoBuf[pos + 4] & 0x1f;
            if(NalType == 0x7)
            {
                if(bNeedCal==1)
                {
                    *FramLen=pos-idx;
                    return 0;
                }
                
                *IskeyFrame = 1;
                *Frame_start = offset+pos;
                bNeedCal=1;
                idx=pos;
            }
            else if(NalType ==0x1)                
            {
               if(bNeedCal)
               {
                  *FramLen=pos-idx;
                  return 0;
               }
               *Frame_start=offset+pos;
               *IskeyFrame = 0;
               idx = pos;
               bNeedCal=1;
            }
        }
    }
    
    return 0;
}

void thread_live_video(void *arg)
{
    char raw_fullpath[128] = {0};
    unsigned int FrameLen=0,Frame_start=0;
    unsigned int offset=0;
    unsigned char IsKeyFrame=0;
    unsigned char *pVideoBuf = NULL;
    sprintf(raw_fullpath, "%s/resource/media/demo_video.264", sd_base_path);

    PR_DEBUG("start live video using %s\n",raw_fullpath);

    TUYA_FILE *streamBin_fp = tuya_hal_fopen(raw_fullpath, "rb");
    if((streamBin_fp == NULL))
    {
        PR_ERR("can't read live video file %s\n",raw_fullpath);
        tuya_hal_thread_release(h264_output_thread);
		return;
    }
    tuya_hal_fseek(streamBin_fp, 0, TUYA_SEEK_END);
    UINT_T file_size = tuya_hal_ftell(streamBin_fp);
    tuya_hal_fseek(streamBin_fp, 0, TUYA_SEEK_SET);
	
    pVideoBuf = tuya_hal_system_malloc(file_size);
	if(pVideoBuf == NULL) {
        PR_ERR("tuya_hal_system_malloc failed\n");
        tuya_hal_thread_release(h264_output_thread);
		return;
	}
	
    tuya_hal_fread(pVideoBuf, file_size, streamBin_fp);

    MEDIA_FRAME_S h264_frame = {0};
    while(1)
    {
        offset=Frame_start+FrameLen;
        if(offset>=file_size)
        {
            offset=0;
        }
        read_one_frame_from_demo_video_file(pVideoBuf+offset,offset,file_size-offset,&IsKeyFrame,&FrameLen,&Frame_start);
        //Note: For I frame of H264, SPS/PPS/SEI/IDR should be combined within one frame, and the NALU separator should NOT be deleted.
        if(IsKeyFrame==1)
        {
            h264_frame.type = E_VIDEO_I_FRAME;
            h264_frame.size = FrameLen;
        }
        else
        {
            h264_frame.type = E_VIDEO_PB_FRAME;
            h264_frame.size = FrameLen;
        }
        int frameRate = 15;
        int sleepTick = 1000000/frameRate;
        static UINT64_T pts = 0;
        pts += sleepTick;
        h264_frame.p_buf = pVideoBuf+Frame_start;
        h264_frame.pts = pts;

        /* Send HD video data to the SDK */
		tuya_ipc_ring_buffer_append_data(E_CHANNEL_VIDEO_MAIN,h264_frame.p_buf,h264_frame.size,h264_frame.type,h264_frame.pts);

        /* Send SD video data to the SDK */
		tuya_ipc_ring_buffer_append_data(E_CHANNEL_VIDEO_SUB,h264_frame.p_buf,h264_frame.size,h264_frame.type,h264_frame.pts);

        tuya_hal_system_sleep(sleepTick/1000);
    }
	
	tuya_hal_system_free(pVideoBuf);
	tuya_hal_thread_release(h264_output_thread);
}

void thread_test_event(void *arg)
{
    tuya_hal_system_sleep(5*1000);

    char jpeg_fullpath[128] = {0};
    unsigned char *pPicBuf = NULL;

    sprintf(jpeg_fullpath, "%s/resource/media/demo_scence_snapshot.jpg", sd_base_path);
    PR_DEBUG("get jpeg using %s\n",jpeg_fullpath);
    TUYA_FILE *streamBin_fp = tuya_hal_fopen(jpeg_fullpath, "rb");
    if((streamBin_fp == NULL))
    {
        PR_ERR("can't read jpeg file %s\n",jpeg_fullpath);
        tuya_hal_thread_release(test_event_thread);
		return;
    }

    tuya_hal_fseek(streamBin_fp, 0, SEEK_END);
    UINT_T file_size = tuya_hal_ftell(streamBin_fp);
    tuya_hal_fseek(streamBin_fp, 0, SEEK_SET);
	
    pPicBuf = tuya_hal_system_malloc(file_size);
	if(pPicBuf == NULL) {
        PR_ERR("tuya_hal_system_malloc failed\n");
        tuya_hal_thread_release(test_event_thread);
		return;
		
	}
    tuya_hal_fread(pPicBuf, file_size, streamBin_fp);
    // report pir event to gw to start recording
    ty_user_devinter_report_event(TY_DEVINTER_EVENT_PIR_START, pPicBuf, file_size,0);
    tuya_hal_system_sleep(1000);

    /* if gw is already starting recording, this event will be skipped.
     * Then gw reset timer to extend recording time.
     */
    ty_user_devinter_report_event(TY_DEVINTER_EVENT_PIR_START, pPicBuf, file_size,0); 
    tuya_hal_system_sleep(1000);

    /* report pir stop event to gw to stop recording
     */
    ty_user_devinter_report_event(TY_DEVINTER_EVENT_PIR_STOP, NULL, 0,0);
    tuya_hal_system_sleep(1000);

	//report quantity of electricity to app
    ty_user_devinter_report_dp_val(145/*TUYA_DP_ELECTRICITY*/, 90);
    tuya_hal_system_sleep(1000);
	
	//reprot power supply mode to app, 0:battery 1:adapter
    ty_user_devinter_report_dp_val(146/*TUYA_DP_POWERMODE*/, 0);
    tuya_hal_system_sleep(1000);
	
	//report full battery event to app
	ty_user_devinter_report_event(TY_DEVINTER_EVENT_FULL_BATTERY,NULL,0,0);
	
	tuya_hal_system_free(pPicBuf);
	tuya_hal_thread_release(test_event_thread);
}

int tuya_devinter_av_start()
{
    /* 
        1. start your audio/video encoder, for example:
            return xxxxx_start_av_encoder();
        2. please call tuya_ipc_ring_buffer_append_data() if got audio / video data
    */

    //simulates audio and video by reading and writing files
    
	if(tuya_hal_thread_create(&h264_output_thread,"h264_output_thread",4096,TRD_PRIO_2,thread_live_video,NULL) < 0)
	{
		PR_ERR("h264_output_thread creat failed\n");
	}
	
	if(tuya_hal_thread_create(&pcm_output_thread,"pcm_output_thread",4096,TRD_PRIO_2,thread_live_audio,NULL) < 0)
	{
		PR_ERR("pcm_output_thread creat failed\n");
	}
	
    return 0;
}

int tuya_devinter_event_start()
{
    /* 
	 *	 This is for demo only. Should be replace with real event
    */
	
	if(tuya_hal_thread_create(&test_event_thread,"test_event_thread",4096,TRD_PRIO_2,thread_test_event,NULL) < 0)
	{
		PR_ERR("test_event_thread creat failed\n");
	}
	
	return 0;
}


