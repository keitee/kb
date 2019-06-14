#include <QCoreApplication>
#include <QList>
#include <QString>
// #include <QMessageLogger>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcEditor, "qtc.editor");

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  // print QList coll
  {
    QList<QString> coll;
    coll.prepend("one");
    coll.prepend("two");
    coll.prepend("Three");
    qDebug() << coll;
  }

  // print outputs
  {
    // ctor logging
    qDebug("Hello qDebug");
    qWarning("Hello qWarning");
    qCritical("Hello qCritical");

    // this causes abort()
    // qFatal("Hello qFatal");

    // stream logging
    qDebug() << "Hello qDebug";
    qWarning() << "Hello qWarning";
    qCritical() << "Hello qCritical";
  }

  // printf() style logging
  {
    QString message{"Hello stream logging"};
    qDebug("QString: %s", message.toUtf8().constData());
  }

  // QTextStream manipulators
  // https://doc.qt.io/qt-5/qtextstream.html
  {
    qDebug().nospace() << "RGB: " << hex << uppercasedigits 
      << 0xff << 0x33 << 0x33;
  }

  // Categorized logging
  //
  // #define Q_DECLARE_LOGGING_CATEGORY(name) \
  //    extern const QLoggingCategory &name();
  //
  // #define Q_LOGGING_CATEGORY(name, ...) \
  //    const QLoggingCategory &name() \
  //    { \
  //        static const QLoggingCategory category(__VA_ARGS__); \
  //        return category; \
  //    }
  {
    // QLoggingCategory lcEditor("qtc.editor");
    // Q_LOGGING_CATEGORY(lcEditor, "qtc.editor");

    qCDebug(lcEditor) << "Hello debug category logging";
    qCWarning(lcEditor) << "Hello warning category logging";
    qCCritical(lcEditor) << "Hello critical category logging";
    qCDebug(lcEditor, "%s", "Hello debug category logging");
  }

  return a.exec();
}
