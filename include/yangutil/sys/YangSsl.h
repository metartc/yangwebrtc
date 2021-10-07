#ifndef INCLUDE_YANGUTIL_SYS_YANGSSL_H_
#define INCLUDE_YANGUTIL_SYS_YANGSSL_H_
#include <stdint.h>
#include <string>
#include <string.h>
#ifdef _WIN32
#include <openssl/ssl.h>
#else
#include <openssl/ssl.h>
#endif
int32_t hmac_encode(const std::string& algo, const char* key, const int& key_length,
       const char* input, const int32_t input_length, char* output, unsigned int& output_length);
std::string encode_hmac(char* hamc_buf, const int32_t hmac_buf_len);
std::string encode_fingerprint(uint32_t crc32);
class YangCertificate
{
private:
    std::string fingerprint;
    bool ecdsa_mode;
    X509* dtls_cert;
    EVP_PKEY* dtls_pkey;
    EC_KEY* eckey;
    YangCertificate();
    int32_t initialize();
    static YangCertificate* m_instance;
public:
    static YangCertificate* createCertificate();
    virtual ~YangCertificate();
public:
    // Initialize DTLS certificate.

    // dtls_cert
    X509* get_cert();
    // public key
    EVP_PKEY* get_public_key();
    // ECDSA key
    EC_KEY* get_ecdsa_key();
    // certificate fingerprint
    std::string get_fingerprint();
    // whether is ecdsa
    bool is_ecdsa();
public:

};
#endif /* INCLUDE_YANGUTIL_SYS_YANGSSL_H_ */
