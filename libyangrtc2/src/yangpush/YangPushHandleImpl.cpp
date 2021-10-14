#include <yangutil/sys/YangLog.h>
#include <yangutil/yang_unistd.h>
#include <yangpush/YangPushHandleImpl.h>
YangPushHandle::YangPushHandle(){

}
YangPushHandle::~YangPushHandle(){

}
YangPushHandle* YangPushHandle::createPushHandle(YangContext* pcontext,YangSysMessageI* pmessage){
    return new YangPushHandleImpl(pcontext,pmessage);
}
YangPushHandleImpl::YangPushHandleImpl(YangContext* pcontext,YangSysMessageI* pmessage) {
    m_cap=NULL;
    m_pub=NULL;
    m_context=pcontext;
    m_message=pmessage;
    m_cap=new YangPushPublish(m_context);

    m_cap->startAudioCapture();
    m_cap->startVideoCapture();
    m_cap->initAudioEncoding();
    m_cap->initVideoEncoding();
}

YangPushHandleImpl::~YangPushHandleImpl() {
    if(m_pub) m_pub->disConnectMediaServer();
    if(m_cap) m_cap->stopAll();
    yang_delete(m_pub);
    yang_delete(m_cap);

}
void YangPushHandleImpl::disconnect(){
    if(m_cap){
        m_cap->stopAudioCaptureState();
        m_cap->stopVideoCaptureState();
    }
    stopPublish();
}
void YangPushHandleImpl::init(){

}
void YangPushHandleImpl::startCapture(){

}
void YangPushHandleImpl::stopPublish(){
    if(m_pub) {
        m_pub->disConnectMediaServer();
    }
    yang_stop(m_pub);
    yang_stop_thread(m_pub);
    yang_delete(m_pub);
}
YangVideoBuffer* YangPushHandleImpl::getPreVideoBuffer(){
    if(m_cap) return m_cap->getPreVideoBuffer();
    return NULL;
}
int YangPushHandleImpl::publish(string url,string localIp,int32_t localPort) {

    int err=Yang_Ok;
    if((err=parse(url))!=Yang_Ok) return err;

    stopPublish();
    yang_trace("\nnetType==%d,server=%s,port=%d,app=%s,stream=%s\n",m_url.netType,m_url.server.c_str(),m_url.port,m_url.app.c_str(),m_url.stream.c_str());


    if(m_pub==NULL){
        m_pub=new YangRtcPublish(m_context);
    }

    //initList();


    m_cap->setNetBuffer(m_pub);
    m_cap->startAudioEncoding();
    m_cap->startVideoEncoding();
    if((err=m_pub->init(m_url.netType,m_url.server,localIp,localPort,1985,m_url.app,m_url.stream))!=Yang_Ok){
        return yang_error_wrap(err," connect server failure!");
    }

    m_pub->start();
    m_cap->startAudioCaptureState();
    m_cap->startVideoCaptureState();
    return err;

}


//rtmp://host[:port]/app/stream
int32_t YangPushHandleImpl::parse(std::string purl) {
    yang_trace("\nurl===============%s\n",purl.c_str());
    char *url = (char*)purl.c_str();
    char *p = strstr(url, "://");
    if (!p) {
        yang_error("RTMP URL: No :// in url!");
        return 1;
    }
    int32_t len = (int) (p - url);
    if (len == 4 && memcmp(url, "rtmp", 4) == 0) {
        m_url.netType = Yang_Rtmp;
        m_url.port=1935;
    } else if (len == 6 && memcmp(url, "webrtc", 6) == 0) {
        m_url.netType = Yang_Webrtc;
        m_url.port=1985;
    } else {
        return 1;
    }
    p += 3;
    if (*p == 0) {
        yang_warn("No hostname in URL!");
        return 1;
    }

    char *end = p + strlen(p);
    char *col = strchr(p, ':');
    //schar *ques = strchr(p, '?');
    char *slash = strchr(p, '/');
    int32_t hostlen;
    if (slash)
        hostlen = slash - p;
    else
        hostlen = end - p;
    if (col && col - p < hostlen)
        hostlen = col - p;

    if (hostlen < 256) {
        m_url.server = std::string(p, hostlen);

    } else {
        yang_warn("Hostname exceeds 255 characters!");
    }

    p += hostlen;

    if (*p == ':') {
        uint32_t  p2;
        p++;
        p2 = atoi(p);
        if (p2 > 65535) {
            yang_warn("Invalid port number!");
        } else {
            m_url.port = p2;
        }
    }

    if (!slash) {
        yang_warn("No application or playpath in URL!");
        return 0;
    }
    p = slash + 1;
    //parse app
    char *slash2, *slash3 = NULL, *slash4 = NULL;
    int32_t applen, appnamelen;

    slash2 = strchr(p, '/');
    if (slash2)
        slash3 = strchr(slash2 + 1, '/');
    if (slash3)
        slash4 = strchr(slash3 + 1, '/');

    applen = end - p; /* ondemand, pass all parameters as app */
    appnamelen = applen; /* ondemand length */
    if (slash4)
        appnamelen = slash4 - p;
    else if (slash3)
        appnamelen = slash3 - p;
    else if (slash2)
        appnamelen = slash2 - p;

    applen = appnamelen;
    m_url.app = std::string(p, applen);
    p += appnamelen;

    //parse streamName
    if (*p == '/')
        p++;

    if (end - p) {
        m_url.stream = std::string(p, end - p);
    }
    //printf("\n************************webrtc://%s:%d/%s/%s\n",m_url.server.c_str(),m_url.port,m_url.app.c_str(),m_url.stream.c_str());
    return 0;
}
