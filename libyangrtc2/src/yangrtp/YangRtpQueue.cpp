#include <yangrtp/YangRtpConstant.h>
#include <yangrtp/YangRtpPacket.h>
#include <yangrtp/YangRtpQueue.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangutil/sys/YangLog.h>
YangRtcQueue::YangRtcQueue() {


}

YangRtcQueue::~YangRtcQueue() {

}


YangMixQueue::YangMixQueue()
{
    nb_videos = 0;
    nb_audios = 0;
    m_audio_size=YANG_MIX_CORRECT_PURE_AV_AUDIO;
    m_video_size=YANG_MIX_CORRECT_PURE_AV_VIDEO;
}

YangMixQueue::~YangMixQueue()
{
    clear();
}
void YangMixQueue::setAudioSize(uint32_t asize){
	m_audio_size=asize;
}
    void YangMixQueue::setVideoSize(uint32_t vsize){
    	m_video_size=vsize;
    }
void YangMixQueue::clear()
{
    std::multimap<int64_t, YangMessage*>::iterator it;
    for (it = msgs.begin(); it != msgs.end(); ++it) {
    	YangMessage* msg = it->second;
        yang_freep(msg);
    }
    msgs.clear();

    nb_videos = 0;
    nb_audios = 0;
}

void YangMixQueue::push(YangMessage* msg)
{
    msgs.insert(std::make_pair(msg->timestamp, msg));

    if (msg->mediaType==1) {
        nb_videos++;
    } else {
        nb_audios++;
    }
}

YangMessage* YangMixQueue::pop()
{
    bool mix_ok = false;

    // pure video
    if (nb_videos >= m_video_size && nb_audios == 0) {
        mix_ok = true;
    }

    // pure audio
    if (nb_audios >= m_audio_size && nb_videos == 0) {
        mix_ok = true;
    }

    // got 1 video and 1 audio, mix ok.
    if (nb_videos >= 1 && nb_audios >= 1) {
        mix_ok = true;
    }

    if (!mix_ok) {
        return NULL;
    }

    // pop the first msg.
    std::multimap<int64_t, YangMessage*>::iterator it = msgs.begin();
    YangMessage* msg = it->second;
    msgs.erase(it);

    if (msg->mediaType==1) {
        nb_videos--;
    } else {
        nb_audios--;
    }
   // printf("%d,",msg->mediaType);
    return msg;
}

YangRtpRingBuffer::YangRtpRingBuffer(int32_t capacity)
{
    m_nn_seq_flip_backs = 0;
    m_begin = m_end = 0;
    m_capacity = (uint16_t)capacity;
    m_contexttialized = false;

    m_queue = new YangRtpPacket*[m_capacity];
    memset(m_queue, 0, sizeof(YangRtpPacket*) * capacity);
}

YangRtpRingBuffer::~YangRtpRingBuffer()
{
    for (int32_t i = 0; i < m_capacity; ++i) {
        YangRtpPacket* pkt = m_queue[i];
        yang_freep(pkt);
    }
    yang_freepa(m_queue);
}

bool YangRtpRingBuffer::empty()
{
    return m_begin == m_end;
}

int32_t YangRtpRingBuffer::size()
{
    int32_t size = yang_rtp_seq_distance(m_begin, m_end);
    //srs_assert(size >= 0);
    return size;
}

void YangRtpRingBuffer::advance_to(uint16_t seq)
{
    m_begin = seq;
}

void YangRtpRingBuffer::set(uint16_t at, YangRtpPacket* pkt)
{
    YangRtpPacket* p = m_queue[at % m_capacity];
    yang_freep(p);

    m_queue[at % m_capacity] = pkt;
}

void YangRtpRingBuffer::remove(uint16_t at)
{
    set(at, NULL);
}

uint32_t YangRtpRingBuffer::get_extended_highest_sequence()
{
    return m_nn_seq_flip_backs * 65536 + m_end - 1;
}

