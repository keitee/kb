#include <iostream>
#include <sstream>
#include <memory>
#include <exception>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QList>
#include <QString>
#include <QDebug>
#include <QLoggingCategory>
#include <QApplication>

#include "qt_class.h"

using namespace std;


// ={=========================================================================
// qt-console app
//
// main()
// {
//   // QCoreApplication a(argc, argv); 
//   // your code is here
//   // return a.exec();
// }
//
// have to comment out lines for QCoreApplication


// ={=========================================================================
// qt-string
//
// The QString class provides a Unicode character string.
//
// QString stores a string of 16-bit QChars, where each QChar corresponds to one
// UTF-16 code unit. (Unicode characters with code values above 65535 are stored
// using surrogate pairs, i.e., two consecutive QChars.)
//
// In addition to QString, Qt also provides the "QByteArray" class to store raw
// bytes and traditional 8-bit '\0'-terminated strings. For most purposes,
// QString is the class you want to use. It is used throughout the Qt API, and
// the Unicode support ensures that your applications will be easy to translate
// if you want to expand your application's market at some point. The two main
// cases where QByteArray is appropriate are when you need to store raw binary
// data, and when memory conservation is critical (like in embedded systems).


// Converting Between 8-Bit Strings and Unicode Strings
//
// QString provides the following three functions that return a const char *
// version of the string as QByteArray: toUtf8(), toLatin1(), and toLocal8Bit().
//
// toLatin1() returns a Latin-1 (ISO 8859-1) encoded 8-bit string.
//
// toUtf8() returns a UTF-8 encoded 8-bit string. UTF-8 is a superset of
// US-ASCII (ANSI X3.4-1986) that supports the entire Unicode character set
// through multibyte sequences.
//
// toLocal8Bit() returns an 8-bit string using the system's local encoding.
//
// To convert from one of these encodings, QString provides fromLatin1(),
// fromUtf8(), and fromLocal8Bit(). Other encodings are supported through the
// QTextCodec class.
//
// As mentioned above, QString provides a lot of functions and operators that
// make it easy to interoperate with const char * strings. But this
// functionality is a double-edged sword: It makes QString more convenient to
// use if all strings are US-ASCII or Latin-1, but there is always the risk that
// an implicit conversion from or to const char * is done using the wrong 8-bit
// encoding. To minimize these risks, you can turn off these implicit
// conversions by defining the following two preprocessor symbols:
//
// QT_NO_CAST_FROM_ASCII disables automatic conversions from C string literals
// and pointers to Unicode.
//
// QT_RESTRICTED_CAST_FROM_ASCII allows automatic conversions from C characters
// and character arrays, but disables automatic conversions from character
// pointers to Unicode.
//
// QT_NO_CAST_TO_ASCII disables automatic conversion from QString to C strings.
//
// One way to define these preprocessor symbols globally for your application is
// to add the following entry to your qmake project file:
// 
// DEFINES += QT_NO_CAST_FROM_ASCII \ 
//  QT_NO_CAST_TO_ASCII
//
// You then need to explicitly call fromUtf8(), fromLatin1(), or fromLocal8Bit()
// to construct a QString from an 8-bit string, or use the lightweight
// QLatin1String class, for example:


// Distinction Between Null and Empty Strings
//
// For historical reasons, QString distinguishes between a null string and an
// empty string. A null string is a string that is initialized using QString's
// default constructor or by passing (const char *)0 to the constructor. An
// empty string is any string with size 0. A null string is always empty, but an
// empty string isn't necessarily null:
//
// QString().isNull();               // returns true
// QString().isEmpty();              // returns true
//
// QString("").isNull();             // returns false
// QString("").isEmpty();            // returns true
//
// QString("abc").isNull();          // returns false
// QString("abc").isEmpty();         // returns false
//
// All functions except isNull() treat null strings the same as empty strings.
// For example, toUtf8().constData() returns a pointer to a '\0' character for a
// null string (not a null pointer), and QString() compares equal to
// QString(""). 
//
// We recommend that you always use the isEmpty() function and avoid isNull().

