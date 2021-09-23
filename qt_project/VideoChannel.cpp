#include "VideoChannel.h"

// 视频编码线程
static DWORD WINAPI EncodeVideoThreadFunction(LPVOID lpParam)
{
    VideoChannel* channel = (VideoChannel*)lpParam;
    SoftEncoder* encoder = channel->GetEnCoder();
    // 无限循环直到退出标志为真
    while (!channel->GetExitFlag())
    {
        // 等待信号发生
        WaitForSingleObject(channel->GetEventHandle(), INFINITE);
        if (channel->GetExitFlag())
            return 0;
        // 调用编码器进行编码
        if (encoder)
        {
            encoder->Encode();
        }
    }
    return 0;
}

VideoChannel::VideoChannel(SoftEncoder* encoder)
{
    DWORD dwThreadID;
    m_ExitFlag = false;
    m_EnCoder = encoder;
    // 准备信号
    m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_EnCoder->SetEncodeCommitEvent(m_Event);
    // 创建编码线程
    m_EnCodehThread = CreateThread(NULL,
        0,
        EncodeVideoThreadFunction,
        this,
        CREATE_SUSPENDED,
        &dwThreadID);
}

// 注意，这个析构将在渲染线程执行
VideoChannel::~VideoChannel()
{
    m_ExitFlag = true;
    // 设置信号
    SetEvent(m_Event);
    //等待编码线程结束
    WaitForSingleObject(m_EnCodehThread, INFINITE);
    // 释放线程资源
    CloseHandle(m_EnCodehThread);
    // 关闭信号句柄
    CloseHandle(m_Event);
}

HANDLE VideoChannel::GetEventHandle()
{
    return m_Event;
}

void VideoChannel::StartCapture()
{
    ResumeThread(m_EnCodehThread);
}

void VideoChannel::StopCapture()
{
}

FrameType* VideoChannel::GrabFrame()
{
    FrameType* frame = NULL;
    if (m_EnCoder)
    {
        frame = m_EnCoder->PopFrame();
    }
    return frame;
}

SoftEncoder* VideoChannel::GetEnCoder()
{
    return m_EnCoder;
}

void VideoChannel::SetEnCoder(SoftEncoder * encoder)
{
    m_EnCoder = encoder;
    m_EnCoder->SetEncodeCommitEvent(m_Event);
}

bool VideoChannel::GetExitFlag()
{
    return m_ExitFlag;
}
