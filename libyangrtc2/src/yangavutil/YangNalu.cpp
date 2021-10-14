#include <yangavutil/video/YangNalu.h>
#include <yangutil/yangtype.h>
#include <memory.h>

#include <yangutil/sys/YangEndian.h>


int32_t yang_getKeyframeNalu(YangFrame* videoFrame){
	uint8_t* tmp=NULL;//videoFrame->payload;
	int len=videoFrame->nb;
	//int ret=0;
	uint32_t naluLen=0;
	int pos=0;
	int32_t err=1;
	while(pos<len){
		tmp=videoFrame->payload+pos;
		if((*(tmp+4) & kNalTypeMask)==YangAvcNaluTypeIDR){
			videoFrame->payload=tmp;
			videoFrame->nb=len-pos;
			err=Yang_Ok;
			break;
		}
		naluLen=yang_get_be32(tmp);
		if(naluLen>len){
			break;
		}
		pos+=naluLen+4;
	}
	return err;
}


bool yang_hasPframe(uint8_t* p){
    if((*(p+4) & kNalTypeMask)==YangAvcNaluTypeNonIDR) return true;
    return false;
}

int32_t yang_parseNalue(YangFrame* videoFrame,YangNaluData* pnalu){
	uint8_t* tmp=NULL;//videoFrame->payload;
	uint32_t len=videoFrame->nb;
	uint32_t naluLen=0;
	int32_t pos=0;
	int32_t err=1;
	pnalu->spsppsPos=-1;
	pnalu->keyframePos=-1;
	while(pos<len){
		tmp=videoFrame->payload+pos;
		if((*(tmp+4) & kNalTypeMask)==YangAvcNaluTypeIDR){
			pnalu->keyframePos=pos;
			break;
		}
		if((*(tmp+4) & kNalTypeMask)==YangAvcNaluTypeSPS){
			pnalu->spsppsPos=pos;
		}
		naluLen=yang_get_be32(tmp);
		if(naluLen>len){
			break;
		}
		pos+=naluLen+4;
	}
	return err;
}

int32_t yang_getSpsppseNalu(YangFrame* videoFrame,uint8_t* pnaludata,uint8_t* meta){
				int32_t spsLen = yang_get_be32(pnaludata);
                int32_t ppsLen = yang_get_be32(pnaludata+4+spsLen);
				uint8_t *sps = pnaludata + 4;
				uint8_t *pps = pnaludata + 4 + spsLen + 4;


				uint8_t *p = meta;
				*p++ = 0x17;
				*p++ = 0x0; // avc_type: sequence header
				*p++ = 0x0; // composition time
				*p++ = 0x0;
				*p++ = 0x0;
				*p++ = 0x01; // version
				*p++ = sps[1];
				*p++ = sps[2];
				*p++ = sps[3];
				*p++ = 0xff;
				*p++ = 0xe1;
				*p++ = 0x00;
				*p++ = spsLen;
				memcpy(p, sps, spsLen);
				p += spsLen;
				*p++ = 0x01;
				*p++ = 0x00;
				*p++ = ppsLen;
				memcpy(p, pps, ppsLen);
				p += ppsLen;

				videoFrame->payload=meta;
				videoFrame->nb=p-meta;
	return Yang_Ok;
}

