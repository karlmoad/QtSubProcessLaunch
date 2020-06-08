//
// Created by Karl Moad on 6/5/20.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest(QString uri, QObject *parent): QObject(parent)
{
    this->_req = QNetworkRequest(QUrl(uri));
}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::SetHeader(QString header, QString value)
{
    this->_req.setRawHeader(header.toUtf8(), value.toUtf8());
}

void HttpRequest::SetRequestBody(const QByteArray *const value)
{

}

void HttpRequest::Get()
{

}

void HttpRequest::Post()
{

}

int HttpRequest::GetStatusCode()
{
    return 0;
}

void HttpRequest::responseReady()
{

}

void HttpRequest::errorOccured()
{

}
