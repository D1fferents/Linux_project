#include "servermainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ServerMainWindow window;
    window.show();
    return app.exec();
}

