#include "frame.h"
#include "EncoderInternel.h"
using namespace std;

class VideoChannel
{
public:
    VideoChannel(SoftEncoder* encoder);
    ~VideoChannel();
    // 启动捕获视频数据
    void StartCapture();
    // 停止捕获视频数据
    void StopCapture();
    // 获取编码后的视频数据
    FrameType* GrabFrame();
    // 获取当前编码器
    SoftEncoder* GetEnCoder();
    void SetEnCoder(SoftEncoder* encoder);
    // 获取退出标志
    bool GetExitFlag();
    // 获取信号句柄
    HANDLE GetEventHandle();
private:
    SoftEncoder* m_EnCoder;
    bool m_ExitFlag;
    HANDLE m_EnCodehThread;
    HANDLE m_Event;
};

