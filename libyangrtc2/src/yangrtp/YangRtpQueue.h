#ifndef YANGWEBRTC_YANGRTCQUEUE_H_
#define YANGWEBRTC_YANGRTCQUEUE_H_
#include <yangrtp/YangRtcp.h>
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavtype.h>
#include <string>
#include <vector>
#include <map>
// when got these videos or audios, pure audio or video, mix ok.
#define YANG_MIX_CORRECT_PURE_AV_VIDEO 10
#define YANG_MIX_CORRECT_PURE_AV_AUDIO 5

class YangRtcQueue {
public:
	YangRtcQueue();
	virtual ~YangRtcQueue();
};

class YangRtpPacket;
class SrsRtpQueue;
class YangRtpRingBuffer;
class YangRtpBuffer{
public:
	YangRtpBuffer(int pbuflen,int unitsize=1500);
	~YangRtpBuffer();
public:
	char* getBuffer();
private:
	char* m_buffer;
	int32_t m_bufLen;
	int32_t m_index;

};
// The mix queue to correct the timestamp for mix_correct algorithm.
class YangMixQueue
{
private:
    uint32_t nb_videos;
    uint32_t nb_audios;
    int m_audio_size;
    int m_video_size;
    std::multimap<int64_t, YangMessage*> msgs;
public:
    YangMixQueue();
    virtual ~YangMixQueue();
public:
    virtual void clear();
    virtual void push(YangMessage* msg);
    virtual YangMessage* pop();
    void setAudioSize(uint32_t asize);
    void setVideoSize(uint32_t vsize);
};

// For UDP, the packets sequence may present as bellow:
//      [seq1(done)|seq2|seq3 ... seq10|seq11(lost)|seq12|seq13]
//                   \___(head_sequence_)   \               \___(highest_sequence_)
//                                           \___(no received, in nack list)
//      * seq1: The packet is done, we have already got and processed it.
//      * seq2,seq3,...,seq10,seq12,seq13: Theses packets are in queue and wait to be processed.
//      * seq10: This packet is lost or not received, we will put it in the nack list.
// We store the received packets in ring buffer.
class YangRtpRingBuffer
{
private:
    // Capacity of the ring-buffer.
    uint16_t m_capacity;
    // Ring bufer.
    YangRtpPacket** m_queue;
    // Increase one when uint16 flip back, for get_extended_highest_sequence.
    uint64_t m_nn_seq_flip_backs;
    // Whether initialized, because we use uint16 so we can't use -1.
    bool m_contexttialized;
public:
    // The begin iterator for ring buffer.
    // For example, when got 1 elems, the begin is 0.
    uint16_t m_begin;
    // The end iterator for ring buffer.
    // For example, when got 1 elems, the end is 1.
    uint16_t m_end;
public:
    YangRtpRingBuffer(int32_t capacity);
    virtual ~YangRtpRingBuffer();
public:
    // Whether the ring buffer is empty.
    bool empty();
    // Get the count of elems in ring buffer.
    int32_t size();
    // Move the low position of buffer to seq.
    void advance_to(uint16_t seq);
    // Free the packet at position.
    void set(uint16_t at, YangRtpPacket* pkt);
    void remove(uint16_t at);
    // The highest sequence number, calculate the flip back base.
    uint32_t get_extended_highest_sequence();
    // Update the sequence, got the nack range by [first, last).
    // @return If false, the seq is too old.
    bool update(uint16_t seq, uint16_t& nack_first, uint16_t& nack_last);
    // Get the packet by seq.
    YangRtpPacket* at(uint16_t seq);
public:
    // TODO: FIXME: Refine it?
    void notify_nack_list_full();
    void notify_drop_seq(uint16_t seq);
public:
    void clear_histroy(uint16_t seq);
    void clear_all_histroy();
};

struct YangNackOption
{
    int32_t max_count;
    int64_t max_alive_time;
    int64_t first_nack_interval;
    int64_t nack_interval;

    int64_t max_nack_interval;
    int64_t min_nack_interval;
    int64_t nack_check_interval;

    YangNackOption();
};

struct YangRtpNackInfo
{
    // Use to control the time of first nack req and the life of seq.
    int64_t generate_time_;
    // Use to control nack interval.
    int64_t pre_req_nack_time_;
    // Use to control nack times.
    int32_t req_nack_count_;

    YangRtpNackInfo();
};

class YangRtpNackForReceiver
{
private:
    // Nack queue, seq order, oldest to newest.
    std::map<uint16_t, YangRtpNackInfo, YangSeqCompareLess> m_queue;
    // Max nack count.
    size_t m_max_queue_size;
    YangRtpRingBuffer* m_rtp;
    YangNackOption m_opts;
private:
    int64_t m_pre_check_time;
private:
    int32_t m_rtt;
public:
    YangRtpNackForReceiver(YangRtpRingBuffer* rtp, size_t queue_size);
    virtual ~YangRtpNackForReceiver();
public:
    void insert(uint16_t first, uint16_t last);
    void remove(uint16_t seq);
    YangRtpNackInfo* find(uint16_t seq);
    void check_queue_size();
public:
    void get_nack_seqs(YangRtcpNack& seqs, uint32_t& timeout_nacks);
public:
    void update_rtt(int32_t rtt);
};

#endif /* YANGWEBRTC_YANGRTCQUEUE_H_ */
