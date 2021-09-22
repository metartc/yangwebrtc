#include <yangcapture/win/api/YangWinAudioApiRenders.h>
#ifdef _WIN32
#include <yangutil/sys/YangLog.h>
#include <unistd.h>
YangWinAudioApiRenders::YangWinAudioApiRenders(YangAudioInfo *pini):YangWinAudioApiRender(pini)
{
   // m_in_audioBuffers = NULL;


}
YangWinAudioApiRenders::~YangWinAudioApiRenders(){
	yang_stop(this);
   yang_stop_thread(this);
  //  m_in_audioBuffers = NULL;
}



void YangWinAudioApiRenders::startLoop() {

    m_loops = 1;
    if(startRender()){
        yang_error("start render fail");
        return;
    }

    HANDLE waitArray[1] = {m_samplesReadyEvent};
    while (m_loops == 1) {
		DWORD waitResult = WaitForMultipleObjects(1, waitArray, FALSE, 500);
		if(waitResult==(WAIT_OBJECT_0 + 0)){
			render_aec_10ms();
		}

    }
    yang_usleep(300*1000);
    stopRender();
}
/**
void YangWinAudioApiRenders::startLoop() {

    m_loops = 1;
    if(startRender()){
        yang_error("start render fail");
        return;
    }
    YangFrame frame;
    int32_t audiolen = 640;
   // short* pcm_short=new short[audiolen/2];
    uint8_t *pcm_write = new uint8_t[audiolen];//new uint8_t[audiolen];
    //YangAutoFreeA(short,pcm_short);
    YangAutoFreeA(uint8_t,pcm_write);
    HANDLE waitArray[1] = {m_samplesReadyEvent};
    while (m_loops == 1) {
		DWORD waitResult = WaitForMultipleObjects(1, waitArray, FALSE, 500);
		if(waitResult==(WAIT_OBJECT_0 + 0)){
			memset(pcm_write,0,640);
			if(hasData()){
				getRenderData(pcm_write);
				   if(m_aec) {
                             m_aec->echo_playback((short*)pcm_write);
                            if(!m_hasRenderEcho) m_hasRenderEcho=1;
                    }
			}
            render_16k(pcm_write,640);
		}

    }
    yang_usleep(300*1000);
    stopRender();

}**/
#endif
