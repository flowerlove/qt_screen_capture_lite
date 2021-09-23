//#include "EncoderInternel.h"
//#include <vector>
//#include <QObject>
//#ifdef __cplusplus
//extern "C" {
//#endif
//#include "libavformat/avformat.h"
//#include "libavdevice/avdevice.h"
//#include "libavcodec/avcodec.h"
//#include "libswscale/swscale.h"
//#include "libavutil/avutil.h"
//#include "libavutil/frame.h"
//#include "libavutil/samplefmt.h"
//#include "libavutil/audio_fifo.h"
//#ifdef __cplusplus
//}
//#endif

//class SoftEncoder : public EnCoder
//{

//public:
//    SoftEncoder(UINT fps, int channel, int protocol);
//    SoftEncoder(MemTypePtr ptr, UINT width, UINT height, UINT fps, int protocol);
//    ~SoftEncoder();
//    // 编码
//    void Encode();
////    // 初始化
//    void Init();
////    // 反初始化
//    void UnInit();
//    // 提交一次待编码帧
//    void CommitEnCodeFrame();

//    // 释放队列中的待编码帧
//    void FreeEncodeFrame();
//    // 内部实际编码
//    void InternelEncode(AVFrame* frame);
//    // PushEncodeFrame
//    void PushEncodeFrame(AVFrame* frame);

//    AVFrame* PopEncodeFrame();
//    void PrepareFrame();
//    AVFrame* MallocAvFrame();

//    void InitH264Encoder();
//    void UnInitH264Encoder();

//private:
//    WindowPreviewProtocol protocol_;
//    std::vector<AVFrame*> m_EnCodeFrames;
//    std::mutex m_EnCodeFramesMut;
//    UINT m_Width = 1920;
//    UINT m_Height = 1080;
	
//    // 编码器
//    AVCodec *m_Codec = NULL;
//    AVCodecContext *m_CodecContext = NULL;
//    struct SwsContext *m_pSwsCtx = NULL;
//    int BUF_SIZE = 1920 * 1080 * 3;

//    int fps_ = 20.0f;
//};
