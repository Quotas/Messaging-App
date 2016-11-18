#include "window.h"
#include <QApplication>




int main(int argc, char *argv[])
{

    //Basic QT main function - qt takes control of our main function so all we need to do is create some objects here

    QApplication a(argc, argv);
    Window w;
    w.show();

    return a.exec();
}
