#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdarg.h>
#include <queue>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangSsrc.h>
#include <yangrtp/YangRtpCodec.h>
#include <yangutil/sys/YangSsl.h>
#include <yangwebrtc/YangRtcSdp.h>
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangLog.h>
#include <yangwebrtc/YangLocalSdp.h>
using namespace std;

const std::string kCRLF = "\r\n";

#define FETCH(is,word) \
if (!(is >> word)) {\
    return Yang_Fetch_Failed;\
}\

#define FETCH_WITH_DELIM(is,word,delim) \
if (!getline(is,word,delim)) {\
    return Yang_Fetch_Failed_WithDelim;\
}\



static void skip_first_spaces(std::string& str)
{
    while (! str.empty() && str[0] == ' ') {
        str.erase(0, 1);
    }
}

int32_t srs_parse_h264_fmtp(const std::string& fmtp, H264SpecificParam& h264_param)
{
    int32_t err = Yang_Ok;
    std::vector<std::string> vec = yang_splits(fmtp, ";");
    for (size_t i = 0; i < vec.size(); ++i) {
        std::vector<std::string> kv = yang_splits(vec[i], "=");
        if (kv.size() == 2) {
            if (kv[0] == "profile-level-id") {
                h264_param.profile_level_id = kv[1];
            } else if (kv[0] == "packetization-mode") {
                // 6.3.  Non-Interleaved Mode
                // This mode is in use when the value of the OPTIONAL packetization-mode
                // media type parameter is equal to 1.  This mode SHOULD be supported.
                // It is primarily intended for low-delay applications.  Only single NAL
                // unit packets, STAP-As, and FU-As MAY be used in this mode.  STAP-Bs,
                // MTAPs, and FU-Bs MUST NOT be used.  The transmission order of NAL
                // units MUST comply with the NAL unit decoding order.
                // @see https://tools.ietf.org/html/rfc6184#section-6.3
                h264_param.packetization_mode = kv[1];
            } else if (kv[0] == "level-asymmetry-allowed") {
                h264_param.level_asymmerty_allow = kv[1];
            } else {
                return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", kv[0].c_str());
            }
        } else {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", vec[i].c_str());
        }
    }

    return err;
}




YangSdpHandle::YangSdpHandle() {
	m_cer=NULL;
}

YangSdpHandle::~YangSdpHandle() {
	m_cer=NULL;
}
void YangSdpHandle::init(YangCertificate* pcer){
	m_cer=pcer;
}
int32_t YangSdpHandle::genLocalSdp(int32_t localport,YangRtcUserConfig* pconf, char *sdp,YangStreamOptType role) {
	//char s[1024 * 10] = { 0 };
	/**FILE *f = NULL;
	char file_path_getcwd[210]={0};
	getcwd(file_path_getcwd, 210);
	char path[255]={0};

	if(role==Yang_Stream_Play)
		sprintf(path,"%s/lib/local_play.sdp",file_path_getcwd );
	else
		sprintf(path,"%s/lib/local_publish.sdp",file_path_getcwd);

	f=fopen(path, "rb");

	int32_t fileLen = fread(s, 1024 * 10, 1, f);
	fclose(f);
	f = NULL;**/

	//printf("\n******************************gensdp localport==%d\n",localport);
	string ss(local_sdp_str);
	yang_replace(ss,"5hcuv2i7jtwg6bj1",yang_random_str(16));
	string msaid=yang_random_str(8)+"-"+yang_random_str(4)+"-"+yang_random_str(4)+"-"+yang_random_str(4)+"-"+yang_random_str(12);
	yang_replace(ss,"79ac70ad-c572-4017-86a1-330d56089c70",msaid);

	yang_replace(ss,"144706935",to_string(pconf->audioSsrc));
	string msvid=yang_random_str(8)+"-"+yang_random_str(4)+"-"+yang_random_str(4)+"-"+yang_random_str(4)+"-"+yang_random_str(12);
	yang_replace(ss,"549dbfe5-fd12-4c5f-9182-d513f6defce5",msvid);
	yang_replace(ss,"144706936",to_string(pconf->videoSsrc));
	yang_replace(ss,"127.0.0.1 8000",pconf->streamConf->serverIp+" "+to_string(localport));
	if(pconf->local_audio&&pconf->local_audio->audioEncoderType==Yang_AED_OPUS){
		char str1[64]={0};
		sprintf(str1,"opus/%d/%d",pconf->local_audio->sample,pconf->local_audio->channel);
		yang_replace(ss,"opus/48000/2",str1);
	}
	if(role==Yang_Stream_Play)
		yang_replace(ss,"a=sendonly\n","a=recvonly\n");


	 //,sample_sdp;
	pconf->local_sdp->parse(ss);
	//local_sdp.parse(s);

	pconf->local_sdp->set_ice_ufrag(yang_random_str(4));
	pconf->local_sdp->set_ice_pwd(yang_random_str(32));
	pconf->local_sdp->set_fingerprint_algo("sha-256");
	pconf->local_sdp->set_fingerprint(m_cer->get_fingerprint());
	pconf->local_sdp->m_session_negotiate = pconf->local_sdp->m_session_config;
	pconf->local_sdp->m_session_negotiate.m_dtls_role = "actpass"; //"passive","active";
	pconf->local_sdp->set_dtls_role(pconf->local_sdp->m_session_negotiate.m_dtls_role);


	generate_local_sdp(pconf, 1, role);
	stringstream os;
	int32_t err = 0;
	if ((err = pconf->local_sdp->encode(os)) != Yang_Ok) {
		printf("\nencode sdp error==%d", err);
		return 1;
	}

	std::string local_sdp_str = os.str();
	// Filter the \r\n to \\r\\n for JSON.
	//local_sdp_str = srs_string_replace(local_sdp_str.c_str(), "\r\n", "\\r\\n");
	yang_replace(local_sdp_str,"\r\n", "\\r\\n");
	memcpy(sdp, local_sdp_str.c_str(), local_sdp_str.length());

	return Yang_Ok;

}


