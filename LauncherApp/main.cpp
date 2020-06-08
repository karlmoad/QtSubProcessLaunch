#include <QtWidgets>
#include <QApplication>
#include "MainWindow.h"


int main(int argc, char *argv[]) {

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.setActiveWindow(&w);
    int ret = a.exec();
    return ret;

}
