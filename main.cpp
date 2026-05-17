#include <QApplication>
#include <QCoreApplication>

#include "main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Notepad");
    QCoreApplication::setApplicationName("Notepad");
    MainWindow window;
    window.show();
    return app.exec();
}