int32_t YangSdpHandle::generate_local_sdp(YangRtcUserConfig* pconf, bool unified_plan, int32_t role) {
	int32_t err = Yang_Ok;

	pconf->local_sdp->m_version = "0";

	pconf->local_sdp->m_username = "YangRtc";
	pconf->local_sdp->m_session_id = yang_int2str(((int64_t) this));
	pconf->local_sdp->m_session_version = "2";
	pconf->local_sdp->m_nettype = "IN";
	pconf->local_sdp->m_addrtype = "IP4";
	pconf->local_sdp->m_unicast_address = "0.0.0.0";


	pconf->local_sdp->m_session_name = "YangRtcSession";

	pconf->local_sdp->m_msid_semantic = "WMS";
	std::string stream_id = pconf->streamConf->app + "/" + pconf->streamConf->stream;
	pconf->local_sdp->m_msids.push_back(stream_id);

	pconf->local_sdp->m_group_policy = "BUNDLE";

	//std::string cname = srs_random_str(16);
	YangMediaDesc *audio_media_desc = pconf->local_sdp->find_media_descs("audio");//new SrsMediaDesc("audio");//
	audio_media_desc->m_port = 9;
	audio_media_desc->m_protos = "UDP/TLS/RTP/SAVPF";
	audio_media_desc->m_rtcp_mux = true;
	audio_media_desc->m_rtcp_rsize = true;
	std::vector<YangMediaPayloadType> payloads =audio_media_desc->find_media_with_encoding_name("opus");

	YangMediaDesc *video_media_desc =pconf->local_sdp->find_media_descs("video"); //SrsMediaDesc("video");
	video_media_desc->m_port = 9;
	video_media_desc->m_protos = "UDP/TLS/RTP/SAVPF";
	video_media_desc->m_rtcp_mux = true;
	video_media_desc->m_rtcp_rsize = true;

	//sendrecv
	if (role==0) {
		audio_media_desc->m_recvonly = true;
		video_media_desc->m_recvonly = true;
	} else if(role==1) {
		audio_media_desc->m_sendonly = true;
		video_media_desc->m_sendonly = true;
	}else if(role==2){
		audio_media_desc->m_sendrecv=true;
		video_media_desc->m_sendrecv = true;
	}





	return err;
}

YangSessionInfo::YangSessionInfo()
{
}

YangSessionInfo::~YangSessionInfo()
{
}

int32_t YangSessionInfo::parse_attribute(const std::string& attribute, const std::string& value)
{
    int32_t err = Yang_Ok;

    if (attribute == "ice-ufrag") {
        m_ice_ufrag = value;
    } else if (attribute == "ice-pwd") {
        m_ice_pwd = value;
    } else if (attribute == "ice-options") {
        m_ice_options = value;
    } else if (attribute == "fingerprint") {
        std::istringstream is(value);
        FETCH(is, m_fingerprint_algo);
        FETCH(is, m_fingerprint);
    } else if (attribute == "setup") {
        // @see: https://tools.ietf.org/html/rfc4145#section-4
        m_setup = value;
    } else {
       // yang_trace("ignore attribute=%s, value=%s", attribute.c_str(), value.c_str());
    }

    return err;
}

