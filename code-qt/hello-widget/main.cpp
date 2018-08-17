#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPushButton button("Hello Qt Widget");

    button.show();

    return a.exec();
}