TEST(Qt, String)
{
  // You can also pass string literals to functions that take QStrings as
  // arguments, invoking the QString(const char *) constructor. Similarly, you
  // can pass a QString to a function that takes a const char * argument using
  // the qPrintable() macro which returns the given QString as a const char *.
  // This is equivalent to calling <QString>.toLocal8Bit().constData().

  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    os << message.toUtf8().constData();
    EXPECT_THAT(os.str(), "Hello stream logging");
  }

  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    // const char *qPrintable(const QString &str)
    os << qPrintable(message);
    EXPECT_THAT(os.str(), "Hello stream logging");
  }

  // First, arg(i) replaces %1. Then arg(total) replaces %2. Finally,
  // arg(fileName) replaces %3.
  //
  // One advantage of using arg() over asprintf() is that the order of the
  // numbered place markers can change, if the application's strings are
  // translated into other languages, but each arg() will still replace the
  // lowest numbered unreplaced place marker, no matter where it appears. Also,
  // if place marker %i appears more than once in the string, the arg() replaces
  // all of them.
  //
  // QString QString::asprintf(const char *cformat, ...)
  //
  // Warning: We do not recommend using QString::asprintf() in new Qt code.
  // Instead, consider using QTextStream or arg(), both of which support Unicode
  // strings seamlessly and are type-safe. Here's an example that uses
  // QTextStream:

  {
    QString i{"3"};              // current file's number
    QString total{"10"};         // number of files to process
    QString fileName{"readme"};  // current file's name

    QString status = QString("Processing file %1 of %2: %3")
      .arg(i).arg(total).arg(fileName);

    std::ostringstream os;
    os << qPrintable(status);
    EXPECT_THAT(os.str(), "Processing file 3 of 10: readme");
  }

  {
    quint8 i{3};
    quint8 total{10};
    QString fileName{"readme"};

    // error caused since asprintf() comes from stdio::asprintf()
    //
    // qt_gtest.cpp:169:81: error: cannot convert ‘const char*’ 
    //  to ‘char**’ for argument ‘1’ to ‘int asprintf(char**, const char*, ...)’
    //     QString status = asprintf("Processing file %d of %d: %s", i, total, fileName);
    // QString status = asprintf("Processing file %d of %d: %s", i, total, fileName);
 
    QByteArray format{"Processing file %d of %d: %s"};
    QString status = QString::asprintf(format.constData(), i, total, qPrintable(fileName));

    // QString status = QString::asprintf("Processing file %d of %d: %s", i, total, qPrintable(fileName));

    std::ostringstream os;
    os << qPrintable(status);
    EXPECT_THAT(os.str(), "Processing file 3 of 10: readme");
  }
}


// To obtain a pointer to the actual character data, call data() or constData().
// These functions return a pointer to the beginning of the data. The pointer is
// guaranteed to remain valid until a non-const function is called on the
// QByteArray. It is also guaranteed that the data ends with a '\0' byte unless
// the QByteArray was created from a raw data. This '\0' byte is automatically
// provided by QByteArray and is not counted in size().

// [ RUN      ] Qt.StringByteArray
// QByteArray: U%03hhu SkyQ EC201
// [       OK ] Qt.StringByteArray (0 ms)

TEST(Qt, StringByteArray)
{
  QByteArray coll{"U%03hhu SkyQ EC201"};

  // qDebug("QByteArray: %s", coll.data());
  // std::cout << "QByteArray: " << coll.data() << std::endl;

  std::ostringstream os;
  os << coll.data();
  EXPECT_THAT(os.str(), "U%03hhu SkyQ EC201");
}


// ={=========================================================================
// qt-list

TEST(Qt, ListPrepend)
{
  {
    QList<QString> coll;

    coll.prepend("one");
    coll.prepend("two");
    coll.prepend("three");

    // works
    // qDebug() << coll;

    std::ostringstream os;
    for (const auto &e : coll)
      os << qPrintable(e) << ", ";
    EXPECT_THAT(os.str(), "three, two, one, ");
  }
}

