/*
 * YangWinAudioApiAec.cpp
 *
 *  Created on: 2021年9月20日
 *      Author: yang
 */
#include <yangcapture/win/api/YangWinAudioApiAec.h>
#include <yangutil/yangtype.h>
#ifdef _WIN32
YangWinAudioApiAec::YangWinAudioApiAec(YangAudioInfo *pini) {
		m_isStart=0;
		m_loops=0;
		m_cb=NULL;
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

	    if(yang_get_winaec) yang_get_winaec(m_capture);
	    if(yang_init_aec) yang_init_aec(m_capture);

}

YangWinAudioApiAec::~YangWinAudioApiAec() {
	m_cb=NULL;
    if (m_capture && yang_delete_winaec)		yang_delete_winaec(m_capture);
    m_lib.unloadObject();
    yang_delete_winaec = NULL;
    yang_get_aec_buffer = NULL;
    yang_init_aec=NULL;
    yang_get_winaec=NULL;
}
void YangWinAudioApiAec::setCaptureCallback(YangCaptureCallback* cb){
	m_cb=cb;
}
int YangWinAudioApiAec::initCapture(){
	return Yang_Ok;
}
  int YangWinAudioApiAec::startCpature(){
	  return Yang_Ok;
  }
  int YangWinAudioApiAec::stopCapture(){
	  return Yang_Ok;
  }
  void YangWinAudioApiAec::stop(){
  	stopLoop();
  }

  void YangWinAudioApiAec::stopLoop() {
      m_loops = 0;


  }

  void YangWinAudioApiAec::run() {
      m_isStart = 1;
      startLoop();
      m_isStart = 0;
  }


  void YangWinAudioApiAec::startLoop(){
      m_loops = 1;
      uint8_t* tmp=new uint8_t[1024*4];
      YangAutoFreeA(uint8_t,tmp);
      m_capture->data=tmp;
      YangFrame audioFrame;
      while(m_loops){
    	  yang_usleep(1000*10);
    	  yang_get_aec_buffer(m_capture);
    	  audioFrame.payload=m_capture->data;
    	  audioFrame.nb=m_capture->bufLen;
    	  if(m_cb) m_cb->caputureAudioData(&audioFrame);
      }
  }
#endif
