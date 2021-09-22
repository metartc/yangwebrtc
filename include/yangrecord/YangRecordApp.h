/*
 * YangRecZbHandle.h
 *
 *  Created on: 2020年10月8日
 *      Author: yang
 */

#ifndef YANGRECORD_INCLUDE_YANGRECORDAPP_H_
#define YANGRECORD_INCLUDE_YANGRECORDAPP_H_
#include <yangcapture/YangMultiVideoCapture.h>
#include "yangcapture/YangAudioCapture.h"
#include "YangMp4FileApp.h"
#include "YangRecordCapture.h"
class YangRecordApp {
public:
	YangRecordApp(YangAudioInfo *paudio,YangVideoInfo *pvideo,YangVideoEncInfo *penc);
	virtual ~YangRecordApp();
	YangRecordCapture *m_cap;
	YangMp4FileApp *m_rec;
	void init();
	void recordFile(char* filename);
	void stopRecord();
	void pauseRecord();
	void resumeRecord();

private:
	YangAudioInfo *m_audio;
			YangVideoInfo *m_video;
			YangVideoEncInfo *m_enc;


};

#endif /* YANGRECORD_INCLUDE_YANGRECORDAPP_H_ */
