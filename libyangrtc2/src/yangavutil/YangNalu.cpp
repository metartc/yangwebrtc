#include <yangavutil/video/YangNalu.h>
#include <yangutil/yangtype.h>
#include <memory.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangEndian.h>

int32_t yang_getH264KeyframeNalu(YangFrame *videoFrame) {
	uint8_t *tmp = NULL; //videoFrame->payload;
	int len = videoFrame->nb;
	uint32_t naluLen = 0;
	int pos = 0;
	int32_t err = 1;
	while (pos < len) {
		tmp = videoFrame->payload + pos;
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeIDR) {
			videoFrame->payload = tmp;
			videoFrame->nb = len - pos;
			err = Yang_Ok;
			break;
		}
		naluLen = yang_get_be32(tmp);
		if (naluLen > len) {
			break;
		}
		pos += naluLen + 4;
	}
	return err;
}

bool yang_hasH264Pframe(uint8_t *p) {
	if ((*(p + 4) & kNalTypeMask) == YangAvcNaluTypeNonIDR)
		return true;
	return false;
}

int32_t yang_parseH264Nalu(YangFrame *videoFrame, YangH264NaluData *pnalu) {
	uint8_t *tmp = NULL; //videoFrame->payload;
	uint32_t len = videoFrame->nb;
	uint32_t naluLen = 0;
	int32_t pos = 0;
	int32_t err = 1;
	pnalu->spsppsPos = -1;
	pnalu->keyframePos = -1;
	while (pos < len) {
		tmp = videoFrame->payload + pos;
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeIDR) {
			pnalu->keyframePos = pos;
			break;
		}
		if ((*(tmp + 4) & kNalTypeMask) == YangAvcNaluTypeSPS) {
			pnalu->spsppsPos = pos;
		}
		naluLen = yang_get_be32(tmp);
		if (naluLen > len) {
			break;
		}
		pos += naluLen + 4;
	}
	return err;
}

int32_t yang_getNalupos(uint8_t* data,int plen){
	uint8_t* p=NULL;
	int len=plen-4;
	for(int i=0;i<len;i++){
		p=data+i;
		if((*p)==0x00&&(*(p+1))==0x00&&(*(p+2))==0x00&&(*(p+3))==0x01){
			return i+4;
		}
	}
	return -1;
}
int32_t yang_parseH264Nalu2(YangFrame *videoFrame, YangH264NaluData2 *pnalu) {
	uint8_t *tmp = NULL; //videoFrame->payload;
	uint32_t len = videoFrame->nb;
	uint32_t naluLen = 0;
	int32_t pos = 0;
	int32_t err = 1;
	int32_t tmpPos=0;
	int32_t tmpLen=len;
	pnalu->spsPos = -1;
	pnalu->ppsPos = -1;
	pnalu->keyframePos = -1;
	pnalu->spsLen=0;
	pnalu->ppsLen=0;
	int32_t preType=-1;
	while (pos < len) {

		tmp = videoFrame->payload + pos;
		tmpPos=yang_getNalupos(tmp,tmpLen);
		if(tmpPos==-1) return 1;

		if(preType==0){
			pnalu->spsLen=tmpPos-4;
			preType=-1;
		}
		if(preType==1){
				pnalu->ppsLen=tmpPos-4;
				preType=-1;
			}
		tmp+=tmpPos;

		if ((*tmp & kNalTypeMask) == YangAvcNaluTypeIDR) {
			pnalu->keyframePos =tmp-videoFrame->payload;
			preType=2;
			return Yang_Ok;
		}
		if ((*tmp & kNalTypeMask) == YangAvcNaluTypeSPS) {
			pnalu->spsPos = tmp-videoFrame->payload;
			preType=0;
		}
		if ((*tmp & kNalTypeMask) == YangAvcNaluTypePPS) {
					pnalu->ppsPos = tmp-videoFrame->payload;
					preType=1;
		}
		tmpLen-=tmpPos;
		pos+=tmpPos;



	}
	return err;
}
int32_t yang_getH264SpsppseNalu(YangFrame *videoFrame, uint8_t *pnaludata) {
	if (!videoFrame || !pnaludata || !videoFrame->payload)
		return yang_error_wrap(1, "getSpsppseNalu is null");
	int32_t spsLen = yang_get_be32(pnaludata);
	int32_t ppsLen = yang_get_be32(pnaludata + 4 + spsLen);
	if (spsLen > videoFrame->nb || ppsLen > videoFrame->nb)
		return 1;
	uint8_t *sps = pnaludata + 4;
	uint8_t *pps = pnaludata + 4 + spsLen + 4;

	uint8_t *p = videoFrame->payload;
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
	yang_put_be16((char*) p, (uint16_t) spsLen);
	p += 2;
	memcpy(p, sps, spsLen);
	p += spsLen;
	yang_put_be16((char*) p, (uint16_t) ppsLen);
	p += 2;
	memcpy(p, pps, ppsLen);
	p += ppsLen;


	videoFrame->nb = p - videoFrame->payload ;
	return Yang_Ok;
}