int32_t YangSessionInfo::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    if (!m_ice_ufrag.empty()) {
        os << "a=ice-ufrag:" << m_ice_ufrag << kCRLF;
    }
    
    if (!m_ice_pwd.empty()) {
        os << "a=ice-pwd:" << m_ice_pwd << kCRLF;
    }

    // For ICE-lite, we never set the trickle.
    if (!m_ice_options.empty()) {
        os << "a=ice-options:" << m_ice_options << kCRLF;
    }
    
    if (!m_fingerprint_algo.empty() && ! m_fingerprint.empty()) {
        os << "a=fingerprint:" << m_fingerprint_algo << " " << m_fingerprint << kCRLF;
    }
    
    if (!m_setup.empty()) {
        os << "a=setup:" << m_setup << kCRLF;
    }

    return err;
}

bool YangSessionInfo::operator=(const YangSessionInfo& rhs)
{
    return m_ice_ufrag        == rhs.m_ice_ufrag &&
           m_ice_pwd          == rhs.m_ice_pwd &&
           m_ice_options      == rhs.m_ice_options &&
           m_fingerprint_algo == rhs.m_fingerprint_algo &&
           m_fingerprint      == rhs.m_fingerprint &&
           m_setup            == rhs.m_setup;
}

YangSSRCInfo::YangSSRCInfo()
{
    m_ssrc = 0;
}

YangSSRCInfo::YangSSRCInfo(uint32_t ssrc, std::string cname, std::string stream_id, std::string track_id)
{
    m_ssrc = ssrc;
    m_cname = cname;
    m_msid = stream_id;
    m_msid_tracker = track_id;
    m_mslabel = m_msid;
    m_label = m_msid_tracker;
}

YangSSRCInfo::~YangSSRCInfo()
{
}

int32_t YangSSRCInfo::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    if (m_ssrc == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc");
    }

    os << "a=ssrc:" << m_ssrc << " cname:" << m_cname << kCRLF;
    if (!m_msid.empty()) {
        os << "a=ssrc:" << m_ssrc << " msid:" << m_msid;
        if (!m_msid_tracker.empty()) {
            os << " " << m_msid_tracker;
        }
        os << kCRLF;
    }
    if (!m_mslabel.empty()) {
        os << "a=ssrc:" << m_ssrc << " mslabel:" << m_mslabel << kCRLF;
    }
    if (!m_label.empty()) {
        os << "a=ssrc:" << m_ssrc << " label:" << m_label << kCRLF;
    }

    return err;
}

YangSSRCGroup::YangSSRCGroup()
{
}

YangSSRCGroup::~YangSSRCGroup()
{
}

YangSSRCGroup::YangSSRCGroup(const std::string& semantic, const std::vector<uint32_t>& ssrcs)
{
    m_semantic = semantic;
    m_ssrcs = ssrcs;
}

int32_t YangSSRCGroup::encode(std::ostringstream& os)
{
    int32_t err = Yang_Ok;

    if (m_semantic.empty()) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid semantics");
    }

    if (m_ssrcs.size() == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrcs");
    }

    os << "a=ssrc-group:" << m_semantic;
    for (int32_t i = 0; i < (int)m_ssrcs.size(); i++) {
        os << " " << m_ssrcs[i];
    }

    return err;
}

YangMediaPayloadType::YangMediaPayloadType(int32_t payload_type)
{
    m_payload_type = payload_type;
    m_clock_rate = 0;
}

YangMediaPayloadType::~YangMediaPayloadType()
{
}

int32_t YangMediaPayloadType::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    os << "a=rtpmap:" << m_payload_type << " " << m_encoding_name << "/" << m_clock_rate;
    if (!m_encoding_param.empty()) {
        os << "/" << m_encoding_param;
    }
    os << kCRLF;

    for (std::vector<std::string>::iterator iter = m_rtcp_fb.begin(); iter != m_rtcp_fb.end(); ++iter) {
        os << "a=rtcp-fb:" << m_payload_type << " " << *iter << kCRLF;
    }

    if (!m_format_specific_param.empty()) {
        os << "a=fmtp:" << m_payload_type << " " << m_format_specific_param
           // TODO: FIXME: Remove the test code bellow.
           // << ";x-google-max-bitrate=6000;x-google-min-bitrate=5100;x-google-start-bitrate=5000"
           << kCRLF;
    }

    return err;
}

YangMediaDesc::YangMediaDesc(const std::string& type)
{
    m_type = type;

    m_port = 0;
    m_rtcp_mux = false;
    m_rtcp_rsize = false;

    m_sendrecv = false;
    m_recvonly = false;
    m_sendonly = false;
    m_inactive = false;
}

