#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Dark palette
    QPalette pal = app.palette();
    pal.setColor(QPalette::Window,      QColor(0x1a,0x1a,0x2e));
    pal.setColor(QPalette::WindowText,  Qt::white);
    pal.setColor(QPalette::Base,        QColor(0x12,0x12,0x2a));
    pal.setColor(QPalette::AlternateBase, QColor(0x20,0x20,0x3e));
    pal.setColor(QPalette::Text,        Qt::white);
    pal.setColor(QPalette::Button,      QColor(0x22,0x22,0x44));
    pal.setColor(QPalette::ButtonText,  Qt::white);
    app.setPalette(pal);

    app.setStyle("Fusion");

    MainWindow w;
    w.show();
    return app.exec();
}
