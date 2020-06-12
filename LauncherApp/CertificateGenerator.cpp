//
// Created by Karl Moad on 6/12/20.
//

#include "CertificateGenerator.h"

CertificateGenerator::CertificateGenerator(): _pkey(nullptr), _x509(nullptr)
{}

CertificateGenerator::~CertificateGenerator()
{
    if(_pkey)
    {
        EVP_PKEY_free(_pkey);
    }

    if(_x509)
    {
        X509_free(_x509);
    }
}

bool CertificateGenerator::GenerateSelfSignedCertificate()
{
    qDebug() << "Generating RSA key....";
    _pkey = generate_key();
    if(!_pkey)
        return false;

    qDebug() << "Generating X.509 cert...";
    _x509 = generate_x509(_pkey);
    if(!_x509)
        return false;

    return true;
}

EVP_PKEY *CertificateGenerator::generate_key()
{
    /* Allocate memory for the EVP_PKEY structure. */
    EVP_PKEY * pkey = EVP_PKEY_new();
    if(!pkey)
    {
        qDebug() << "Unable to create EVP_PKEY structure.";
        return NULL;
    }

    /* Generate the RSA key and assign it to pkey. */
    RSA * rsa = RSA_generate_key(2048, RSA_F4, NULL, NULL);
    if(!EVP_PKEY_assign_RSA(pkey, rsa))
    {
        qDebug() << "Unable to generate 2048-bit RSA key.";
        EVP_PKEY_free(pkey);
        return NULL;
    }

    /* The key has been generated, return it. */
    return pkey;
}

X509 *CertificateGenerator::generate_x509(EVP_PKEY *pkey)
{
    /* Allocate memory for the X509 structure. */
    X509 * x509 = X509_new();
    if(!x509)
    {
        qDebug() << "Unable to create X509 structure.";
        return NULL;
    }

    /* Set the serial number. */
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    /* This certificate is valid from now until exactly one year from now. */
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);

    /* Set the public key for our certificate. */
    X509_set_pubkey(x509, pkey);

    /* We want to copy the subject name to the issuer name. */
    X509_NAME * name = X509_get_subject_name(x509);

    /* Set the country code and common name. */
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *)"CA",        -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"MyCompany", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"localhost", -1, -1, 0);

    /* Now set the issuer name. */
    X509_set_issuer_name(x509, name);

    /* Actually sign the certificate with our key. */
    if(!X509_sign(x509, pkey, EVP_sha1()))
    {
        qDebug() << "Error signing certificate.";
        X509_free(x509);
        return NULL;
    }

    return x509;
}

bool CertificateGenerator::WriteToFile(const QString &path)
{
    QString keyPath = path + QString("/key.pem");
    QString certPath = path + QString("/cert.pem");

/* Open the PEM file for writing the key to disk. */
    FILE * pkey_file = fopen(keyPath.toLocal8Bit().data(), "wb");
    if(!pkey_file)
    {
        qDebug() << "Unable to open \"key.pem\" for writing.";
        return false;
    }

    /* Write the key to disk. */
    bool ret = PEM_write_PrivateKey(pkey_file, _pkey, NULL, NULL, 0, NULL, NULL);
    fclose(pkey_file);

    if(!ret)
    {
        qDebug() << "Unable to write private key to disk.";
        return false;
    }

    /* Open the PEM file for writing the certificate to disk. */
    FILE * x509_file = fopen(certPath.toLocal8Bit().data(), "wb");
    if(!x509_file)
    {
        qDebug() << "Unable to open \"cert.pem\" for writing.";
        return false;
    }

    /* Write the certificate to disk. */
    ret = PEM_write_X509(x509_file, _x509);
    fclose(x509_file);

    if(!ret)
    {
        qDebug() << "Unable to write certificate to disk.";
        return false;
    }

    return true;
}
