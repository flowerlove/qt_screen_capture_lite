#include "EncoderInternel.h"

void SoftEncoder::InitH264Encoder()
{
    // 执行ffmpeg编码初始化
    m_Codec = avcodec_find_encoder_by_name("libx264");
    m_CodecContext = avcodec_alloc_context3(m_Codec);
    av_opt_set(m_CodecContext->priv_data, "qp", "31", 0);
    /* resolution must be a multiple of two */
    m_CodecContext->width = 1920;
    m_CodecContext->height = 1080;
    /* frames per second */
    m_CodecContext->time_base.num = 1;
    m_CodecContext->time_base.den = 30;
    m_CodecContext->framerate.num = 30;
    m_CodecContext->framerate.den = 1;
    m_CodecContext->gop_size = 30;
    m_CodecContext->max_b_frames = 0;
    m_CodecContext->delay = 0;

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    m_CodecContext->thread_count = info.dwNumberOfProcessors;
    m_CodecContext->thread_type = FF_THREAD_SLICE;
    m_CodecContext->pix_fmt = AV_PIX_FMT_NV12;

    av_opt_set(m_CodecContext->priv_data, "preset", "superfast", 0);
    av_opt_set(m_CodecContext->priv_data, "tune", "zerolatency", 0);
    av_opt_set(m_CodecContext->priv_data, "profile", "baseline", 0);

    m_pSwsCtx = sws_getContext(1920, 1080, AV_PIX_FMT_BGRA,
        1920, 1080, m_CodecContext->pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    temp_frame_ = MallocAvFrame();
    int ret = avcodec_open2(m_CodecContext, m_Codec, NULL);
    if (ret != 0)
    {
        UnInitH264Encoder();
    }
}

void SoftEncoder::UnInitH264Encoder()
{
    // 释放ffmpeg资源
    if (m_pSwsCtx)
        sws_freeContext(m_pSwsCtx);
    m_pSwsCtx = NULL;
    if (m_CodecContext)
        avcodec_free_context(&m_CodecContext);
    m_CodecContext = NULL;
}

void SoftEncoder::FreeEncodeFrame()
{
    AVFrame* frame = PopEncodeFrame();
    while (frame)
    {
        av_frame_free(&frame);
        frame = PopEncodeFrame();
    }
}

void SoftEncoder::PushEncodeFrame(AVFrame* frame)
{
    m_EnCodeFramesMut->lock();
    m_EnCodeFrames->push_back(frame);
    m_EnCodeFramesMut->unlock();
}

AVFrame* SoftEncoder::PopEncodeFrame()
{
    AVFrame* frame = NULL;
    m_EnCodeFramesMut->lock();
    if (m_EnCodeFrames->size() > 0)
    {
        frame = m_EnCodeFrames->at(0);
        m_EnCodeFrames->erase(m_EnCodeFrames->begin());
    }
    m_EnCodeFramesMut->unlock();
    return frame;
}

// 初始化
void SoftEncoder::Init()
{
    if (is_init_flag_)
    {
        return;
    }

    InitH264Encoder();

    is_init_flag_ = true;
}
// 反初始化
void SoftEncoder::UnInit()
{
    UnInitH264Encoder();
}

AVFrame* SoftEncoder::MallocAvFrame()
{
    AVFrame* frame = av_frame_alloc();
    frame->format = m_CodecContext->pix_fmt;
    frame->width = m_CodecContext->width;
    frame->height = m_CodecContext->height;
    av_frame_get_buffer(frame, 4);
    return frame;
}

void SoftEncoder::PrepareFrame()
{
    // 分配一帧数据
    AVFrame* frame = NULL;
    m_EnCodeFramesMut->lock();
    if (m_EnCodeFrames->size() > 4)
    {
        m_EnCodeFramesMut->unlock();
        return;
    }
    m_EnCodeFramesMut->unlock();
}

// 准备编码
void SoftEncoder::CommitEnCodeFrame()
{
    PrepareFrame();
    SetEvent(GetEncodeCommitEvent());
}

// 编码
void SoftEncoder::Encode()
{
    AVFrame* frame = PopEncodeFrame();
    if (frame)
    {
        // 开始编码 to do
        InternelEncode(frame);
        // 释放数据帧
        av_frame_free(&frame);
    }
}

void SoftEncoder::InternelEncode(AVFrame* frame)
{
    UINT num = 0;
    FrameType* oframe = NULL;
    AVPacket pkt = {};
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int got_output = 0;
    frame->pts = num++;

    sws_scale(m_pSwsCtx, frame->data, frame->linesize, 0, frame->height, temp_frame_->data, temp_frame_->linesize);

    int ret = avcodec_send_frame(m_CodecContext, temp_frame_);
    if(ret == 0)
    {
        ret = avcodec_receive_packet(m_CodecContext, &pkt);

        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error encoding frame\n");
            av_packet_unref(&pkt);
            return;
        }

        if(ret == 0)
        {
            oframe = MallocFrame(pkt.size);
            memcpy(oframe->buffer, pkt.data, pkt.size);
            oframe->frameType = 0;
            if (pkt.flags == AV_PKT_FLAG_KEY)
                oframe->frameType = 2;

            if (m_CodecContext->pix_fmt == AV_PIX_FMT_YUVJ420P)
            {
                int channel = GetRtspChannel();
            }
            else
            {
                PushFrame(oframe);
                SetEvent(GetEncodeFinishedEvent());
            }

            av_packet_unref(&pkt);
        }
    }
}
