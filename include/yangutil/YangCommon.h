/*
 * YangCommon.h
 *
 *  Created on: 2019年7月20日
 *      Author: yang
 */

#ifndef YANGUTIL_VIDEO_YANGCOMMON_H_
#define YANGUTIL_VIDEO_YANGCOMMON_H_

class YangCommon {
public:
	YangCommon();
	virtual ~YangCommon();
public:
	char * put_amf_string(char *c, const char *str);
	char * put_amf_double(char *c, double d);
	char * put_byte(char *output, unsigned char nVal);
	char * put_be16(char *output, unsigned short nVal);
	char * put_be24(char *output, unsigned int nVal);
	char * put_be32(char *output, unsigned int nVal);
	char * put_be64(char *output, unsigned long nVal);
};

#endif /* YANGUTIL_VIDEO_YANGCOMMON_H_ */
