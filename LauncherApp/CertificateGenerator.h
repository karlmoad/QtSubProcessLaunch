//
// Created by Karl Moad on 6/12/20.
//

#ifndef LAUNCHERAPP_CERTIFICATEGENERATOR_H
#define LAUNCHERAPP_CERTIFICATEGENERATOR_H

#include <cstdio>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <QString>
#include <QDebug>

class CertificateGenerator
{
public:
    CertificateGenerator();
    ~CertificateGenerator();

    bool GenerateSelfSignedCertificate();
    bool WriteToFile(QString const &path);

private:

    EVP_PKEY* generate_key();
    X509 * generate_x509(EVP_PKEY * pkey);

    EVP_PKEY *_pkey;
    X509     *_x509;
};


#endif //LAUNCHERAPP_CERTIFICATEGENERATOR_H
