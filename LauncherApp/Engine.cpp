//
// Created by Karl Moad on 6/2/20.
//

#include "Engine.h"

Engine::Engine(QObject *parent): QObject(parent)
{
    _process = new QProcess(this);
    connect(this->_process, &QProcess::errorOccurred, this, &Engine::errorHandler);
    connect(this->_process, &QProcess::started, this, &Engine::startedHandler);
    connect(this->_process, &QProcess::stateChanged, this, &Engine::stateChangedHandler);
    connect(this->_process, &QProcess::readyReadStandardOutput, this, &Engine::logInfoAvailable);
    connect(this->_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                this->finishedHandler(exitCode, exitStatus);
            });
    _net = new QNetworkAccessManager();
    QObject::connect(_net, &QNetworkAccessManager::finished,
                     this, [=](QNetworkReply *reply) {
                if (reply->error()) {
                    qDebug() << reply->errorString();
                    return;
                }
                responseAvailable(reply->readAll());
            }
    );
}

Engine::~Engine()
{
    this->Kill();
}

void Engine::Initialize()
{
    QStringList argz;
    argz << this->_AccessKey;

    this->_process->start(this->_path, argz);
}

void Engine::Kill()
{
    QNetworkRequest request(QUrl("http://localhost:30200/quit"));
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
    QNetworkRequest req(QUrl("http://localhost:30200/query"));
    req.setRawHeader(QString("Authorization").toUtf8(),_AccessKey.toUtf8());
    _net->post(req,QJsonDocument(json).toJson());
}
