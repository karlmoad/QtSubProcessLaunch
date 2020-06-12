//
// Created by Karl Moad on 6/2/20.
//

#ifndef LAUNCHERAPP_MAINWINDOW_H
#define LAUNCHERAPP_MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "Engine.h"
#include "CertificateGenerator.h"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;


private slots:
    void startHandler();
    void stopHandler();
    void queryHandler();
    void certRequestHandler();
    void responseHandler(QByteArray const &response);

private:
    Engine *engine;
    QPushButton *btnInit;
    QPushButton *btnQuery;
    QPushButton *btnStop;
    QPushButton *btnCert;
};


#endif //LAUNCHERAPP_MAINWINDOW_H
