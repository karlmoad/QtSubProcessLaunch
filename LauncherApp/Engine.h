//
// Created by Karl Moad on 6/2/20.
//

#ifndef LAUNCHERAPP_ENGINE_H
#define LAUNCHERAPP_ENGINE_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QDebug>
#include <QtNetwork>

class Engine: public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    virtual ~Engine();

    void Initialize();
    void ExecuteQuery(QJsonObject const &json);
    void Kill();

signals:
    void started();
    void stopped();
    void responseAvailable(QByteArray const &response);

private slots:
    void startedHandler();
    void finishedHandler(int exitCode, QProcess::ExitStatus exitStatus);
    void stateChangedHandler(QProcess::ProcessState state);
    void errorHandler(QProcess::ProcessError error);
    void logInfoAvailable();

private:
    QProcess *_process;
    QNetworkAccessManager *_net;
    QString _AccessKey = "98765432101234567890";
    QString _path = "/Users/karl/Documents/code/exp/SinpleProcLaunch/Service/build/EchoService";
};


#endif //LAUNCHERAPP_ENGINE_H
