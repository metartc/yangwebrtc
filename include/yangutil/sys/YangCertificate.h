#ifndef INCLUDE_YANGUTIL_SYS_YANGCERTIFICATE_H_
#define INCLUDE_YANGUTIL_SYS_YANGCERTIFICATE_H_
#include <stdint.h>
#include <string>
#include <string.h>
#include <usr/openssl/ssl.h>
class YangCertificate
{
private:
    std::string fingerprint;
    bool ecdsa_mode;
    X509* dtls_cert;
    EVP_PKEY* dtls_pkey;
    EC_KEY* eckey;
    YangCertificate();
    int initialize();
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
#endif /* INCLUDE_YANGUTIL_SYS_YANGCERTIFICATE_H_ */
