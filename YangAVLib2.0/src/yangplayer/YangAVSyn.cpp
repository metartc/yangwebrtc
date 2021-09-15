#include<cmath>
#include <string>
#include <string.h>
#include <yangplayer/YangAVSyn.h>

YangAVSyn::YangAVSyn() {
	m_maxMinus=20;
	m_baseClock=0;
	m_startClock=0;
	m_paused=0;

}

YangAVSyn::~YangAVSyn() {

}
int32_t YangAVSyn::playAudioFrame(int64_t pts){
	updateBaseTimestamp(pts);
	//int32_t minusClock=YangTime::getMilliTick()-m_startClock;
	int32_t minusClock=yang_get_milli_tick()-m_startClock;
	int32_t minusPts=pts-m_baseClock;
	int32_t minus=abs(minusPts-minusClock);
	if(abs(minus)<=m_maxMinus) return 0;
	return 1;
}
int32_t YangAVSyn::playVideoFrame(int64_t pts){
	updateBaseTimestamp(pts);
	int32_t minusClock=yang_get_milli_tick()-m_startClock;
	int32_t minusPts=pts-m_baseClock;
	int32_t minus=abs(minusPts-minusClock);
	if(abs(minus)<=m_maxMinus) return 0;
	return 1;
}
void YangAVSyn::updateBaseTimestamp(int64_t pts){
	if(m_baseClock==0) {
		setClock();
		m_baseClock=pts;
	}
}
void YangAVSyn::setClock(){
	m_startClock=yang_get_milli_tick();
}