bool YangRtpRingBuffer::update(uint16_t seq, uint16_t& nack_first, uint16_t& nack_last)
{
    if (!m_contexttialized) {
        m_contexttialized = true;
        m_begin = seq;
        m_end = seq + 1;
        return true;
    }

    // Normal sequence, seq follows high_.
    if (yang_rtp_seq_distance(m_end, seq) >= 0) {
        //TODO: FIXME: if diff_upper > limit_max_size clear?
        // int16_t diff_upper = yang_rtp_seq_distance(end, seq)
        // notify_nack_list_full()
        nack_first = m_end;
        nack_last = seq;

        // When distance(seq,high_)>0 and seq<high_, seq must flip back,
        // for example, high_=65535, seq=1, distance(65535,1)>0 and 1<65535.
        // TODO: FIXME: The first flip may be dropped.
        if (seq < m_end) {
            ++m_nn_seq_flip_backs;
        }
        m_end = seq + 1;
        // TODO: FIXME: check whether is neccessary?
        // yang_rtp_seq_distance(begin, end) > max_size
        // advance_to(), srs_rtp_seq_distance(begin, end) < max_size;
        return true;
    }

    // Out-of-order sequence, seq before low_.
    if (yang_rtp_seq_distance(seq, m_begin) > 0) {
        nack_first = seq;
        nack_last = m_begin;
        m_begin = seq;

        // TODO: FIXME: Maybe should support startup drop.
        return true;
        // When startup, we may receive packets in chaos order.
        // Because we don't know the ISN(initiazlie sequence number), the first packet
        // we received maybe no the first packet client sent.
        // @remark We only log a warning, because it seems ok for publisher.
        //return false;
    }

    return true;
}

YangRtpPacket* YangRtpRingBuffer::at(uint16_t seq) {
    return m_queue[seq % m_capacity];
}

void YangRtpRingBuffer::notify_nack_list_full()
{
    clear_all_histroy();

    m_begin = m_end = 0;
    m_contexttialized = false;
}

void YangRtpRingBuffer::notify_drop_seq(uint16_t seq)
{
    remove(seq);
    advance_to(seq+1);
}

void YangRtpRingBuffer::clear_histroy(uint16_t seq)
{
    // TODO FIXME Did not consider loopback
    for (uint16_t i = 0; i < m_capacity; i++) {
        YangRtpPacket* p = m_queue[i];
        if (p && p->m_header.get_sequence() < seq) {
            yang_freep(p);
            m_queue[i] = NULL;
        }
    }
}

void YangRtpRingBuffer::clear_all_histroy()
{
    for (uint16_t i = 0; i < m_capacity; i++) {
        YangRtpPacket* p = m_queue[i];
        if (p) {
            yang_freep(p);
            m_queue[i] = NULL;
        }
    }
}

YangNackOption::YangNackOption()
{
    max_count = 15;
    max_alive_time = 1000 * YANG_UTIME_MILLISECONDS;
    first_nack_interval = 10 * YANG_UTIME_MILLISECONDS;
    nack_interval = 50 * YANG_UTIME_MILLISECONDS;
    max_nack_interval = 500 * YANG_UTIME_MILLISECONDS;
    min_nack_interval = 15 * YANG_UTIME_MILLISECONDS;

    nack_check_interval = 15 * YANG_UTIME_MILLISECONDS;

    //TODO: FIXME: audio and video using diff nack strategy
    // video:
    // max_alive_time = 1 * YANG_UTIME_SECONDS
    // max_count = 15;
    // nack_interval = 50 * YANG_UTIME_MILLISECONDS
    //
    // audio:
    // DefaultRequestNackDelay = 30; //ms
    // DefaultLostPacketLifeTime = 600; //ms
    // FirstRequestInterval = 50;//ms
}

