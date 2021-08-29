/*
 * YangPainterFactroy.h
 *
 *  Created on: 2020年9月17日
 *      Author: yang
 */

#ifndef YANGPLAYER_INCLUDE_YANGWINPLAYFACTROY_H_
#define YANGPLAYER_INCLUDE_YANGWINPLAYFACTROY_H_
#include "yangutil/yangtype.h"
class YangPainter{
public:
	YangPainter();
	virtual ~YangPainter();
	virtual void draw(void *win,YangRect *prect,YangColor *pcolor)=0;
};
class YangWinPlay{
	public:
	YangWinPlay();
	virtual ~YangWinPlay();
	virtual void init(void* pid)=0;
	virtual void initBg(int pwid,int phei)=0;
	virtual	void initVideo(int pwid,int phei,YangYuvType sdfe)=0;
	virtual	void initText(char *pname,YangColor *pcolor)=0;
	//virtual	void initAll(char *pname,YangColor *pcolor,void* pid,int pwid,int phei,YangYuvType sdfe)=0;

	//virtual void reInitBg(int pwid,int phei)=0;
	virtual void reInitVideo(int pwid,int phei,YangYuvType sdfe)=0;
	virtual void reInitText(char *pname,YangColor *pcolor)=0;


	virtual void renderPreview(unsigned char* pdata)=0;
	virtual void render(unsigned char* pdata)=0;
	virtual void render(unsigned char* pdata,long long ptimestamp)=0;
	virtual void renderBg(YangColor *pcolor)=0;
	int m_width,m_height;
};
class YangWinPlayFactroy {
public:
	YangWinPlayFactroy();
	virtual ~YangWinPlayFactroy();
	YangPainter *createPainter();
	YangWinPlay *createWinPlay();
};

#endif /* YANGPLAYER_INCLUDE_YANGWINPLAYFACTROY_H_ */
