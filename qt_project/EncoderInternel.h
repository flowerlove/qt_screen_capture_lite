#pragma once

#include "frame.h"
#include <QObject>
#include <QMutex>
#include <QVector>
#include <QMap>
#include <QQueue>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#include "libavutil/samplefmt.h"
#include "libavutil/audio_fifo.h"
#ifdef __cplusplus
}
#endif

struct SubTextureInfo
{
    MemTypePtr texture_ptr_;
    bool dirty_;
};

    struct _parmas {
        UINT width, height;
    };

    enum WindowPreviewProtocol
    {
        none = 0,
        webrtc = 1,
        mjpeg = 2
    };

class SoftEncoder
{
public:
    void Encode();
    ~SoftEncoder() { FreeEncodedFrame(); }
    void Init();
    void UnInit();
    // 提交一次待编码帧
    void CommitEnCodeFrame();

    // 释放队列中的待编码帧
    void FreeEncodeFrame();
    // 内部实际编码
    void InternelEncode(AVFrame* frame);
    // PushEncodeFrame
    void PushEncodeFrame(AVFrame* frame);

    AVFrame* PopEncodeFrame();
    void PrepareFrame();
    AVFrame* MallocAvFrame();

    void InitH264Encoder();
    void UnInitH264Encoder();

    void PushFrame(FrameType* frame)
    {
        framemut->lock();
        m_Frames->push_front(frame);
        framemut->unlock();
    }
    FrameType* PopFrame()
    {
        framemut->lock();
        FrameType* frame = NULL;
        if (!m_Frames->empty())
        {
            frame = std::move(m_Frames->front());
            m_Frames->pop_back();
        }
        framemut->unlock();
        return frame;
    }

    QQueue<FrameType*>* m_Frames  = new QQueue<FrameType*>();
    QMutex* framemut = new QMutex();
    bool is_init_flag_ = false;
    int rtsp_channel_ = 1;
    bool request_idr_flag_ = false;

    void SetEncodeFinishedEvent(HANDLE event)
    {
        m_EnCodeFinishedEvent = event;
    }

    HANDLE GetEncodeFinishedEvent()
    {
        return m_EnCodeFinishedEvent;
    }

    void SetEncodeCommitEvent(HANDLE event)
    {
        m_CommitEnCodeEvent = event;
    }

    HANDLE GetEncodeCommitEvent()
    {
        return m_CommitEnCodeEvent;
    }

    bool AddSubRenderTexture(int channel, MemTypePtr ptr)
    {
        return true;
    }

    bool RemoveSubRenderTexture(int channel)
    {
        return true;
    }

    bool ReplaceRenderTexture(const MemTypePtr ptr)
    {
        return true;
    }

    int GetRtspChannel() const
    {
        return rtsp_channel_;
    }

    void SetRtspChannel(int value)
    {
        rtsp_channel_ = value;
    }

    void RequestIDR(int channel)
    {
        if (channel != rtsp_channel_)
        {
            return;
        }

        request_idr_flag_ = true;
    }

private:
    // 每编码成功就发送信号
    HANDLE m_EnCodeFinishedEvent;
    // 接收到编码准备信号
    HANDLE m_CommitEnCodeEvent;
    // 释放队列中的已经编码的数据帧
    void FreeEncodedFrame()
    {
        FrameType* frame = PopFrame();
        while (frame)
        {
            FrameDestroy(frame);
            frame = PopFrame();
        }
    }
    QVector<AVFrame*>* m_EnCodeFrames = new QVector<AVFrame*>();
    QMutex* m_EnCodeFramesMut = new QMutex();

    // 编码器
    const AVCodec *m_Codec = NULL;
    AVCodecContext *m_CodecContext = NULL;
    struct SwsContext *m_pSwsCtx = NULL;
    AVFrame* temp_frame_ = NULL;
};