YangMediaDesc::~YangMediaDesc()
{
}

YangMediaPayloadType* YangMediaDesc::find_media_with_payload_type(int32_t payload_type)
{
    for (size_t i = 0; i < m_payload_types.size(); ++i) {
        if (m_payload_types[i].m_payload_type == payload_type) {
            return &m_payload_types[i];
        }
    }

    return NULL;
}

vector<YangMediaPayloadType> YangMediaDesc::find_media_with_encoding_name(const std::string& encoding_name) const
{
    std::vector<YangMediaPayloadType> payloads;

    std::string lower_name(encoding_name), upper_name(encoding_name);
    transform(encoding_name.begin(), encoding_name.end(), lower_name.begin(), ::tolower);
    transform(encoding_name.begin(), encoding_name.end(), upper_name.begin(), ::toupper);

    for (size_t i = 0; i < m_payload_types.size(); ++i) {
        if (m_payload_types[i].m_encoding_name == std::string(lower_name.c_str()) ||
            m_payload_types[i].m_encoding_name == std::string(upper_name.c_str())) {
            payloads.push_back(m_payload_types[i]);
        }
    }

    return payloads;
}

int32_t YangMediaDesc::update_msid(string id)
{
    int32_t err = Yang_Ok;

    for(vector<YangSSRCInfo>::iterator it = m_ssrc_infos.begin(); it != m_ssrc_infos.end(); ++it) {
        YangSSRCInfo& info = *it;

        info.m_msid = id;
        info.m_mslabel = id;
    }

    return err;
}

int32_t YangMediaDesc::parse_line(const std::string& line)
{
    int32_t err = Yang_Ok;
    std::string content = line.substr(2);

    switch (line[0]) {
        case 'a': {
            return parse_attribute(content);
        }
        case 'c': {
            // TODO: process c-line
            break;
        }
        default: {
            yang_trace("ignore media line=%s", line.c_str());
            break;
        }
    }

    return err;
}

int32_t YangMediaDesc::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    os << "m=" << m_type << " " << m_port << " " << m_protos;
    for (std::vector<YangMediaPayloadType>::iterator iter = m_payload_types.begin(); iter != m_payload_types.end(); ++iter) {
        os << " " << iter->m_payload_type;
    }

    os << kCRLF;

    // TODO:nettype and address type
    os << "c=IN IP4 0.0.0.0" << kCRLF;

    if ((err = m_session_info.encode(os)) != Yang_Ok) {
        return printf("encode session info failed");
    }

    os << "a=mid:" << m_mid << kCRLF;
    if (!m_msid.empty()) {
        os << "a=msid:" << m_msid;
        
        if (!m_msid_tracker.empty()) {
            os << " " << m_msid_tracker;
        }

        os << kCRLF;
    }

    for(map<int, string>::iterator it = m_extmaps.begin(); it != m_extmaps.end(); ++it) {
        os << "a=extmap:"<< it->first<< " "<< it->second<< kCRLF;
    }
    if (m_sendonly) {
        os << "a=sendonly" << kCRLF;
    }
    if (m_recvonly) {
        os << "a=recvonly" << kCRLF;
    }
    if (m_sendrecv) {
        os << "a=sendrecv" << kCRLF;
    }
    if (m_inactive) {
        os << "a=inactive" << kCRLF;
    }

    if (m_rtcp_mux) {
        os << "a=rtcp-mux" << kCRLF;
    }

    if (m_rtcp_rsize) {
        os << "a=rtcp-rsize" << kCRLF;
    }

    for (std::vector<YangMediaPayloadType>::iterator iter = m_payload_types.begin(); iter != m_payload_types.end(); ++iter) {
        if ((err = iter->encode(os)) != Yang_Ok) {
            return printf("encode media payload failed");
        }
    }

    for (std::vector<YangSSRCInfo>::iterator iter = m_ssrc_infos.begin(); iter != m_ssrc_infos.end(); ++iter) {
        YangSSRCInfo& ssrc_info = *iter;

        if ((err = ssrc_info.encode(os)) != Yang_Ok) {
            return printf("encode ssrc failed");
        }
    }

    int32_t foundation = 0;
    int32_t component_id = 1; /* RTP */
    for (std::vector<YangCandidate>::iterator iter = m_candidates.begin(); iter != m_candidates.end(); ++iter) {
        // @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-4.2
        uint32_t priority = (1<<24)*(126) + (1<<8)*(65535) + (1)*(256 - component_id);

        // @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
        os << "a=candidate:" << foundation++ << " "
           << component_id << " udp " << priority << " "
           << iter->ip_ << " " << iter->port_
           << " typ " << iter->type_ 
           << " generation 0" << kCRLF;

        yang_trace("local SDP candidate line=%s", os.str().c_str());
    }

    return err;
}

