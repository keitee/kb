#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>

/*

Project Type: Qt Widgets Application

Creates a Qt application for the desktop. Includes a Qt Designer-based main
window.

Preselects a desktop Qt for building the application if available.

The following code is used to display a button inside a MainWindow:

Unlike Tutorial 1, uses `MainWindow` which has window panel and add a button to
it. 

*/


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    QPushButton *button = new QPushButton("Hello Qt Tutorial 2", &w);
    button->setGeometry(20, 20, 180, 30);

    w.show();

    return a.exec();
}
