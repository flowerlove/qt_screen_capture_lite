//RDPServer类
#include <map>
#include "frame.h"
#include "librtspserver.h"
#include "VideoChannel.h"
#include <QObject>
#include "EncoderInternel.h"

using namespace std;

struct WindowPreviewInfo
{
    int show_width;
    int show_height;
    MemTypePtr texture_ptr;

    int encode_width;
    int encode_height;
    int rtsp_channel;

    SoftEncoder* video_encoder;
};

 class RdpServer/* : public QObject*/
{
//     Q_OBJECT
public:
    RdpServer();
    ~RdpServer();

//    bool CreateNvidiaVideoChannel(const unsigned int width,const unsigned int height,
//        const unsigned int rtsp_channel, const unsigned int fps, int protocol);

//	bool DestroyNividiaVideoChannel();

    bool CreateSoftVideoChannel(const unsigned int rtsp_channel, const unsigned int fps, int protocol);

    bool DestroySoftVideoChannel();

    void RequestIDR(int channel);

    // 创建RTSP
    bool CreateRtspServer(const int port, RtspServerVerifyUserCallback VerifyUserCallback);
    // 启动
    bool Start();

    bool ExitFlag(){ return mExitFlag; }
    HANDLE MsgEvent(){ return mMsgEvent; }
    void SetId(int id){ mId = id; }
    int GetId(){ return mId; }

    static EnCodeType GetEnCodeId(EnCodeType type);

    void AllEncoderInit();
    void AllEncoderCommit();

    SoftEncoder* soft_video_encoder_ = NULL;
    VideoChannel* soft_video_channel_ = NULL;

//    EnCoder* nvidia_video_encoder_ = NULL;
//    VideoChannel* nvidia_video_channel_ = NULL;

private:
    void DestroyAllVideo();
    void DestroyRtsp();

    HANDLE mMsgEvent;
    HANDLE RdpThread = NULL;
    bool mExitFlag = false;
    int mId = -1;
};
