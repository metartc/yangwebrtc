

#ifndef SRS_APP_RTC_SDP_HPP
#define SRS_APP_RTC_SDP_HPP
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <yangrtp/YangRtpConstant.h>
#include <yangstream/YangStreamType.h>
#include <yangwebrtc/YangRtcDtls.h>
#include <yangwebrtc/YangRtcUserConfig.h>

const std::string kTWCCExt = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";

#define Yang_Fetch_Failed 200
#define Yang_Fetch_Failed_WithDelim 200


class YangMediaPayloadType;
class YangCodecPayload;

class YangSessionConfig
{
public:
    std::string m_dtls_role;
    std::string m_dtls_version;
};

class YangSessionInfo
{
public:
    YangSessionInfo();
    virtual ~YangSessionInfo();

    int32_t parse_attribute(const std::string& attribute, const std::string& value);
    int32_t encode(std::stringstream& os);

    bool operator=(const YangSessionInfo& rhs);
public:
    std::string m_ice_ufrag;
    std::string m_ice_pwd;
    std::string m_ice_options;
    std::string m_fingerprint_algo;
    std::string m_fingerprint;
    std::string m_setup;
};

class YangSSRCInfo
{
public:
    YangSSRCInfo();
    YangSSRCInfo(uint32_t ssrc, std::string cname, std::string stream_id, std::string track_id);
    virtual ~YangSSRCInfo();
public:
    int32_t encode(std::stringstream& os);
public:
    uint32_t m_ssrc;
    std::string m_cname;
    std::string m_msid;
    std::string m_msid_tracker;
    std::string m_mslabel;
    std::string m_label;
};

class YangSSRCGroup
{
public:
    YangSSRCGroup();
    YangSSRCGroup(const std::string& usage, const std::vector<uint32_t>& ssrcs);
    virtual ~YangSSRCGroup();
public:
    int32_t encode(std::ostringstream& os);
public:
    // e.g FIX, FEC, SIM.
    std::string m_semantic;
    // SSRCs of this type. 
    std::vector<uint32_t> m_ssrcs;
};

struct H264SpecificParam
{
    std::string profile_level_id;
    std::string packetization_mode;
    std::string level_asymmerty_allow;
};

//extern int32_t srs_parse_h264_fmtp(const std::string& fmtp, H264SpecificParam& h264_param);


struct YangCandidate
{
    std::string ip_;
    int32_t port_;
    std::string type_;
};

class YangMediaDesc
{
public:
    YangMediaDesc(const std::string& type);
    virtual ~YangMediaDesc();
public:
    int32_t parse_line(const std::string& line);
    int32_t encode(std::stringstream& os);
    YangMediaPayloadType* find_media_with_payload_type(int32_t payload_type);
    std::vector<YangMediaPayloadType> find_media_with_encoding_name(const std::string& encoding_name) const;
    const std::map<int, std::string>& get_extmaps() const { return m_extmaps; }
    int32_t update_msid(std::string id);

    bool is_audio() const { return m_type == "audio"; }
    bool is_video() const { return m_type == "video"; }
private:
    int32_t parse_attribute(const std::string& content);
    int32_t parse_attr_rtpmap(const std::string& value);
    int32_t parse_attr_rtcp(const std::string& value);
    int32_t parse_attr_rtcp_fb(const std::string& value);
    int32_t parse_attr_fmtp(const std::string& value);
    int32_t parse_attr_mid(const std::string& value);
    int32_t parse_attr_msid(const std::string& value);
    int32_t parse_attr_ssrc(const std::string& value);
    int32_t parse_attr_ssrc_group(const std::string& value);
    int32_t parse_attr_extmap(const std::string& value);
private:
    YangSSRCInfo& fetch_or_create_ssrc_info(uint32_t ssrc);

public:
    YangSessionInfo m_session_info;
    std::string m_type;
    int32_t m_port;

    bool m_rtcp_mux;
    bool m_rtcp_rsize;

    bool m_sendonly;
    bool m_recvonly;
    bool m_sendrecv;
    bool m_inactive;

    std::string m_mid;
    std::string m_msid;
    std::string m_msid_tracker;
    std::string m_protos;
    std::vector<YangMediaPayloadType> m_payload_types;

    std::vector<YangCandidate> m_candidates;
    std::vector<YangSSRCGroup> m_ssrc_groups;
    std::vector<YangSSRCInfo>  m_ssrc_infos;
    std::map<int, std::string> m_extmaps;
};

class YangSdp
{
public:
    YangSdp();
    virtual ~YangSdp();
public:
    int32_t parse(const std::string& sdp_str);
    int32_t encode(std::stringstream& os);
public:
    YangMediaDesc* find_media_descs(const std::string& type);
public:
    void set_ice_ufrag(const std::string& ufrag);
    void set_ice_pwd(const std::string& pwd);
    void set_dtls_role(const std::string& dtls_role);
    void set_fingerprint_algo(const std::string& algo);
    void set_fingerprint(const std::string& fingerprint);
    void add_candidate(const std::string& ip, const int& port, const std::string& type);

    std::string get_ice_ufrag() const;
    std::string get_ice_pwd() const;
    std::string get_dtls_role() const;

private:
    int32_t parse_line(const std::string& line);
private:
    int32_t parse_origin(const std::string& content);
    int32_t parse_version(const std::string& content);
    int32_t parse_session_name(const std::string& content);
    int32_t parse_timing(const std::string& content);
    int32_t parse_attribute(const std::string& content);
    int32_t parse_media_description(const std::string& content);
    int32_t parse_attr_group(const std::string& content);
private:
    bool m_in_media_session;
public:
    // version
    std::string m_version;

    // origin
    std::string m_username;
    std::string m_session_id;
    std::string m_session_version;
    std::string m_nettype;
    std::string m_addrtype;
    std::string m_unicast_address;

    // session_name
    std::string m_session_name;

    // timing
    int64_t m_start_time;
    int64_t m_end_time;

    YangSessionInfo m_session_info;
    YangSessionConfig m_session_config;
    YangSessionConfig m_session_negotiate;

    std::vector<std::string> m_groups;
    std::string m_group_policy;

    std::string m_msid_semantic;
    std::vector<std::string> m_msids;

    // m-line, media sessions
    std::vector<YangMediaDesc> m_media_descs;

     bool is_unified() const;
    // TODO: FIXME: will be fixed when use single pc.
    int32_t update_msid(std::string id);
};




class YangSdpHandle {
public:
	YangSdpHandle();
	virtual ~YangSdpHandle();
		//role0: play 1:publish 2:play&publish
	int32_t genLocalSdp(int32_t localport,YangRtcUserConfig* conf, char *sdp,YangStreamOptType role);
	void init(YangCertificate* pcer);

private:
	YangCertificate* m_cer;
	int32_t generate_local_sdp(YangRtcUserConfig* pconf, bool unified_plan,int32_t role);
	int32_t generate_publish_local_sdp(YangStreamConfig* req, YangSdp& local_sdp,  bool unified_plan);
};
#endif
