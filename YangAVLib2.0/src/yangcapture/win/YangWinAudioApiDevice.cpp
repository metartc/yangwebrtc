
#include "YangWinAudioApiDevice.h"
#ifdef _WIN32
#include <unistd.h>
#include <yangutil/yangtype.h>
#include <yangavutil/audio/YangAudioUtil.h>
YangWinAudioApiDevice::YangWinAudioApiDevice(YangAudioInfo *pini) {
    m_isStart=0;
    m_isInit=0;
    m_ret=0;
    m_size=640;
    m_loops=0;
    m_capture = NULL;
    m_preProcess=NULL;
    m_ahandle = new YangAudioCaptureHandle(pini);
#if UsingApi
    m_player = new YangWinAudioApiRender(pini);
    if (m_player)     m_player->init();
#else
    m_player=new YangAudioPlaySdl(pini);
    if(m_player) m_player->init();
#endif

#if UsingBuiltInAec
    m_lib.loadObject("YangWinAec");
    yang_get_winaec =(void (*)(YangAecType* context)) m_lib.loadFunction("yang_get_winaec");
    yang_init_aec =(int (*)(YangAecType* context)) m_lib.loadFunction("yang_init_aec");
    yang_get_aec_buffer  = (int (*)(YangAecType* context)) m_lib.loadFunction("yang_get_aec_buffer");
    yang_delete_winaec  = (void (*)(YangAecType* context)) m_lib.loadFunction("yang_delete_winaec");
    m_capture=new YangAecType();
    m_capture->audioIndex=pini->aIndex;
    m_capture->bufLen=0;
    m_capture->context=NULL;
    m_capture->data=NULL;
    m_in_audioBuffer = NULL;
    if(yang_get_winaec) yang_get_winaec(m_capture);
    if(yang_init_aec) yang_init_aec(m_capture);
#else
    m_capture=new YangWinAudioApiCapture();
    m_capture->initCapture();
#endif


}

YangWinAudioApiDevice::~YangWinAudioApiDevice() {
    yang_delete(m_ahandle);
    yang_delete(m_player);
#if UsingBuiltInAec
    if (m_capture && yang_delete_winaec)		yang_delete_winaec(m_capture);
    m_lib.unloadObject();
    yang_delete_winaec = NULL;
    yang_get_aec_buffer = NULL;
    yang_init_aec=NULL;
    yang_get_winaec=NULL;
#else
    yang_delete(m_capture);
#endif
}

void YangWinAudioApiDevice::setCatureStart() {
    m_ahandle->isBuf = 1;
}
void YangWinAudioApiDevice::setCatureStop() {
    m_ahandle->isBuf = 0;
}
void YangWinAudioApiDevice::setOutAudioBuffer(YangAudioBuffer *pbuffer) {
    m_ahandle->setOutAudioBuffer(pbuffer);
}
void YangWinAudioApiDevice::setPlayAudoBuffer(YangAudioBuffer *pbuffer) {
    m_ahandle->m_aecPlayBuffer = pbuffer;
}
void YangWinAudioApiDevice::setAec(YangAecBase *paec) {
    m_ahandle->m_aec = paec;
}
void YangWinAudioApiDevice::setPreProcess(YangPreProcess *pp) {
    m_preProcess=pp;
}

void YangWinAudioApiDevice::setInAudioBuffer(vector<YangAudioPlayBuffer*> *pal) {
    m_in_audioBuffer = pal;
}

int32_t YangWinAudioApiDevice::getRenderData(uint8_t *pcm_write) {
    uint8_t *tmp = NULL;
    for (size_t i = 0; i < m_in_audioBuffer->size(); i++) {
        if (m_in_audioBuffer->at(i) && m_in_audioBuffer->at(i)->m_size > 0) {
            tmp = m_in_audioBuffer->at(i)->getAudios(&m_audioFrame);
            if (tmp) {
                if (m_preProcess)	m_preProcess->preprocess_run((short*) tmp);
                if (i == 0) {
                    memcpy(pcm_write, tmp, m_size);
                } else {
                    m_mix.yangMix1(pcm_write, tmp, m_size, 128);
                }
            }
            tmp = NULL;
            // m_in_audioBuffer->at(i)->resetIndex();
        }
    }
    return Yang_Ok;
}
int32_t YangWinAudioApiDevice::hasData() {
    if (!m_in_audioBuffer)
        return 0;
    for (int32_t i = 0; i < (int) m_in_audioBuffer->size(); i++) {
        //al=audioList->at(i);
        //if(audioList->at(i)==NULL) printf("\n%d__%d*****************has null value\n",audioList->size(),i);
        if (m_in_audioBuffer->at(i)->m_size > 0)
            return 1;
    }
    //al=NULL;
    return 0;
}

