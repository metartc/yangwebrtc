#ifndef INCLUDE_YANGCAPTURE_YANGAUDIODEVICEQUERY_H_
#define INCLUDE_YANGCAPTURE_YANGAUDIODEVICEQUERY_H_
#include <vector>
#include <string>

using namespace std;
struct YangAudioDeivce{
	string name;
	string deviceName;
	string subName;
	int aIndex;
	int aSubIndex;
	int aIdx;
};
class YangAudioDeviceQuery {
public:
	YangAudioDeviceQuery();
	virtual ~YangAudioDeviceQuery();
  	vector<YangAudioDeivce>* getCaptureDeviceList();
    vector<YangAudioDeivce>* getPlayDeviceList();
private:
    vector<YangAudioDeivce> m_captureDeviceList;
    vector<YangAudioDeivce> m_playDeviceList;
    void getDeviceList(int stream,vector<YangAudioDeivce>* plist);

};

#endif /* INCLUDE_YANGCAPTURE_YANGAUDIODEVICEQUERY_H_ */
