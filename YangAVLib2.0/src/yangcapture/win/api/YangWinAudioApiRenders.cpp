#include <yangcapture/win/api/YangWinAudioApiRenders.h>
#ifdef _WIN32
#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
YangWinAudioApiRenders::YangWinAudioApiRenders(YangAudioInfo *pini):YangWinAudioApiRender(pini)
{



}
YangWinAudioApiRenders::~YangWinAudioApiRenders(){
	yang_stop(this);
   yang_stop_thread(this);

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

#endif
