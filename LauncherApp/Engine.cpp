//
// Created by Karl Moad on 6/2/20.
//

#include "Engine.h"

Engine::Engine(QObject *parent): QObject(parent)
{
    //_process = new QProcess();
    //connect(this->_process, &QProcess::errorOccurred, this, &Engine::errorHandler);
    //connect(this->_process, &QProcess::started, this, &Engine::startedHandler);
    //connect(this->_process, &QProcess::stateChanged, this, &Engine::stateChangedHandler);
    //connect(this->_process, &QProcess::readyReadStandardOutput, this, &Engine::logInfoAvailable);
    /*connect(this->_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                this->finishedHandler(exitCode, exitStatus);
            });
            */
    _net = new QNetworkAccessManager();
    connect(_net, &QNetworkAccessManager::finished,
                     this, [=](QNetworkReply *reply) {
                if (reply->error()) {
                    qDebug() << reply->errorString();
                    return;
                }
                responseAvailable(reply->readAll());
            }
    );
    OPENSSL_config(NULL);
}

Engine::~Engine()
{
    //this->Kill();
    //delete _process;

    EVP_cleanup();
    ERR_free_strings();
}

void Engine::Initialize()
{
    QStringList argz;
    argz << this->_AccessKey;
    //this->_process->start(exePath, argz);
}

void Engine::Kill()
{
    QNetworkRequest request(QUrl("https://localhost:30200/quit"));
    //request.setSslConfiguration(this->_sslConfig);
    request.setRawHeader(QString("Authorization").toUtf8(),_AccessKey.toUtf8());
    _net->get(request);
}

void Engine::startedHandler()
{
    qDebug() << "The process has started";
    emit started();
}

void Engine::finishedHandler(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Finished: exit code: " << exitCode << " status: " << exitStatus;
    emit stopped();
}

void Engine::stateChangedHandler(QProcess::ProcessState state)
{
    qDebug() << "process state change:" << state;
}

void Engine::errorHandler(QProcess::ProcessError error)
{
    qDebug() << "An error has occurred:" << error;
}

void Engine::logInfoAvailable()
{
    qDebug() << this->_process->readAllStandardOutput();
}

void Engine::ExecuteQuery(const QJsonObject &json)
{
    //QNetworkRequest req(QUrl("https://localhost:30200/query"));
    //req.setRawHeader(QString("Authorization").toUtf8(),_AccessKey.toUtf8());
    //_net->post(req,QJsonDocument(json).toJson());





}

QByteArray Engine::encryptAES(QByteArray passphrase, QByteArray &data)
{
    QByteArray msalt = randomBytes(SALTSIZE);
    int rounds = 1;
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];

    const unsigned char* salt = (const unsigned char*) msalt.constData();
    const unsigned char* password = (const unsigned char*) passphrase.constData();

    //int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt,password, passphrase.length(),rounds,key,iv);
    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt,password, passphrase.length(),rounds,key,iv);

    if(i != KEYSIZE)
    {
        qCritical() << "EVP_BytesToKey() error: " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    EVP_CIPHER_CTX en;
    EVP_CIPHER_CTX_init(&en);

    if(!EVP_EncryptInit_ex(&en, EVP_aes_256_cbc(),NULL,key, iv))
    {
        qCritical() << "EVP_EncryptInit_ex() failed " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    char *input = data.data();
    int len = data.size();

    int c_len = len + AES_BLOCK_SIZE, f_len = 0;
    unsigned char *ciphertext = (unsigned char*)malloc(c_len);

    if(!EVP_EncryptInit_ex(&en, NULL, NULL, NULL, NULL))
    {
        qCritical() << "EVP_EncryptInit_ex() failed " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    // May have to repeat this for large files

    if(!EVP_EncryptUpdate(&en, ciphertext, &c_len,(unsigned char *)input, len))
    {
        qCritical() << "EVP_EncryptUpdate() failed " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    if(!EVP_EncryptFinal(&en, ciphertext+c_len, &f_len))
    {
        qCritical() << "EVP_EncryptFinal_ex() failed "  << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    len = c_len + f_len;
    EVP_CIPHER_CTX_cipher(&en);

    //ciphertext

    QByteArray encrypted = QByteArray(reinterpret_cast<char*>(ciphertext), len);
    QByteArray finished;
    finished.append("Salted__");
    finished.append(msalt);
    finished.append(encrypted);

    free(ciphertext);

    return finished;
}

QByteArray Engine::decryptAES(QByteArray passphrase, QByteArray &data)
{
    QByteArray msalt;
    if(QString(data.mid(0,8)) == "Salted__")
    {
        msalt = data.mid(8,8);
        data = data.mid(16);
    }
    else
    {
        qWarning() << "Could not load salt from data!";
        msalt = randomBytes(SALTSIZE);
    }

    int rounds = 1;
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];
    const unsigned char* salt = (const unsigned char*)msalt.constData();
    const unsigned char* password = (const unsigned char*)passphrase.data();

    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt,password, passphrase.length(),rounds,key,iv);

    if(i != KEYSIZE)
    {
        qCritical() << "EVP_BytesToKey() error: " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    EVP_CIPHER_CTX de;
    EVP_CIPHER_CTX_init(&de);

    if(!EVP_DecryptInit_ex(&de,EVP_aes_256_cbc(), NULL, key,iv ))
    {
        qCritical() << "EVP_DecryptInit_ex() failed" << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    char *input = data.data();
    int len = data.size();

    int p_len = len, f_len = 0;
    unsigned char *plaintext = (unsigned char *)malloc(p_len + AES_BLOCK_SIZE);

    //May have to do this multiple times for large data???
    if(!EVP_DecryptUpdate(&de, plaintext, &p_len, (unsigned char *)input, len))
    {
        qCritical() << "EVP_DecryptUpdate() failed " <<  ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    if(!EVP_DecryptFinal_ex(&de,plaintext+p_len,&f_len))
    {
        qCritical() << "EVP_DecryptFinal_ex() failed " <<  ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    len = p_len + f_len;

    EVP_CIPHER_CTX_cleanup(&de);


    QByteArray decrypted = QByteArray(reinterpret_cast<char*>(plaintext), len);
    free(plaintext);

    return decrypted;
}

QByteArray Engine::randomBytes(int size)
{
    unsigned char arr[size];
    RAND_bytes(arr, size);
    return QByteArray(reinterpret_cast<char*>(arr),size);
}

