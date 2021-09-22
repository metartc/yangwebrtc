#ifndef YANGWINACEI_H
#define YANGWINACEI_H


#include <stdint.h>
struct YangAecType{
    int audioIndex;
    unsigned long bufLen;
    uint8_t* data;
    void* context;
};



#endif // YANGWINACEI_H