int32_t YangMediaDesc::parse_attribute(const std::string& content)
{
    int32_t err = Yang_Ok;
    std::string attribute = "";
    std::string value = "";
    size_t pos = content.find_first_of(":");

    if (pos != std::string::npos) {
        attribute = content.substr(0, pos);
        value = content.substr(pos + 1);
    } else {
        attribute = content;
    }

    if (attribute == "extmap") {
        return parse_attr_extmap(value);
    } else if (attribute == "rtpmap") {
        return parse_attr_rtpmap(value);
    } else if (attribute == "rtcp") {
        return parse_attr_rtcp(value);
    } else if (attribute == "rtcp-fb") {
        return parse_attr_rtcp_fb(value);
    } else if (attribute == "fmtp") {
        return parse_attr_fmtp(value);
    } else if (attribute == "mid") {
        return parse_attr_mid(value);
    } else if (attribute == "msid") {
        return parse_attr_msid(value);
    } else if (attribute == "ssrc") {
        return parse_attr_ssrc(value);
    } else if (attribute == "ssrc-group") {
        return parse_attr_ssrc_group(value);
    } else if (attribute == "rtcp-mux") {
        m_rtcp_mux = true;
    } else if (attribute == "rtcp-rsize") {
        m_rtcp_rsize = true;
    } else if (attribute == "recvonly") {
        m_recvonly = true;
    } else if (attribute == "sendonly") {
        m_sendonly = true;
    } else if (attribute == "sendrecv") {
        m_sendrecv = true;
    } else if (attribute == "inactive") {
        m_inactive = true;
    } else {
        return m_session_info.parse_attribute(attribute, value);
    }

    return err;
}
int32_t YangMediaDesc::parse_attr_extmap(const std::string& value)
{
    int32_t err = Yang_Ok;
    std::istringstream is(value);
    int32_t id = 0;
    FETCH(is, id);
    if(m_extmaps.end() != m_extmaps.find(id)) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "duplicate ext id: %d", id);
    }
    string ext;
    FETCH(is, ext);
    m_extmaps[id] = ext;
    return err;
}

int32_t YangMediaDesc::parse_attr_rtpmap(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#page-25
    // a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]

    std::istringstream is(value);

    int32_t payload_type = 0;
    FETCH(is, payload_type);

    YangMediaPayloadType* payload = find_media_with_payload_type(payload_type);
    if (payload == NULL) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase rtpmap", payload_type);
    }

    std::string word;
    FETCH(is, word);

    std::vector<std::string> vec = yang_splits(word, "/");
    if (vec.size() < 2) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid rtpmap line=%s", value.c_str());
    }

    payload->m_encoding_name = vec[0];
    payload->m_clock_rate = atoi(vec[1].c_str());

    if (vec.size() == 3) {
        payload->m_encoding_param = vec[2];
    }

    return err;
}

int32_t YangMediaDesc::parse_attr_rtcp(const std::string& value)
{
    int32_t err = Yang_Ok;

    // TODO:parse rtcp attribute

    return err;
}

int32_t YangMediaDesc::parse_attr_rtcp_fb(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5104#section-7.1

    std::istringstream is(value);

    int32_t payload_type = 0;
    FETCH(is, payload_type);

    YangMediaPayloadType* payload = find_media_with_payload_type(payload_type);
    if (payload == NULL) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase rtcp-fb", payload_type);
    }

    std::string rtcp_fb = is.str().substr(is.tellg());
    skip_first_spaces(rtcp_fb);

    payload->m_rtcp_fb.push_back(rtcp_fb);

    return err;
}

int32_t YangMediaDesc::parse_attr_fmtp(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#page-30
    // a=fmtp:<format> <format specific parameters>

    std::istringstream is(value);

    int32_t payload_type = 0;
    FETCH(is, payload_type);

    YangMediaPayloadType* payload = find_media_with_payload_type(payload_type);
    if (payload == NULL) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase fmtp", payload_type);
    }

    std::string word;
    FETCH(is, word);

    payload->m_format_specific_param = word;

    return err;
}

int32_t YangMediaDesc::parse_attr_mid(const std::string& value)
{
    // @see: https://tools.ietf.org/html/rfc3388#section-3
    int32_t err = Yang_Ok;
    std::istringstream is(value);
    // mid_ means m-line id
    FETCH(is, m_mid);
    yang_trace("mid=%s", m_mid.c_str());
    return err;
}

