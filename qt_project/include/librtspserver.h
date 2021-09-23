#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _LIBRTSPSERVER_H
#define _LIBRTSPSERVER_H

#define MAX_RTSP_CHANNELS			256
#define MAX_RTSP_SERVER_LINK_CNT	1024

#define	RTSP_H264	0
#define	RTSP_H265	1
#define	RTSP_MJPEG	2
#define RTSP_RAW    3
#define RTSP_AAC	0
#define RTSP_G711A	1
#define RTSP_G711U  2

#define R_FRAME_I   0
#define R_FRAME_P	1

	typedef struct
	{
		int m_iLinkChn;         //连接哪个通道
		int m_iStreamId;		//0:主码流，1：副码流 
		int m_iLinkMode;        //0:UDP,1:TCP
		int m_iClientPort;		//rtsp客户端端口
		char m_cClientIp[32];   //rtsp客户端IP
	}TRtspLinkClientMsg;


	typedef struct
	{
		int m_iClientCnt;
		TRtspLinkClientMsg m_tClientLinkMsg[MAX_RTSP_SERVER_LINK_CNT];
	}TRtspLinkClientMsgList;

	typedef int(*RtspServerVerifyUserCallback) (const char *_pcUserName, const char *_pcPassWord);
	typedef int(*TranslateChlAddrCallback) (const char *_pcAddr, int* _iChn);
	typedef void(*DebugOutputCallback) (const char *_pcMsg);
	typedef int(*OnRTSP_RequestIDR_Callback) (const int chn);

	/*******************************************************
	功能：启动rtspserver
	参数说明：
	_iRtspServerPort：监听端口号
	_iChnCnt：通道个数，最大256
	_iSubChnSupport：是否支持副码流，0：不支持，1：支持
	VerifyUserCallback：用户名密码校验回调函数，参数为解码后的明文，返回0表示检验通过，-1表示失败
	返回值：
		-1：失败
		0：成功
	********************************************************/

	int StartRTSPServer(int _iRtspServerPort, int _iChnCnt, int _iSubChnSupport, RtspServerVerifyUserCallback VerifyUserCallback);

	/*******************************************************
	功能：设置地址转换回调函数
	参数说明：
	Callback：将用户请求的Url地址转移成内部的通道号（从1开始），成功返回0，失败-1
	返回值：
		-1：失败
		0：成功
	********************************************************/

	int SetTranslateChlAddrCallback(TranslateChlAddrCallback Callback);

	/*******************************************************
	功能：设置日志回调函数
	参数说明：
	Callback：日志回调
	返回值：
	-1：失败
	0：成功
	********************************************************/

	int SetDebugOutputCallback(DebugOutputCallback Callback);

	/*******************************************************
	功能：设置请求关键帧回调函数
	参数说明：
	Callback：关键帧回调
	返回值：
	-1：失败
	0：成功
	********************************************************/

	int SetRequestIDRCallback(OnRTSP_RequestIDR_Callback Callback);


	/*******************************************************
	功能：发送视频流给rtspserver，由rtspserver封包并发给客户端
	参数说明：
	_iChn：通道，从零开始
	_iStreamId：0：主码流，1：副码流
	_iVencType：编码类型，0：H264，1：H265，2：MJPEG
	_iFrameType:帧类型，0：I帧，1：P帧
	_ullTimeStamp：时间戳，单位ms
	_iFrameSize：帧大小
	_pucFrameBuf：帧数据
	返回值：
		-1：失败
		0：成功
	********************************************************/

	int SendVideoStreamToRtspServer(int _iChn, int _iStreamId, int _iVencType, int _iFrameType, unsigned long long _ullTimeStamp,
		int _iFrameSize, unsigned char *_pucFrameBuf);

	/*******************************************************
	功能：发送音频流给rtspserver，由rtspserver封包并发给客户端
	参数说明：
	_iChn：通道，从零开始
	_iStreamId：0：主码流，1：副码流
	_iVencType：编码类型，0：AAC
	_iSampleRate：采样率，G711 需要设置，AAC 忽略
	_iAudioChnCnt：音频通道数，G711 需要设置，AAC 忽略
	_ullTimeStamp：时间戳，单位ms
	_iFrameSize：帧大小
	_pucFrameBuf：帧数据
	返回值：
	-1：失败
	0：成功
	********************************************************/

	int SendAudioStreamToRtspServer(int _iChn, int _iStreamId, int _iVencType, int _iSampleRate, int _iAudioChnCnt,
		unsigned long long _ullTimeStamp, int _iFrameSize, unsigned char *_pucFrameBuf);

	/*******************************************************
	功能：获取rtspserver连接数
	参数说明：
	_iChn：通道，从零开始
	_iStreamId：0：主码流，1：副码流

	返回值：
		-1：失败
		大于零：连接个数
	********************************************************/

	int GetRtspServerConnectCnt(int _iChn, int _iStreamId);

	/*******************************************************
	功能：设置rtspserver端口号
	参数说明：
	_iPort：端口

	返回值：
	-1：失败
	0：成功
	********************************************************/

	int SetRtspServerPort(int _iPort);


	/*******************************************************
	功能：停止rtspserver
	参数说明：

	返回值：
	-1：失败
	0：成功
	********************************************************/
	int StopRtspServer();


	/*******************************************************
	功能：获取rtsp客户端连接列表
	参数说明：
	_ptClientList：存放客户端连接信息

	返回值：
	-1：失败
	大于零：成功，客户端连接个数
	********************************************************/
	int GetRtspServerConnectMsg(TRtspLinkClientMsgList *_ptClientList);

#endif

#ifdef __cplusplus
}
#endif