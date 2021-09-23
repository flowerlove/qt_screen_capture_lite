#pragma once

#include <malloc.h>
#include <Windows.h>

typedef void* MemTypePtr;

typedef enum {
    ENCODE_FREE = -1,
    ENCODE_HARD_D3D11 = 0,
    ENCODE_SOFT = 10
}EnCodeType;

typedef struct _frame_t
{
    char *buffer;
    UINT32 len;
    UINT32 frameType;
}FrameType;

typedef struct _encode_frame_t
{
    MemTypePtr ptr;
    UINT32 Width;
    UINT32 Height;
}EnCodeFrameType;

void FrameDestroy(FrameType* frame);
FrameType* MallocFrame(int len);

