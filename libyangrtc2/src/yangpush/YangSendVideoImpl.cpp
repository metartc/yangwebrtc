#include <yangpush/YangSendVideoImpl.h>

YangSendVideoImpl::YangSendVideoImpl() {
	m_outVideoBuffer=NULL;
	m_outPreVideoBuffer=NULL;
	m_width=1920;
	m_height=1080;
	m_buf=NULL;
	m_len=0;

}

YangSendVideoImpl::~YangSendVideoImpl() {
	yang_deleteA(m_buf);
}
void YangSendVideoImpl::init(int32_t wid,int32_t hei){
	m_width=wid;
	m_height=hei;
}
void YangSendVideoImpl::putVideoRgba(uint8_t *data, int len) {
	if(m_buf==NULL){
		m_len=m_width*m_height*3/2;
		m_buf=new uint8_t[m_len];

	}

	m_yuv.bgratoI420(data,m_buf,m_width,m_height);
	putVideoI420(m_buf,m_len);

}

void YangSendVideoImpl::putVideoI420(uint8_t *data, int len) {
	m_videoFrame.payload=data;
	m_videoFrame.nb=len;
	if(m_outVideoBuffer) m_outVideoBuffer->putVideo(&m_videoFrame);
	if(m_outPreVideoBuffer) m_outPreVideoBuffer->putVideo(&m_videoFrame);


}