/*

Qt Logging Framework - KDAB

http://www.kdab.com/wp-content/uploads/stories/slides/Day2/KaiKoehne_Qt%20Logging%20Framework%2016_9_0.pdf

Logs message, message type, file, line, function

Macros since Qt 5.0:

#define qDebug \
 QMessageLogger(__FILE__, __LINE__, \
 Q_FUNC_INFO).debug

qFatal aborts

The QMessageLogger class generates log messages.

QMessageLogger is used to generate messages for the Qt logging framework.
Usually one uses it through qDebug(), qInfo(), qWarning(), qCritical, or
qFatal() functions, which are actually macros: For example qDebug() expands to
QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO).debug() for debug builds, and
QMessageLogger(0, 0, 0).debug() for release builds.

*/

TEST(Qt, Logging)
{
  // two ways of logging
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
}


/*

Categorized logging (Qt 5.2)

Split up logging messages in hierarchical categories.

Category is identified by it's name
 category.subcategory.subsubcategory[...]

Logging of messages can be enabled or disabled based on the category and
message type, at runtime.

#define Q_DECLARE_LOGGING_CATEGORY(name) \
   extern const QLoggingCategory &name();

#define Q_LOGGING_CATEGORY(name, ...) \
   const QLoggingCategory &name() \
   { \
       static const QLoggingCategory category(__VA_ARGS__); \
       return category; \
   }

Note: The qCDebug(), qCWarning(), qCCritical() macros prevent arguments from
being evaluated if the respective message types are not enabled for the
category, so explicit checking is not needed:

    // usbEntries() will only be called if driverUsb category is enabled
    qCDebug(driverUsb) << "devices: " << usbEntries();


#if !defined(QT_NO_DEBUG_OUTPUT)
#  define qCDebug(category, ...) \
    for (bool qt_category_enabled = category().isDebugEnabled(); qt_category_enabled; qt_category_enabled = false) \
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName()).debug(__VA_ARGS__)
#else
#  define qCDebug(category, ...) QT_NO_QDEBUG_MACRO()
#endif
 
*/

Q_LOGGING_CATEGORY(lcEditor1, "qtc.editor.1");

