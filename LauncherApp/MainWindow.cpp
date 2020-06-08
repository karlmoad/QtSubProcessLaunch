//
// Created by Karl Moad on 6/2/20.
//

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    this->setMinimumSize(900, 768);
    this->setWindowTitle("Launch The App");
    engine = new Engine(this);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    this->btnInit = new QPushButton("Start");
    this->btnQuery = new QPushButton("Query");
    this->btnStop = new QPushButton("Stop");
    layout->addWidget(btnInit);
    layout->addWidget(btnQuery);
    layout->addWidget(btnStop);

    connect(this->btnInit, &QPushButton::clicked, this, &MainWindow::startHandler);
    connect(this->btnStop, &QPushButton::clicked, this, &MainWindow::stopHandler);
    connect(this->btnQuery, &QPushButton::clicked, this, &MainWindow::queryHandler);
    connect(this->engine, &Engine::responseAvailable, this, &MainWindow::responseHandler);

    this->setCentralWidget(widget);
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    event->accept();
}

void MainWindow::startHandler()
{
    this->engine->Initialize();
}

void MainWindow::stopHandler()
{
    this->engine->Kill();
}

void MainWindow::queryHandler()
{
    QJsonObject json;
    json["ContextId"] = "9876543210";
    json["TableId"] = "42";
    json["Statement"] = "SELECT * FROM $42";
    json["Connection"] = "CERNER";

    engine->ExecuteQuery(json);
}

void MainWindow::responseHandler(QByteArray const &response)
{
    qDebug() << response;
}
