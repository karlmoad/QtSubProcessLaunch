//
// Created by Karl Moad on 6/5/20.
//

#ifndef LAUNCHERAPP_HTTPREQUEST_H
#define LAUNCHERAPP_HTTPREQUEST_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class HttpRequest: public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QString uri, QObject *parent=nullptr);
    virtual ~HttpRequest();

    void SetHeader(QString header, QString value);
    void SetRequestBody(const QByteArray *const value);

    void Get();
    void Post();

    int GetStatusCode();


signals:
    void responseReady();
    void errorOccured();

public slots:


private:
    QNetworkRequest _req;
    QNetworkReply   *_reply;
    QByteArray      *_body;

};


#endif //LAUNCHERAPP_HTTPREQUEST_H
