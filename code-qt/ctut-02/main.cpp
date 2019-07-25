#include <QCoreApplication>
#include <QtWidgets>
#include "mainwindow.h"

/*

Since "Application -> Qt Widget Application" do not provide option to use cmake,
copied ctut-01 which was created via "Application -> Qt Console Application"

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
