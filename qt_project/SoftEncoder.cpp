//#include "SoftEncoder.h"
//#include <iostream>

//SoftEncoder::SoftEncoder(UINT fps, int channel, int protocol)
//{
//    m_Codec = NULL;
//    m_CodecContext = NULL;
//    m_pSwsCtx = NULL;
//    fps_ = fps;
//    rtsp_channel_ = channel;
//    protocol_ = (WindowPreviewProtocol)protocol;
//}

//SoftEncoder::SoftEncoder(MemTypePtr ptr, UINT width, UINT height, UINT fps, int protocol)
//{
//    m_Codec = NULL;
//    m_CodecContext = NULL;
//    m_pSwsCtx = NULL;
//    m_Width = width;
//    m_Height = height;
//    fps_ = fps;
//    protocol_ = (WindowPreviewProtocol)protocol;
//}

//SoftEncoder::~SoftEncoder()
//{
//    FreeEncodeFrame();
//    UnInit();
//}

//void SoftEncoder::FreeEncodeFrame()
//{
//    AVFrame* frame = PopEncodeFrame();
//    while (frame)
//    {
//        av_frame_free(&frame);
//        frame = PopEncodeFrame();
//    }
//}

//void SoftEncoder::PushEncodeFrame(AVFrame* frame)
//{
//    m_EnCodeFramesMut.lock();
//    m_EnCodeFrames.push_back(frame);
//    m_EnCodeFramesMut.unlock();
//}

//AVFrame* SoftEncoder::PopEncodeFrame()
//{
//    AVFrame* frame = NULL;
//    m_EnCodeFramesMut.lock();
//    if (m_EnCodeFrames.size() > 0)
//    {
//        frame = m_EnCodeFrames.at(0);
//        m_EnCodeFrames.erase(m_EnCodeFrames.begin());
//    }
//    m_EnCodeFramesMut.unlock();
//    return frame;
//}

//// 初始化
//void SoftEncoder::Init()
//{
//    if (is_init_flag_ || protocol_ == WindowPreviewProtocol::none)
//    {
//        return;
//    }

//    if (protocol_ == WindowPreviewProtocol::webrtc)
//    {
//        InitH264Encoder();
//    }
//    else
//    {
//        return;
//    }

//    is_init_flag_ = true;
//}
//// 反初始化
//void SoftEncoder::UnInit()
//{
//    if (protocol_ == WindowPreviewProtocol::none)
//    {
//        return;
//    }

//    if (protocol_ == WindowPreviewProtocol::webrtc)
//    {
//        UnInitH264Encoder();
//    }
//}

//AVFrame* SoftEncoder::MallocAvFrame()
//{
//    AVFrame* frame = av_frame_alloc();
//    frame->format = m_CodecContext->pix_fmt;
//    frame->width = m_CodecContext->width;
//    frame->height = m_CodecContext->height;
//    av_frame_get_buffer(frame, 32);
//    return frame;
//}

//void SoftEncoder::PrepareFrame()
//{
//    // 分配一帧数据
//    AVFrame* frame = NULL;
//    m_EnCodeFramesMut.lock();
//    if (m_EnCodeFrames.size() > 4)
//    {
//        m_EnCodeFramesMut.unlock();
//        return;
//    }
//    m_EnCodeFramesMut.unlock();
//}

//// 准备编码
//void SoftEncoder::CommitEnCodeFrame()
//{
//    PrepareFrame();
//    SetEvent(GetEncodeCommitEvent());
//}

//// 编码
//void SoftEncoder::Encode()
//{
//    AVFrame* frame = PopEncodeFrame();
//    if (frame)
//    {
//        // 开始编码 to do
//        InternelEncode(frame);
//        // 释放数据帧
//        av_frame_free(&frame);
//    }
//}

//void SoftEncoder::InternelEncode(AVFrame* frame)
//{
//    UINT num = 0;
//    FrameType* oframe = NULL;
////    AVPacket pkt = {};
////    av_init_packet(&pkt);
////    pkt.data = NULL;
////    pkt.size = 0;
//    int got_output = 0;
//    frame->pts = num++;
//    int ret = 0;//avcodec_encode_video2(m_CodecContext, &pkt, frame, &got_output);
//    if (ret < 0) {
//        av_log(NULL, AV_LOG_ERROR, "Error encoding frame\n");
////        av_packet_unref(&pkt);
//        return;
//    }
//    if (got_output) {
////        oframe = MallocFrame(pkt.size);
////        memcpy(oframe->buffer, pkt.data, pkt.size);
////        oframe->frameType = 0;
////        if (pkt.flags == AV_PKT_FLAG_KEY)
////            oframe->frameType = 2;

//        if (m_CodecContext->pix_fmt == AV_PIX_FMT_YUVJ420P)
//        {
//            int channel = GetRtspChannel();
//        }
//        else
//        {
//            PushFrame(oframe);
//            SetEvent(GetEncodeFinishedEvent());
//        }
//    }
////    av_packet_unref(&pkt);
//}