YangRtpNackInfo::YangRtpNackInfo()
{
    generate_time_ = yang_get_system_time();//yang_update_system_time();
    pre_req_nack_time_ = 0;
    req_nack_count_ = 0;
}

YangRtpNackForReceiver::YangRtpNackForReceiver(YangRtpRingBuffer* rtp, size_t queue_size)
{
    m_max_queue_size = queue_size;
    m_rtp = rtp;
    m_pre_check_time = 0;
    m_rtt = 0;


}

YangRtpNackForReceiver::~YangRtpNackForReceiver()
{
}

void YangRtpNackForReceiver::insert(uint16_t first, uint16_t last)
{
   for (uint16_t s = first; s != last; ++s) {
        m_queue[s] = YangRtpNackInfo();
    }
}

void YangRtpNackForReceiver::remove(uint16_t seq)
{
    m_queue.erase(seq);
}

YangRtpNackInfo* YangRtpNackForReceiver::find(uint16_t seq)
{
    std::map<uint16_t, YangRtpNackInfo>::iterator iter = m_queue.find(seq);

    if (iter == m_queue.end()) {
        return NULL;
    }

    return &(iter->second);
}

void YangRtpNackForReceiver::check_queue_size()
{
    if (m_queue.size() >= m_max_queue_size) {
        m_rtp->notify_nack_list_full();
        m_queue.clear();
    }
}

void YangRtpNackForReceiver::get_nack_seqs(YangRtcpNack& seqs, uint32_t& timeout_nacks)
{

    int64_t now = yang_get_system_time();

    int64_t interval = now - m_pre_check_time;

    if (interval < m_opts.nack_check_interval) {
        return;
    }
    m_pre_check_time = now;

    std::map<uint16_t, YangRtpNackInfo>::iterator iter = m_queue.begin();
    while (iter != m_queue.end()) {
        const uint16_t& seq = iter->first;
        YangRtpNackInfo& nack_info = iter->second;

        int32_t alive_time = now - nack_info.generate_time_;
        if (alive_time > m_opts.max_alive_time || nack_info.req_nack_count_ > m_opts.max_count) {
            ++timeout_nacks;
            m_rtp->notify_drop_seq(seq);
            m_queue.erase(iter++);
            continue;
        }

        // TODO:Statistics unorder packet.
        if (now - nack_info.generate_time_ < m_opts.first_nack_interval) {
            break;
        }

        int64_t nack_interval = yang_max(m_opts.min_nack_interval, m_opts.nack_interval / 3);
        if(m_opts.nack_interval < 50 * YANG_UTIME_MILLISECONDS){
            nack_interval = yang_max(m_opts.min_nack_interval, m_opts.nack_interval);
        }

        if (now - nack_info.pre_req_nack_time_ >= nack_interval ) {
            ++nack_info.req_nack_count_;
            nack_info.pre_req_nack_time_ = now;
            seqs.add_lost_sn(seq);
        }

        ++iter;
    }
}

void YangRtpNackForReceiver::update_rtt(int32_t rtt)
{
    m_rtt = rtt * YANG_UTIME_MILLISECONDS;

    if (m_rtt > m_opts.nack_interval) {
        m_opts.nack_interval = m_opts.nack_interval  * 0.8 + m_rtt * 0.2;
    } else {
        m_opts.nack_interval = m_rtt;
    }

    m_opts.nack_interval = yang_min(m_opts.nack_interval, m_opts.max_nack_interval);
}

YangRtpBuffer::YangRtpBuffer(YangStreamOptType role){
	m_bufLen=1500;//role==Yang_Stream_Play?1500:20;
	m_index=0;
	m_buffer=new char[1500*m_bufLen];

}
YangRtpBuffer::~YangRtpBuffer(){
	yang_deleteA(m_buffer);
}
char* YangRtpBuffer::getBuffer(){
	m_index++;
	if(m_index==m_bufLen) m_index=0;
	return m_buffer+1500*m_index;
}

