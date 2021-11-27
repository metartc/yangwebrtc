#include "yangencoder/YangGpuEncoderFactory.h"
#include "YangEncoderGpu.h"
YangGpuEncoderFactory::YangGpuEncoderFactory()
{

}
YangGpuEncoderFactory::~YangGpuEncoderFactory()
{

}

YangVideoEncoder* YangGpuEncoderFactory::createGpuEncoder(){
    return new YangEncoderGpu();
}