int32_t YangMediaDesc::parse_attr_msid(const std::string& value)
{
    // @see: https://tools.ietf.org/id/draft-ietf-mmusic-msid-08.html#rfc.section.2
    // TODO: msid and msid_tracker
    int32_t err = Yang_Ok;
    std::istringstream is(value);
    // msid_ means media stream id
    FETCH(is, m_msid);
    is >> m_msid_tracker;
    return err;
}

int32_t YangMediaDesc::parse_attr_ssrc(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5576#section-4.1

    std::istringstream is(value);

    uint32_t ssrc = 0;
    FETCH(is, ssrc);

    std::string ssrc_attr = "";
    FETCH_WITH_DELIM(is, ssrc_attr, ':');
    skip_first_spaces(ssrc_attr);

    std::string ssrc_value = is.str().substr(is.tellg());
    skip_first_spaces(ssrc_value);

    YangSSRCInfo& ssrc_info = fetch_or_create_ssrc_info(ssrc);

    if (ssrc_attr == "cname") {
        // @see: https://tools.ietf.org/html/rfc5576#section-6.1
        ssrc_info.m_cname = ssrc_value;
        ssrc_info.m_ssrc = ssrc;
    } else if (ssrc_attr == "msid") {
        // @see: https://tools.ietf.org/html/draft-alvestrand-mmusic-msid-00#section-2
        std::vector<std::string> vec = yang_splits(ssrc_value, " ");
        if (vec.empty()) {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc line=%s", value.c_str());
        }

        ssrc_info.m_msid = vec[0];
        if (vec.size() > 1) {
            ssrc_info.m_msid_tracker = vec[1];
        }
    } else if (ssrc_attr == "mslabel") {
        ssrc_info.m_mslabel = ssrc_value;
    } else if (ssrc_attr == "label") {
        ssrc_info.m_label = ssrc_value;
    }

    return err;
}

int32_t YangMediaDesc::parse_attr_ssrc_group(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5576#section-4.2
    // a=ssrc-group:<semantics> <ssrc-id> ...

    std::istringstream is(value);

    std::string semantics;
    FETCH(is, semantics);

    std::string ssrc_ids = is.str().substr(is.tellg());
    skip_first_spaces(ssrc_ids);

    std::vector<std::string> vec = yang_splits(ssrc_ids, " ");
    if (vec.size() == 0) {
        return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid ssrc-group line=%s", value.c_str());
    }

    std::vector<uint32_t> ssrcs;
    for (size_t i = 0; i < vec.size(); ++i) {
        std::istringstream in_stream(vec[i]);
        uint32_t ssrc = 0;
        in_stream >> ssrc;
        ssrcs.push_back(ssrc);
    }
    m_ssrc_groups.push_back(YangSSRCGroup(semantics, ssrcs));

    return err;
}

YangSSRCInfo& YangMediaDesc::fetch_or_create_ssrc_info(uint32_t ssrc)
{
    for (size_t i = 0; i < m_ssrc_infos.size(); ++i) {
        if (m_ssrc_infos[i].m_ssrc == ssrc) {
            return m_ssrc_infos[i];
        }
    }

    YangSSRCInfo ssrc_info;
    ssrc_info.m_ssrc = ssrc;
    m_ssrc_infos.push_back(ssrc_info);

    return m_ssrc_infos.back();
}

YangSdp::YangSdp()
{
    m_in_media_session = false;
    
    m_start_time = 0;
    m_end_time = 0;
}

YangSdp::~YangSdp()
{
}

int32_t YangSdp::parse(const std::string& sdp_str)
{
    int32_t err = Yang_Ok;
    //printf("\nsdp===%s\n",sdp_str.c_str());
    // All webrtc SrsSdp annotated example
    // @see: https://tools.ietf.org/html/draft-ietf-rtcweb-SrsSdp-11
    // Sdp example
    // session info
    // v=
    // o=
    // s=
    // t=
    // media description
    // m=
    // a=
    // ...
    // media description
    // m=
    // a=
    // ...
    std::istringstream is(sdp_str);
    std::string line;
    while (getline(is, line)) {
        //yang_trace("%s", line.c_str());
        if (line.size() < 2 || line[1] != '=') {
            return yang_error_wrap(ERROR_RTC_SDP_DECODE, "invalid sdp line=%s", line.c_str());
        }
        if (!line.empty() && line[line.size()-1] == '\r') {
            line.erase(line.size()-1, 1);
        }

        if ((err = parse_line(line)) != Yang_Ok) {
            return printf("parse sdp line failed");
        }
    }

    // The msid/tracker/mslabel is optional for SSRC, so we copy it when it's empty.
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc& media_desc = *iter;

        for (size_t i = 0; i < media_desc.m_ssrc_infos.size(); ++i) {
            YangSSRCInfo& ssrc_info = media_desc.m_ssrc_infos.at(i);

            if (ssrc_info.m_msid.empty()) {
                ssrc_info.m_msid  = media_desc.m_msid;
            }

            if (ssrc_info.m_msid_tracker.empty()) {
                ssrc_info.m_msid_tracker = media_desc.m_msid_tracker;
            }

            if (ssrc_info.m_mslabel.empty()) {
                ssrc_info.m_mslabel = media_desc.m_msid;
            }

            if (ssrc_info.m_label.empty()) {
                ssrc_info.m_label = media_desc.m_msid_tracker;
            }
        }
    }

    return err;
}

