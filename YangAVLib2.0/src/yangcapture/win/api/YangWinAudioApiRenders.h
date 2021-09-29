#ifndef YANGWINAUDIOAPIRENDERS_H
#define YANGWINAUDIOAPIRENDERS_H
#include "YangWinAudioApiRender.h"
#ifdef _WIN32
class YangWinAudioApiRenders:public YangWinAudioApiRender
{
public:
    YangWinAudioApiRenders(YangAudioInfo *pini);
    virtual   ~YangWinAudioApiRenders();
protected:

    void startLoop();
};
#endif
#endif // YANGWINAUDIOAPIRENDERS_H