void YangWinAudioApiDevice::stopLoop() {
    m_loops = 0;

}

void YangWinAudioApiDevice::run() {
    m_isStart = 1;
    startLoop();
    m_isStart = 0;
}

int YangWinAudioApiDevice::init() {
    if (m_isInit)
        return Yang_Ok;

    m_isInit = 1;
    return Yang_Ok;
}

void YangWinAudioApiDevice::startLoop() {

    m_loops = 1;
    int captureLen = 0;
    uint8_t *audioBuf = new uint8_t[640];
    uint8_t *bufs = new uint8_t[2048];
    uint8_t *tmp = new uint8_t[2048];
    uint8_t *play_buf = new uint8_t[640];
    int bufLen = 0;
    YangAutoFreeA(uint8_t, audioBuf);
    YangAutoFreeA(uint8_t, bufs);
    YangAutoFreeA(uint8_t, tmp);
    YangAutoFreeA(uint8_t, play_buf);
    //int first=1;
    //int captureData=0;
    //int left=0;
#if UsingBuiltInAec
    if(m_capture) m_capture->data=audioBuf;
#else
    int readStart;
    YangFrame audioFrame;
    YangResample m_res;
    m_res.init(2, 48000, 16000);
    uint8_t* capBuf=new uint8_t[8192];

    YangAutoFreeA(uint8_t, capBuf);
    audioFrame.payload=capBuf;
    audioFrame.nb=0;
    int a48kLen=960*4;
    uint32_t outlen = 320*4;
    uint32_t inlen = 960 * 4;
    uint8_t* capTmp=new uint8_t[320*4];
    YangAutoFreeA(uint8_t, capTmp);
    if(m_capture) m_capture->startCpature();
#endif

#if UsingApi
    if(m_player&&m_player->startRender()) {

    }
#endif
    while (m_loops) {
        usleep(10000);
        captureLen=0;
#if UsingBuiltInAec
        m_capture->bufLen=0;
        if(yang_get_aec_buffer) {
            int ret=yang_get_aec_buffer(m_capture);
            len=m_capture->bufLen;
            printf("%d-%d,",len,ret);
        }
#else

        m_capture->captureFrame(&audioFrame);
        if(audioFrame.nb>=a48kLen){
            m_res.resample((const short*) capBuf, inlen, (short*) capTmp, outlen);
            StereoToMono((short*) capTmp, (short*) audioBuf, m_size / 2);
            captureLen=640;
            audioFrame.nb-=a48kLen;
        }
#endif

        if(captureLen==0) continue;
        if (captureLen) {
            if(bufLen) memcpy(bufs, tmp, bufLen);
            memcpy(bufs+bufLen, audioBuf, captureLen);
            bufLen += captureLen;
            if (bufLen >= 640) {
#if UsingBuiltInAec
                m_ahandle->putBuffer1(bufs, 640);
#else
                if (m_preProcess)      m_preProcess->preprocess_run((short*) bufs);       
#endif
                bufLen -= 640;
                if(bufLen) memcpy(tmp, bufs + 640, bufLen);

                memset(play_buf, 0, 640);
                //get play buffer data
                if (hasData())		getRenderData(play_buf);
#if UsingApi
                m_player->render_16k(play_buf,640);
                //aec buffer render data
                if (readStart)	 m_ahandle->putEchoPlay((short*) play_buf,640);

                if (!readStart)		readStart = 1;
                if (readStart)
                    m_ahandle->putEchoBuffer(bufs,640);  //aec filter render data
                else
                    m_ahandle->putBuffer1(bufs,640);
#else
                m_player->playSDL(play_buf);
#endif


            }


        }
    }
#if UsingApi
    if(m_player) m_player->stopRender();
#endif

#if UsingBuiltInAec

#else
    if(m_capture) m_capture->stopCapture();
#endif
    //YANG_DELETES(pcm_read);
}
#endif