int32_t YangSdp::encode(std::stringstream& os)
{
    int32_t err = Yang_Ok;

    os << "v=" << m_version << kCRLF;
    os << "o=" << m_username << " " << m_session_id << " " << m_session_version << " " << m_nettype << " " << m_addrtype << " " << m_unicast_address << kCRLF;
    os << "s=" << m_session_name << kCRLF;
    os << "t=" << m_start_time << " " << m_end_time << kCRLF;
    // ice-lite is a minimal version of the ICE specification, intended for servers running on a public IP address.
    os << "a=ice-lite" << kCRLF;

    if (!m_groups.empty()) {
        os << "a=group:" << m_group_policy;
        for (std::vector<std::string>::iterator iter = m_groups.begin(); iter != m_groups.end(); ++iter) {
            os << " " << *iter;
        }
        os << kCRLF;
    }

    os << "a=msid-semantic: " << m_msid_semantic;
    for (std::vector<std::string>::iterator iter = m_msids.begin(); iter != m_msids.end(); ++iter) {
        os << " " << *iter;
    }
    os << kCRLF;

    if ((err = m_session_info.encode(os)) != Yang_Ok) {
        return printf("encode session info failed");
    }

    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        if ((err = (*iter).encode(os)) != Yang_Ok) {
            return printf("encode media description failed");
        }
    }

    return err;
}

YangMediaDesc* YangSdp::find_media_descs(const std::string& type)
{
   // std::vector<SrsMediaDesc*> descs;
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);

        if (desc->m_type == type) {
            return desc;//descs.push_back(desc);
        }
    }

    return NULL;
}

void YangSdp::set_ice_ufrag(const std::string& ufrag)
{
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);
        desc->m_session_info.m_ice_ufrag = ufrag;
    }
}

void YangSdp::set_ice_pwd(const std::string& pwd)
{
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);
        desc->m_session_info.m_ice_pwd = pwd;
    }
}

void YangSdp::set_dtls_role(const std::string& dtls_role)
{
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);
        desc->m_session_info.m_setup = dtls_role;
    }
}

void YangSdp::set_fingerprint_algo(const std::string& algo)
{
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);
        desc->m_session_info.m_fingerprint_algo = algo;
    }
}

void YangSdp::set_fingerprint(const std::string& fingerprint)
{
    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);
        desc->m_session_info.m_fingerprint = fingerprint;
    }
}

void YangSdp::add_candidate(const std::string& ip, const int& port, const std::string& type)
{
    // @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
    YangCandidate candidate;
    candidate.ip_ = ip;
    candidate.port_ = port;
    candidate.type_ = type;

    for (std::vector<YangMediaDesc>::iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        YangMediaDesc* desc = &(*iter);
        desc->m_candidates.push_back(candidate);
    }
}

std::string YangSdp::get_ice_ufrag() const
{
    // Becaues we use BUNDLE, so we can choose the first element.
    for (std::vector<YangMediaDesc>::const_iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        const YangMediaDesc* desc = &(*iter);
        return desc->m_session_info.m_ice_ufrag;
    }

    return "";
}

std::string YangSdp::get_ice_pwd() const
{
    // Becaues we use BUNDLE, so we can choose the first element.
    for (std::vector<YangMediaDesc>::const_iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        const YangMediaDesc* desc = &(*iter);
        return desc->m_session_info.m_ice_pwd;
    }

    return "";
}

std::string YangSdp::get_dtls_role() const
{
    // Becaues we use BUNDLE, so we can choose the first element.
    for (std::vector<YangMediaDesc>::const_iterator iter = m_media_descs.begin(); iter != m_media_descs.end(); ++iter) {
        const YangMediaDesc* desc = &(*iter);
        return desc->m_session_info.m_setup;
    }

    return "";
}

