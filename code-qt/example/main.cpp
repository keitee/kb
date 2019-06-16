#include <QCoreApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QLoggingCategory cat("qtc.editor.3", QtInfoMsg);
    cat().categoryName();

    return a.exec();
}
