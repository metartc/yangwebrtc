#ifndef YANGWINACEI_H
#define YANGWINACEI_H
#include "YangWinAec_global.h"
#include <stdint.h>
struct YangAecType{
    int audioIndex;
    unsigned long bufLen;
    uint8_t* data;
    void* context;
};
     extern "C" YANGWINAEC_EXPORT void yang_get_winaec(YangAecType* context);
     extern "C"  YANGWINAEC_EXPORT int yang_init_aec(YangAecType* context);
     extern "C"  YANGWINAEC_EXPORT int yang_get_aec_buffer(YangAecType* context);
     extern "C" YANGWINAEC_EXPORT void  yang_delete_winaec(YangAecType* context);


#endif // YANGWINACEI_H