int32_t YangSdp::parse_line(const std::string& line)
{
    int32_t err = Yang_Ok;

    std::string content = line.substr(2);

    switch (line[0]) {
        case 'o': {
            return parse_origin(content);
        }
        case 'v': {
            return parse_version(content);
        }
        case 's': {
            return parse_session_name(content);
        }
        case 't': {
            return parse_timing(content);
        }
        case 'a': {
            if (m_in_media_session) {
                return m_media_descs.back().parse_line(line);
            }
            return parse_attribute(content);
        }
        case 'm': {
            return parse_media_description(content);
        }
        case 'c': {

            break;
        }
        default: {
            yang_trace("ignore sdp line=%s", line.c_str());
            break;
        }
    }

    return err;
}

int32_t YangSdp::parse_origin(const std::string& content)
{
    int32_t err = Yang_Ok;

    // @see: https://tools.ietf.org/html/rfc4566#section-5.2
    // o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
    // eg. o=- 9164462281920464688 2 IN IP4 127.0.0.1
    std::istringstream is(content);

    FETCH(is, m_username);
    FETCH(is, m_session_id);
    FETCH(is, m_session_version);
    FETCH(is, m_nettype);
    FETCH(is, m_addrtype);
    FETCH(is, m_unicast_address);

    return err;
}

int32_t YangSdp::parse_version(const std::string& content)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#section-5.1

    std::istringstream is(content);

    FETCH(is, m_version);

    return err;
}

int32_t YangSdp::parse_session_name(const std::string& content)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#section-5.3
    // s=<session name>

    m_session_name = content;

    return err;
}

int32_t YangSdp::parse_timing(const std::string& content)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#section-5.9
    // t=<start-time> <stop-time>
    
    std::istringstream is(content);

    FETCH(is, m_start_time);
    FETCH(is, m_end_time);

    return err;
}

int32_t YangSdp::parse_attribute(const std::string& content)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc4566#section-5.13
    // a=<attribute>
    // a=<attribute>:<value>

    std::string attribute = "";
    std::string value = "";
    size_t pos = content.find_first_of(":");

    if (pos != std::string::npos) {
        attribute = content.substr(0, pos);
        value = content.substr(pos + 1);
    }

    if (attribute == "group") {
        return parse_attr_group(value);
    } else if (attribute == "msid-semantic") {
        std::istringstream is(value);
        FETCH(is, m_msid_semantic);

        std::string msid;
        while (is >> msid) {
            m_msids.push_back(msid);
        }
    } else {
        return m_session_info.parse_attribute(attribute, value);
    }

    return err;
}

int32_t YangSdp::parse_attr_group(const std::string& value)
{
    int32_t err = Yang_Ok;
    // @see: https://tools.ietf.org/html/rfc5888#section-5

    std::istringstream is(value);

    FETCH(is, m_group_policy);

    std::string word;
    while (is >> word) {
        m_groups.push_back(word);
    }

    return err;
}

int32_t YangSdp::parse_media_description(const std::string& content)
{
    int32_t err = Yang_Ok;

    // @see: https://tools.ietf.org/html/rfc4566#section-5.14
    // m=<media> <port> <proto> <fmt> ...
    // m=<media> <port>/<number of ports> <proto> <fmt> ...
    std::istringstream is(content);

    std::string media;
    FETCH(is, media);

    int32_t port;
    FETCH(is, port);

    std::string proto;
    FETCH(is, proto);

    m_media_descs.push_back(YangMediaDesc(media));
    m_media_descs.back().m_protos = proto;
    m_media_descs.back().m_port = port;

    int32_t fmt;
    while (is >> fmt) {
        m_media_descs.back().m_payload_types.push_back(YangMediaPayloadType(fmt));
    }

    if (!m_in_media_session) {
        m_in_media_session = true;
    }

    return err;
}

bool YangSdp::is_unified() const
{
    // TODO: FIXME: Maybe we should consider other situations.
    return m_media_descs.size() > 2;
}

int32_t YangSdp::update_msid(string id)
{
    int32_t err = Yang_Ok;

    m_msids.clear();
    m_msids.push_back(id);

    for (vector<YangMediaDesc>::iterator it = m_media_descs.begin(); it != m_media_descs.end(); ++it) {
        YangMediaDesc& desc = *it;

        if ((err = desc.update_msid(id)) != Yang_Ok) {
            printf("desc %s update msid %s", desc.m_mid.c_str(), id.c_str());
            return err;
        }
    }

    return err;
}






YangCodecPayload::YangCodecPayload()
{
    m_pt_of_publisher = m_pt = 0;
    m_sample = 0;
}










