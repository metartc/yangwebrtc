#pragma once

#include <stdint.h>
#include <memory>
#include <map>

class YangTsBuffer;
class TsFrame;

class YangTsDemuxer
{
public:
    YangTsDemuxer();
    virtual ~YangTsDemuxer();

public:
    int decode(YangTsBuffer *in, TsFrame *&out);
    // stream, pid
    std::map<uint8_t, int> stream_pid_map;
    int pmt_id;

private:
    // pid, frame
    std::map<int, std::shared_ptr<TsFrame>> _ts_frames;
    int _pcr_id;
};

