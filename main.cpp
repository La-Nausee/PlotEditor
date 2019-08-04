#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets/QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QString("fusion"));
    a.setApplicationName("Plot Editor 1.0");

    MainWindow w;
    w.setWindowTitle(QApplication::applicationName());

    if(!w.initGraphContext())
    {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

    w.showMaximized();

    return a.exec();
}