TEST(Qt, LoggingCategory)
{
  {
    // QLoggingCategory lcEditor("qtc.editor");

    qCDebug(lcEditor1) << "1. Hello debug category logging";
    qCWarning(lcEditor1) << "1. Hello warning category logging";
    qCCritical(lcEditor1) << "1. Hello critical category logging";
    qCDebug(lcEditor1, "%s", "1. Hello debug category logging");
  }

  {
    QLoggingCategory lcEditor2("qtc.editor.2");

    qCDebug(lcEditor2) << "2. Hello debug category logging";
    qCWarning(lcEditor2) << "2. Hello warning category logging";
    qCCritical(lcEditor2) << "2. Hello critical category logging";
    qCDebug(lcEditor2, "%s", "2. Hello debug category logging");
  }

  // QLoggingCategory(const char *, QtMsgType severityLevel) (Qt 5.4)
  //
  // Disables message types < severityLevel. ???
  //
  // enum QtMsgType
  //
  // This enum describes the messages that can be sent to a message handler
  // (QtMessageHandler). You can use the enum to identify and associate the
  // various message types with the appropriate actions.
  // 
  // Constant	Value	Description
  // QtDebugMsg	0	A message generated by the qDebug() function.
  // QtInfoMsg	4	A message generated by the qInfo() function.
  // QtWarningMsg	1	A message generated by the qWarning() function.
  // QtCriticalMsg	2	A message generated by the qCritical() function.
  // QtFatalMsg	3	A message generated by the qFatal() function.
  // QtSystemMsg	QtCriticalMsg	 
  // QtInfoMsg was added in Qt 5.5.
  // enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg };


  // qtc.editor.3: 3. Hello warning category logging
  // qtc.editor.3: 3. Hello critical category logging
  // qtc.editor.3: 3. Hello info category logging
  // qtc.editor.3 isCriticalEnabled
  // qtc.editor.3 isCriticalEnabled
  // qtc.editor.3 isInfoEnabled
  // qtc.editor.3 isWarningEnabled

  {
    // all but debug. why not see DebugMsg?
    QLoggingCategory cat("qtc.editor.3", QtInfoMsg);

    qCDebug(cat) << "3. Hello debug category logging";
    qCWarning(cat) << "3. Hello warning category logging";
    qCCritical(cat) << "3. Hello critical category logging";
    qCInfo(cat) << "3. Hello info category logging";
    qCDebug(cat, "%s", "3. Hello debug category logging");

    // from qCX macros, how is this possible?
    //
    // for (bool qt_category_enabled = cat().isCriticalEnabled(); qt_category_enabled; qt_category_enabled = false)
    // 
    // since:
    //
    // #define QLOGGINGCATEGORY_H
    //
    // // allows usage of both factory method and variable in qCX macros
    // QLoggingCategory &operator()() { return *this; }
    // const QLoggingCategory &operator()() const { return *this; }

    for (bool qt_category_enabled = cat.isCriticalEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat.categoryName() << " isCriticalEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isCriticalEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isCriticalEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isDebugEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isDebugEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isInfoEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isInfoEnabled" << std::endl;

    // use of isEnabled()
    for (bool qt_category_enabled = cat().isEnabled(QtWarningMsg); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isWarningEnabled" << std::endl;
  }


  // qtc.editor.4: 4. Hello warning category logging
  // qtc.editor.4: 4. Hello critical category logging
  // qtc.editor.4 isCriticalEnabled
  // qtc.editor.4 isWarningEnabled

  {
    // all but debug. why not see DebugMsg?
    QLoggingCategory cat("qtc.editor.4", QtWarningMsg);

    qCDebug(cat) << "4. Hello debug category logging";
    qCWarning(cat) << "4. Hello warning category logging";
    qCCritical(cat) << "4. Hello critical category logging";
    qCInfo(cat) << "4. Hello info category logging";
    qCDebug(cat, "%s", "4. Hello debug category logging");

    for (bool qt_category_enabled = cat().isCriticalEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isCriticalEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isDebugEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isDebugEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isInfoEnabled(); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isInfoEnabled" << std::endl;

    // use of isEnabled()
    for (bool qt_category_enabled = cat().isEnabled(QtWarningMsg); qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isWarningEnabled" << std::endl;
  }
}


// ={=========================================================================
// qt-regexp

TEST(Qt, RegExp)
{
  quint8 code{130};
  QByteArray pattern{"U%03hhu SkyQ EC201"};
  QString value = QString::asprintf(pattern, code);
  QRegExp regex(value);

  // void QRegExp::setPatternSyntax(QRegExp::PatternSyntax syntax)
  //
  // Sets the syntax mode for the regular expression. The default is
  // QRegExp::RegExp.
  //  
  // Setting syntax to QRegExp::Wildcard enables simple shell-like QRegExp
  // wildcard matching. For example, r*.txt matches the string readme.txt in
  // wildcard mode, but does not match readme.

  // enum QRegExp::PatternSyntax
  //
  // The syntax used to interpret the meaning of the pattern.
  // 
  // Constant	Value	Description
  //
  // QRegExp::RegExp	0	A rich Perl-like pattern matching syntax. This is the
  // *default*
  //
  // QRegExp::RegExp2	3	Like RegExp, but with greedy quantifiers. (Introduced in
  // Qt 4.2.)
  //
  // QRegExp::Wildcard	1	This provides a simple pattern matching syntax similar
  // to that used by shells (command interpreters) for "file globbing". See
  // QRegExp wildcard matching.
  //
  // QRegExp::WildcardUnix	4	This is similar to Wildcard but with the behavior
  // of a Unix shell. The wildcard characters can be escaped with the character
  // "\".

  regex.setPatternSyntax(QRegExp::WildcardUnix);
  std::ostringstream os;
  os << qPrintable(regex.pattern());
  EXPECT_THAT(os.str(), "U130 SkyQ EC201");

  // bool QRegExp::exactMatch(const QString &str) const
  //
  // Returns true if str is matched exactly by this regular expression;
  // otherwise returns false. You can determine how much of the string was
  // matched by calling matchedLength().

  EXPECT_THAT(regex.exactMatch("U130 SkyQ EC201"), true);
  EXPECT_THAT(regex.exactMatch("U131 SkyQ EC201"), false);
}


// ={=========================================================================
// qt-slot

TEST(Qt, SlotAndSignal)
{
  // Calling a.setValue(12) makes a emit a valueChanged(12) signal, which b will
  // receive in its setValue() slot, i.e. b.setValue(12) is called. Then b emits
  // the same valueChanged() signal, but since no slot has been connected to b's
  // valueChanged() signal, the signal is ignored.
  //
  // Note that the setValue() function sets the value and emits the signal only
  // if value != m_value. This prevents infinite looping in the case of cyclic
  // connections (e.g., if b.valueChanged() were connected to a.setValue()).

  {
    Counter a, b;
    QObject::connect(&a, &Counter::valueChanged,
        &b, &Counter::setValue);

    a.setValue(12);     // a.value() == 12, b.value() == 12
    EXPECT_THAT(a.value(), 12);
    EXPECT_THAT(b.value(), 12);

    b.setValue(48);     // a.value() == 12, b.value() == 48
    EXPECT_THAT(a.value(), 12);
    EXPECT_THAT(b.value(), 48);
  }

  // to show signal/slot is blocking call
  {
    Counter a, b;
    QObject::connect(&a, &Counter::valueChanged,
        &b, &Counter::doSomethingLong);

    a.valueChanged(12);
    qCritical() << "Counter finished";
  }
}


// ={=========================================================================
// qt-thread
/*

A QThread object manages one thread of control within the program. QThreads
begin executing in run(). By default, run() starts the `event loop` by calling
exec() and runs a Qt event loop inside the thread.

Static Public Members

void	msleep(unsigned long msecs)
void	sleep(unsigned long secs)
void	usleep(unsigned long usecs)

*/

TEST(Qt, Thread)
{
  // https://wiki.qt.io/QThreads_general_usage
  {
    using namespace qt_thread_1;

    QThread* thread = new QThread;
    Worker* worker = new Worker();
    worker->moveToThread(thread);
    // QObject::connect(worker, SIGNAL (error(QString)), this, SLOT (errorString(QString)));
    
    // *qt-runtime-check*
    // see typo but no compile error. see error only when runs
    // QObject::connect: No such slot qt_thread_1::Worker::process()
    // QObject::connect(thread, SIGNAL (started()), worker, SLOT (process()));

    QObject::connect(thread, &QThread::started, worker, &Worker::progress);

    // again, no compile error but when thread finishes and emit signal, nothing
    // happens and blocks forever. no quit() gets run.
    //
    // QObject::connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
    //
    // need to make a link to QApplication and also need to run QApplication and
    // exec in the main.

    QObject::connect(worker, SIGNAL (finished()), QApplication::instance(), SLOT (quit()));

    QObject::connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    QObject::connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
  }

  // so, the belows do not runs as QApplication exited already from the above.
  // https://doc.qt.io/qt-5/qthread.html
  {
    using namespace qt_thread_2;

    Controller co;

    qCritical() << "before sending operate..";
    emit co.operate(QString("send operate"));
    qCritical() << "after sending operate..";
  }
}


// ={=========================================================================

int main(int argc, char **argv)
{

  QCoreApplication a(argc, argv);

  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  RUN_ALL_TESTS();

  // return RUN_ALL_TESTS();

  return a.exec();
}
