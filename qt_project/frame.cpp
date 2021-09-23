#include "frame.h"

void FrameDestroy(FrameType* frame)
{
    if (frame)
    {
        if (frame->buffer)
            free(frame->buffer);
        free(frame);
    }
}

FrameType* MallocFrame(int len)
{
    FrameType* frame = (FrameType*)malloc(sizeof(FrameType));
    frame->buffer = (char*)malloc(len);
    frame->len = len;
    return frame;
}
