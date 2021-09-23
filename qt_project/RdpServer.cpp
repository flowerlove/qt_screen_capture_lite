#include "RdpServer.h"
#include "EncoderInternel.h"

RdpServer::RdpServer()
{
//    av_register_all();
    RdpThread = NULL;
    mMsgEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

RdpServer::~RdpServer()
{
    if (RdpThread != NULL)
    {
        mExitFlag = true;
        SetEvent(mMsgEvent);
        //等待发送线程退出
        WaitForSingleObject(RdpThread, INFINITE);
    }
    DestroyAllVideo();
    DestroyRtsp();
    CloseHandle(mMsgEvent);
    CloseHandle(RdpThread);

    StopRtspServer();
}

EnCodeType RdpServer::GetEnCodeId(EnCodeType encodeId)
{
    if (encodeId = ENCODE_FREE)
    {
        encodeId = ENCODE_SOFT;
    }
    return encodeId;
}

void RdpServer::AllEncoderInit()
{
    if (soft_video_encoder_)
    {
        soft_video_encoder_->Init();
    }

//    if (nvidia_video_encoder_)
//    {
//        nvidia_video_encoder_->Init();
//    }
}

void RdpServer::AllEncoderCommit()
{
    if (soft_video_encoder_)
    {
        soft_video_encoder_->CommitEnCodeFrame();
    }

//    if (nvidia_video_encoder_)
//    {
//        nvidia_video_encoder_->CommitEnCodeFrame();
//    }
}

bool RdpServer::CreateSoftVideoChannel(const UINT rtsp_channel, const UINT fps, int protocol)
{
    if (soft_video_encoder_ == NULL)
    {
        soft_video_encoder_ = new SoftEncoder();
        soft_video_encoder_->SetEncodeFinishedEvent(mMsgEvent);
    }

    if (soft_video_channel_ == NULL)
    {
        soft_video_channel_ = new VideoChannel(soft_video_encoder_);
        soft_video_channel_->StartCapture();
    }

    return true;
}

bool RdpServer::DestroySoftVideoChannel()
{
    if (soft_video_channel_)
    {
        delete soft_video_channel_;
        soft_video_channel_ = NULL;
    }

    if (soft_video_encoder_)
    {
        delete soft_video_encoder_;
        soft_video_encoder_ = NULL;
    }

    return true;
}

void RdpServer::RequestIDR(int channel)
{
//    soft_video_encoder_->RequestIDR(channel);
//    nvidia_video_encoder_->RequestIDR(channel);
}

// 创建RTSP服务器
bool RdpServer::CreateRtspServer(const int port, RtspServerVerifyUserCallback VerifyUserCallback)
{
    auto ret = StartRTSPServer(port, 256, 0, VerifyUserCallback);
    if (ret != 0)
    {
        mExitFlag = true;
    }
    return true;
}

// 负责抓视频与音频，并且发送到远方
static DWORD WINAPI RdpThreadFunction(LPVOID lpParam)
{
    RdpServer* rdp = reinterpret_cast<RdpServer*>(lpParam);
    if (rdp == NULL)
    {
        return 0;
    }

    clock_t base = clock();
    while (rdp->ExitFlag() == false)
    {

        WaitForSingleObject(rdp->MsgEvent(), INFINITE);
        if (rdp->ExitFlag())
        {
            return 0;
        }

        FrameType* frame = nullptr;
        auto video_encoder = rdp->soft_video_encoder_;
        if (video_encoder && video_encoder->GetRtspChannel() >= 0)
        {
            frame = video_encoder->PopFrame();
            while (frame)
            {
                int iFrameType = 1;

                if (frame->frameType == 2 || frame->frameType == 3)
                {
                    iFrameType = 0;
                }

                SendVideoStreamToRtspServer(video_encoder->GetRtspChannel(),
                    0, 0, iFrameType, clock() - base,
                    frame->len, (unsigned char*)frame->buffer);

                FrameDestroy(frame);
                frame = nullptr;

                frame = video_encoder->PopFrame();
            }
        }
    }
    return 0;
}

bool RdpServer::Start()
{
    if (RdpThread != NULL)
        return true;
    DWORD dwThreadID;

    RdpThread = CreateThread(NULL,
        0,
        RdpThreadFunction,
        this,
        0,
        &dwThreadID);
    return true;
}

void RdpServer::DestroyAllVideo()
{
    DestroySoftVideoChannel();
}

void RdpServer::DestroyRtsp()
{
    StopRtspServer();
}
