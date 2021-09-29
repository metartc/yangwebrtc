/*
 * YangPainterFactroy.cpp
 *
 *  Created on: 2020年9月17日
 *      Author: yang
 */

#include <yangplayer/YangWinPlayFactroy.h>

#include "yangplayer/YangWinPlaySdl.h"
YangPainter::YangPainter(){

}

YangPainter::~YangPainter(){

}

YangWinPlay::YangWinPlay(){
	 m_width=0,m_height=0;
}

YangWinPlay::~YangWinPlay(){

}
//YangPainter *YangWinPlayFactroy::createPainter(){
//	return new YangWinPlayPainter();
//}
YangWinPlay *YangWinPlayFactroy::createWinPlay(){
	return new YangWinPlaySdl();
}

YangWinPlayFactroy::YangWinPlayFactroy() {
	// TODO Auto-generated constructor stub

}

YangWinPlayFactroy::~YangWinPlayFactroy() {
	// TODO Auto-generated destructor stub
}

