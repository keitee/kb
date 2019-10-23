#include <exception>
#include <iostream>
#include <memory>
#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QDebug>
#include <QList>
#include <QLoggingCategory>
#include <QString>
// #include <QApplication>
#include <QQueue>
#include <QSignalSpy>
#include <QTime>
#include <QTimer>

#include "qclass.h"

using namespace std;
using ::testing::Ge;

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
// https://doc.qt.io/qt-5/qstring.html
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

TEST(QtString, convertToConstChar)
{
  // You can also pass string literals to functions that take QStrings as
  // arguments, invoking the QString(const char *) constructor. Similarly, you
  // can pass a QString to a function that takes a const char * argument using
  // the qPrintable() macro which returns the given QString as a const char *.
  // This is equivalent to calling <QString>.toLocal8Bit().constData().

  // toUtf8()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    os << message.toUtf8().constData();
    EXPECT_THAT(os.str(), "Hello stream logging");
  }

  // toLatin1()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    os << message.toLatin1().constData();
    EXPECT_THAT(os.str(), "Hello stream logging");
  }

  // toLocal8Bit()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    os << message.toLocal8Bit().constData();
    EXPECT_THAT(os.str(), "Hello stream logging");
  }

  // qPrintable()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    // const char *qPrintable(const QString &str)
    os << qPrintable(message);
    EXPECT_THAT(os.str(), "Hello stream logging");
  }
}

TEST(QtString, xxx)
{
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
    QString i{"3"};             // current file's number
    QString total{"10"};        // number of files to process
    QString fileName{"readme"}; // current file's name

    QString status =
      QString("Processing file %1 of %2: %3").arg(i).arg(total).arg(fileName);

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
    //     QString status = asprintf("Processing file %d of %d: %s", i, total,
    //     fileName);
    // QString status = asprintf("Processing file %d of %d: %s", i, total,
    // fileName);

    QByteArray format{"Processing file %d of %d: %s"};
    QString status =
      QString::asprintf(format.constData(), i, total, qPrintable(fileName));

    // QString status = QString::asprintf("Processing file %d of %d: %s", i,
    // total, qPrintable(fileName));

    std::ostringstream os;
    os << qPrintable(status);
    EXPECT_THAT(os.str(), "Processing file 3 of 10: readme");
  }
}

TEST(QtString, Compare)
{
  QString coll{"compare"};
  EXPECT_THAT(coll, QString("compare"));
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

TEST(QtString, ByteArray)
{
  QByteArray coll{"U%03hhu SkyQ EC201"};

  // qDebug("QByteArray: %s", coll.data());
  // std::cout << "QByteArray: " << coll.data() << std::endl;

  std::ostringstream os;
  os << coll.data();
  EXPECT_THAT(os.str(), "U%03hhu SkyQ EC201");
}

// int QString::indexOf
// (const QString &str, int from = 0, Qt::CaseSensitivity cs =
// Qt::CaseSensitive) const
//
// Returns the index position of the first occurrence of the string str in this
// string, searching forward from index position from. Returns -1 if str is not
// found.

TEST(QtString, useIndexOf)
{
  QString coll{"QtString supports indexOf method"};

  EXPECT_THAT(coll.indexOf("indexOf"), 18);

  EXPECT_THAT(coll.indexOf("indexof"), -1);
}

TEST(QtString, useStartsWith)
{
  QString coll{"value=10"};

  EXPECT_THAT(coll.startsWith("value="), true);

  EXPECT_THAT(coll.mid(6), "10");
}

// ={=========================================================================
// qt-list

TEST(QtList, Prepend)
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

// ={=========================================================================
// qt-queue

TEST(QtQueue, Pop)
{
  {
    QQueue<int> coll;

    coll.enqueue(1);
    coll.enqueue(2);
    coll.enqueue(3);

    EXPECT_THAT(coll.size(), 3);

    // T QQueue::dequeue()
    // Removes the head item in the queue and returns it. This function assumes
    // that the queue isn't empty.

    EXPECT_THAT(coll.dequeue(), 1);
    EXPECT_THAT(coll.size(), 2);
  }
}

// ={=========================================================================
/* qt-variant
https://doc.qt.io/qt-5/qvariant.html#QVariantMap-typedef

A QVariant object holds a single value of a single type() at a time. (Some
type()s are multi-valued, for example a string list.) You can find out what
type, T, the variant holds, convert it to a different type using convert(), get
its value using one of the toT() functions (e.g., toSize()) and check whether
the type can be converted to a particular type using canConvert().

*/

TEST(QtVariant, Variant)
{
  // T QVariant::value() const
  //
  // Returns the stored value "converted to the template type T". Call
  // canConvert() to find out whether a type can be converted. If the value
  // cannot be converted, a default-constructed value will be returned.
  //
  // If the type T is supported by QVariant, this function behaves exactly as
  // toString(), toInt() etc.

  {
    QVariant v;
    v = 7;
    EXPECT_THAT(7, v.toInt());

    // compile error since "7" is not QString
    // EXPECT_THAT("7", v.toString());
    EXPECT_THAT(QString("7"), v.toString());

    // const char *QVariant::typeName() const
    // Returns the name of the type stored in the variant. The returned strings
    // describe the C++ datatype used to store the data: for example, "QFont",
    // "QString", or "QVariantList". An Invalid variant returns 0.
    EXPECT_THAT("int", v.typeName());

    EXPECT_THAT(v.toInt(), v.value<int>());        // same as v.toInt()
    EXPECT_THAT(v.toString(), v.value<QString>()); // same as v.toString()
  }

  // typedef QVariant::QVariantList
  // Synonym for QList<QVariant>.
  //
  // QVariant QVariant::fromValue(const T &value)
  // Returns a QVariant containing a copy of value. Behaves exactly like
  // setValue() otherwise.
  //
  // A QVariant containing a sequential container will also return true for this
  // function if the targetTypeId is QVariantList. It is possible to iterate
  // over the contents of the container without extracting it as a (copied)
  // QVariantList:
  //
  //
  // bool QVariant::canConvert() const
  // Returns true if the variant can be converted to the template type T,
  // otherwise false.
  //
  // output:
  // QVariant(int, 7)
  // QVariant(int, 11)
  // QVariant(int, 42)

  {
    QList<int> coll{7, 11, 42};
    QVariant v = QVariant::fromValue(coll);
    if (v.canConvert<QVariantList>())
    {
      QSequentialIterable it = v.value<QSequentialIterable>();

      foreach (const QVariant &v, it)
        qDebug() << v;
    }
  }
}

// ={=========================================================================
/* qt-map

https://doc.qt.io/qt-5/qmap.html

If you want to navigate through all the (key, value) pairs stored in a QMap, you
can use an iterator. QMap provides both Java-style iterators (QMapIterator and
QMutableMapIterator) and STL-style iterators (QMap::const_iterator and
QMap::iterator).

Here's how to iterate over a QMap<QString, int> using a Java-style iterator:

QMapIterator<QString, int> i(map);
while (i.hasNext()) {
    i.next();
    cout << i.key() << ": " << i.value() << endl;
}

Here's the same code, but using an STL-style iterator this time:

QMap<QString, int>::const_iterator i = map.constBegin();
while (i != map.constEnd()) {
    cout << i.key() << ": " << i.value() << endl;
    ++i;
}


https://doc.qt.io/qt-5/qmap-const-iterator.html

const Key &const_iterator::key() const
Returns the current item's key.

See also value().

const T &const_iterator::value() const
Returns the current item's value.

See also key() and operator*().

NOTE: this is different from cxx-map iterator

    auto it = coll.find(3.0);
    EXPECT_THAT(it->first, 3);
    EXPECT_THAT(it->second, 2);

*/

namespace qt_map
{
  struct State
  {
    int parentState;
    int initialState;
    QString name;
  };
} // namespace qt_map

TEST(QtMap, Access)
{
  {
    using namespace qt_map;

    State state1{100, 101, "one"};
    State state2{200, 201, "two"};
    State state3{300, 301, "thr"};

    QMap<int, State> coll;

    coll.insert(1, state1);
    coll.insert(2, state2);
    coll.insert(3, state3);

    EXPECT_THAT(coll.size(), 3);

    auto it = coll.find(2);

    if (it == coll.end())
      EXPECT_THAT(true, false);

    // *cxx-error* However, this compiles okay but fails when runs. So gtest
    // issue as checking equal works ?
    // EXPECT_EQ(it, coll.end());

    // EXPECT_THAT(it.key(), 2);
    if (it.key() != 2)
      EXPECT_THAT(true, false);

    if (it.value().initialState != 201)
      EXPECT_THAT(true, false);

    EXPECT_THAT(it->initialState, 201);

    // this is interesting since it->initialState works
    EXPECT_THAT(it->initialState, it.value().initialState);
  }
}

/*
typedef QVariant::QVariantMap
Synonym for QMap<QString, QVariant>.
*/

TEST(QtMap, VariantMap)
{
  QVariantMap coll;

  // TODO: HOW TO INSERT since {} not supported?
  // coll.insert({QString("string0"), QVariant(0)});
  // coll.insert({QString("string1"), "how's the day"});
  // coll.insert({QString("string2"), 3.2});
  // coll.insert({QString("string3"), 10});
}

// ={=========================================================================
// qt-logging

/*
https://doc.qt.io/qt-5/debug.html

Warning and Debugging Messages

Qt includes global macros for writing out warning and debug text. You can use
them for the following purposes:

qDebug() is used for writing custom debug output.
qInfo() is used for informational messages.
qWarning() is used to report warnings and recoverable errors in your
application. qCritical() is used for writing critical error messages and
reporting system errors. qFatal() is used for writing fatal error messages
shortly before exiting.


*controlled by compilation flag*

qDebug(), qInfo(), and qWarning() are debugging tools. They can be compiled away
by defining QT_NO_DEBUG_OUTPUT, QT_NO_INFO_OUTPUT, or QT_NO_WARNING_OUTPUT
during compilation.


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

TEST(QtLogging, Logging)
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
    qDebug().nospace() << "RGB: " << hex << uppercasedigits << 0xff << 0x33
                       << 0x33;
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
    for (bool qt_category_enabled = category().isDebugEnabled();
qt_category_enabled; qt_category_enabled = false) \
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE,
QT_MESSAGELOG_FUNC, category().categoryName()).debug(__VA_ARGS__) #else # define
qCDebug(category, ...) QT_NO_QDEBUG_MACRO() #endif

*/

Q_LOGGING_CATEGORY(lcEditor1, "qtc.editor.1");

TEST(QtLogging, Category)
{
  {
    // QLoggingCategory lcEditor("qtc.editor");
    // can use macro as above

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
  // Both the QLoggingCategory constructor and the Q_LOGGING_CATEGORY() macro
  // accept an optional QtMsgType argument, which disables all message types
  // with a *lower severity*.
  //
  // Q_LOGGING_CATEGORY(driverUsbEvents, "driver.usb.events", QtWarningMsg)
  //
  // logs messages of type QtWarningMsg, QtCriticalMsg, QtFatalMsg,
  // but ignores messages of type QtDebugMsg and QtInfoMsg.
  // (not use enum value?)
  //
  // If no argument is passed, all messages are logged.
  //
  // enum QtMsgType
  //
  // This enum describes the messages that can be sent to a message handler
  // (QtMessageHandler). You can use the enum to identify and associate the
  // various message types with the appropriate actions.
  //
  // Constant	Value	Description
  // QtDebugMsg     0   A message generated by the qDebug() function.
  // QtInfoMsg      4   A message generated by the qInfo() function.
  // QtWarningMsg   1   A message generated by the qWarning() function.
  // QtCriticalMsg  2   A message generated by the qCritical() function.
  // QtFatalMsg     3   A message generated by the qFatal() function.
  // QtSystemMsg
  //
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
    // for (bool qt_category_enabled = cat().isCriticalEnabled();
    // qt_category_enabled; qt_category_enabled = false)
    //
    // since:
    //
    // #define QLOGGINGCATEGORY_H
    //
    // // allows usage of both factory method and variable in qCX macros
    // QLoggingCategory &operator()() { return *this; }
    // const QLoggingCategory &operator()() const { return *this; }

    for (bool qt_category_enabled                 = cat.isCriticalEnabled();
         qt_category_enabled; qt_category_enabled = false)
      std::cout << cat.categoryName() << " isCriticalEnabled" << std::endl;

    for (bool qt_category_enabled                 = cat().isCriticalEnabled();
         qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isCriticalEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isDebugEnabled(); qt_category_enabled;
         qt_category_enabled      = false)
      std::cout << cat().categoryName() << " isDebugEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isInfoEnabled(); qt_category_enabled;
         qt_category_enabled      = false)
      std::cout << cat().categoryName() << " isInfoEnabled" << std::endl;

    // use of isEnabled()
    for (bool qt_category_enabled = cat().isEnabled(QtWarningMsg);
         qt_category_enabled; qt_category_enabled = false)
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

    for (bool qt_category_enabled                 = cat().isCriticalEnabled();
         qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isCriticalEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isDebugEnabled(); qt_category_enabled;
         qt_category_enabled      = false)
      std::cout << cat().categoryName() << " isDebugEnabled" << std::endl;

    for (bool qt_category_enabled = cat().isInfoEnabled(); qt_category_enabled;
         qt_category_enabled      = false)
      std::cout << cat().categoryName() << " isInfoEnabled" << std::endl;

    // use of isEnabled()
    for (bool qt_category_enabled = cat().isEnabled(QtWarningMsg);
         qt_category_enabled; qt_category_enabled = false)
      std::cout << cat().categoryName() << " isWarningEnabled" << std::endl;
  }
}

/*
https://doc.qt.io/qt-5/qtglobal.html#qInstallMessageHandler

QtMessageHandler <QtGlobal>::qInstallMessageHandler(QtMessageHandler handler)

Installs a Qt message handler which has been defined previously. Returns a
pointer to the previous message handler.

The message handler is a function that *prints out* debug messages, warnings,
critical and fatal error messages. The Qt library (debug mode) contains hundreds
of warning messages that are printed when internal errors (usually invalid
function arguments) occur. Qt built in release mode also contains such warnings
unless QT_NO_WARNING_OUTPUT and/or QT_NO_DEBUG_OUTPUT have been set during
compilation. If you implement your own message handler, you get total control of
these messages.

The default message handler prints the message to the standard output under X11
or to the debugger under Windows. If it is a fatal message, the application
aborts immediately.

Only one message handler can be defined, since this is usually done on an
application-wide basis to control debug output.

To restore the message handler, call qInstallMessageHandler(0).

Example:

#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const
QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file,
context.line, function); break; case QtInfoMsg: fprintf(stderr, "Info: %s
(%s:%u, %s)\n", localMsg.constData(), file, context.line, function); break; case
QtWarningMsg: fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(),
file, context.line, function); break; case QtCriticalMsg: fprintf(stderr,
"Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line,
function); break; case QtFatalMsg: fprintf(stderr, "Fatal: %s (%s:%u, %s)\n",
localMsg.constData(), file, context.line, function); break;
    }
}

int main(int argc, char **argv)
{
    qInstallMessageHandler(myMessageOutput);
    QApplication app(argc, argv);
    ...
    return app.exec();
}

Finally, the QtMsgType definition identifies the various messages that can be
generated and sent to a Qt message handler; QtMessageHandler is a type
definition for a pointer to a function with the signature void
myMessageHandler(QtMsgType, const QMessageLogContext &, const char *).
QMessageLogContext class contains the line, file, and function the message was
logged at. This information is created by the QMessageLogger class.

*/

// ={=========================================================================
// qt-regexp

TEST(QtRegex, Regex)
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
  // QRegExp::RegExp	0	A rich Perl-like pattern matching syntax. This is
  // the *default*
  //
  // QRegExp::RegExp2	3	Like RegExp, but with greedy quantifiers.
  // (Introduced in Qt 4.2.)
  //
  // QRegExp::Wildcard	1	This provides a simple pattern matching syntax
  // similar to that used by shells (command interpreters) for "file globbing".
  // See QRegExp wildcard matching.
  //
  // QRegExp::WildcardUnix	4	This is similar to Wildcard but with the
  // behavior of a Unix shell. The wildcard characters can be escaped with the
  // character
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

TEST(QtSlot, SlotAndSignal)
{
  // Calling a.setValue(12) makes a emit a.valueChanged(12) signal, which b will
  // receive in its setValue() slot, i.e. b.setValue(12) is called. Then b emits
  // the same valueChanged() signal, but since no slot has been connected to b's
  // valueChanged() signal, the signal is ignored.
  //
  // Note that the setValue() function sets the value and emits the signal only
  // if value != m_value. This prevents infinite looping in the case of cyclic
  // connections (e.g., if b.valueChanged() were connected to a.setValue()).

  {
    Counter a, b;

    QObject::connect(&a, &Counter::valueChanged, &b, &Counter::setValue);

    EXPECT_THAT(a.value(), 0);
    EXPECT_THAT(b.value(), 0);

    a.setValue(12);
    // effectively, emit valueChanged(value);
    // a.value() == 12, b.value() == 12

    EXPECT_THAT(a.value(), 12);
    EXPECT_THAT(b.value(), 12);

    b.setValue(48);
    // effectively, emit valueChanged(value); but b is not connected so no
    // effect to a.
    // a.value() == 12, b.value() == 48

    EXPECT_THAT(a.value(), 12);
    EXPECT_THAT(b.value(), 48);
  }

  // to show signal/slot is blocking call
  // and shows connect(valueChanged(int), doSomethingLong());
  // this is receiver decide that it do not use it

  {
    Counter a, b;

    QObject::connect(&a, &Counter::valueChanged, &b, &Counter::doSomethingLong);

    // https://doc.qt.io/qt-5/qtime.html
    QTime elapsed;
    elapsed.start();

    a.valueChanged(12);

    // elapsed.elapsed() returns ms.
    EXPECT_THAT(elapsed.elapsed(), Ge(5000));
  }

  // to show `emit signal` is a function call
  {
    Counter a, b;

    QObject::connect(&a, &Counter::valueChanged, &b, &Counter::doSomethingLong);

    QTime elapsed;
    elapsed.start();

    emit a.valueChanged(12);

    // elapsed.elapsed() returns ms.
    EXPECT_THAT(elapsed.elapsed(), Ge(5000));
  }
}

// ={=========================================================================
// qt-timer

/*
https://doc.qt.io/qt-5/qtimer.html

Detailed Description

The QTimer class provides a high-level programming interface for timers. To use
it, create a QTimer, connect its timeout() signal to the appropriate slots, and
call start(). From then on, it will emit the timeout() signal
*at constant intervals.*

Example for a one second (1000 millisecond) timer (from the Analog Clock
example):

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,
      QOverload<>::of(&AnalogClock::update));
    timer->start(1000);

From then on, the update() slot is called every second.

You can set a timer to time out only once by calling setSingleShot(true).

void setSingleShot(bool singleShot)


You can also use the static QTimer::singleShot() function to call a slot after a
specified interval:

    QTimer::singleShot(200, this, SLOT(updateCaption()));

    void QTimer::singleShot(
      int msec, const QObject *receiver, const char *member)

This static function calls a slot after a given time interval.

It is very convenient to use this function because you do not need to bother
with a timerEvent or create a local QTimer object.


*qt-event-loop*
In multithreaded applications, you can use QTimer in any thread that has an
event loop. To start an event loop from a non-GUI thread, use QThread::exec().


Qt uses the timer's thread affinity to determine which thread will emit the
timeout() signal. Because of this, you must start and stop the timer in its
thread; it is not possible to start a timer from another thread.


As a special case, a QTimer with a timeout of 0 will time out as soon as all the
events in the window system's event queue have been processed. This can be used
to do heavy work while providing a snappy user interface:

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(processOneThing()));
    timer->start();

From then on, processOneThing() will be called repeatedly. It should be written
in such a way that it always returns quickly (typically after processing one
data item) so that Qt can deliver events to the user interface and stop the
timer as soon as it has done all its work. This is the traditional way of
implementing heavy work in GUI applications, but as multithreading is nowadays
becoming available on more and more platforms, we expect that zero-millisecond
QTimer objects will gradually be replaced by QThreads.

Accuracy and Timer Resolution

The accuracy of timers depends on the underlying operating system and hardware.
Most platforms support a resolution of 1 millisecond, though the accuracy of the
timer will not equal this resolution in many real-world situations.

The accuracy also depends on the timer type. For Qt::PreciseTimer, QTimer will
try to keep the accuracy at 1 millisecond. Precise timers will also never time
out earlier than expected.

For Qt::CoarseTimer and Qt::VeryCoarseTimer types, QTimer may wake up earlier
than expected, within the margins for those types: 5% of the interval for
Qt::CoarseTimer and 500 ms for Qt::VeryCoarseTimer.

All timer types may time out later than expected if the system is busy or unable
to provide the requested accuracy. In such a case of timeout overrun, Qt will
emit timeout() only once, even if multiple timeouts have expired, and then will
resume the original interval.


Alternatives to QTimer

An alternative to using QTimer is to call QObject::startTimer() for your object
and reimplement the QObject::timerEvent() event handler in your class (which
    must inherit QObject). The disadvantage is that timerEvent() does not
support such high-level features as single-shot timers or signals.

Another alternative is QBasicTimer. It is typically less cumbersome than using
QObject::startTimer() directly. See Timers for an overview of all three
approaches.

Some operating systems limit the number of timers that may be used; Qt tries to
work around these limitations.

See also QBasicTimer, QTimerEvent, QObject::timerEvent(), Timers, Analog Clock
Example, and Wiggly Example.


https://doc.qt.io/qt-5/qelapsedtimer.html

The QElapsedTimer class is usually used to quickly calculate how much time has
elapsed between two events. Its API is similar to that of QTime, so code that
was using that can be ported quickly to the new class.

However, unlike QTime, QElapsedTimer tries to use monotonic clocks if possible.
This means it's not possible to convert QElapsedTimer objects to a
human-readable time.

The typical use-case for the class is to determine how much time was spent in a
slow operation. The simplest example of such a case is for debugging purposes,
as in the following example:

    QElapsedTimer timer;
    timer.start();

    slowOperation1();

    qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";

In this example, the timer is started by a call to start() and the elapsed timer
is calculated by the elapsed() function.

The time elapsed can also be used to recalculate the time available for another
operation, after the first one is complete. This is useful when the execution
must complete within a certain time period, but several steps are needed. The
waitFor-type functions in QIODevice and its subclasses are good examples of such
need. In that case, the code could be as follows:

void executeSlowOperations(int timeout)
{
    QElapsedTimer timer;
    timer.start();
    slowOperation1();

    int remainingTime = timeout - timer.elapsed();
    if (remainingTime > 0)
        slowOperation2(remainingTime);
}

Another use-case is to execute a certain operation for a specific timeslice. For
this, QElapsedTimer provides the hasExpired() convenience function, which can be
used to determine if a certain number of milliseconds has already elapsed:

void executeOperationsForTime(int ms)
{
    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(ms))
        slowOperation1();
}

It is often more convenient to use QDeadlineTimer in this case, which counts
towards a timeout in the future instead of tracking elapsed time.

*/

TEST(QtTimer, SingleTimerFromQTimer)
{
  Timer o;

  o.setSingleShot();

  // https://doc.qt.io/qt-5/qsignalspy.html#details
  QSignalSpy timerExpiredSpy(&o, &Timer::timerExpired);

  // use wait to check timer expired event is fired within this tiem.
  timerExpiredSpy.wait(2000);
  EXPECT_EQ(timerExpiredSpy.count(), 1);
}

TEST(QtTimer, SingleTimerFromSingleShot)
{
  Timer o;

  o.useSingleShot();

  // https://doc.qt.io/qt-5/qsignalspy.html#details
  QSignalSpy timerExpiredSpy(&o, &Timer::timerExpired);

  // use wait to check timer expired event is fired within this tiem.
  timerExpiredSpy.wait(2000);
  EXPECT_EQ(timerExpiredSpy.count(), 1);
}

TEST(QtTimer, ContinuousTimerFromQTimer)
{
  Timer o;

  o.setContinuousShot();

  // https://doc.qt.io/qt-5/qsignalspy.html#details
  QSignalSpy timerExpiredSpy(&o, &Timer::timerExpired);

  // use wait to check timer expired event is fired within this tiem.
  //
  // bool QSignalSpy::wait(int timeout = 5000)
  //
  // Starts an event loop that runs until the given signal is received.
  // Optionally the event loop can return earlier on a timeout (in
  // milliseconds).
  //
  // Returns true if the signal was emitted *at least* once in timeout
  // milliseconds, otherwise returns false.
  //
  // So wait() returns as soon as the first timeout signal arrives

  timerExpiredSpy.wait(5000);

  // Can use sleep like this? NO since these blocks this test run including
  // timer
  //
  // QThread::msleep(5000);
  // std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  // one Timer::timerExpired signal and three QTimer::timeout signals
  EXPECT_EQ(timerExpiredSpy.count(), 1);
  EXPECT_EQ(o.getCount(), 3);
}

/*
https://doc.qt.io/qt-5/qobject.html#startTimer


int QObject::startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer)

Starts a timer and returns a timer identifier, or returns zero if it could not
start a timer.

A timer event will occur every interval milliseconds until killTimer() is
called. If interval is 0, then the timer event occurs once every time there are
no more window system events to process.

The virtual timerEvent() function is called with the QTimerEvent event parameter
class when a timer event occurs. Reimplement this function to get timer events.

If multiple timers are running, the QTimerEvent::timerId() can be used to find
out which timer was activated.


*/

TEST(QtTimer, TimerFromQObject)
{
  // std::mutex m;
  // std::condition_variable cv;
  // std::unique_lock<std::mutex> lock(m);
  // TimerQObject o(cv);

  // *cxx-error* why?
  // TimerQObject o();

  TimerQObject o;

  // cannot use this since it blocks all including timer. so use QSignalSpy
  // instead to wait.
  //
  // QThread::exec();
  // cv.wait(lock);

  QSignalSpy timerExpiredSpy(&o, &TimerQObject::timerExpired);
  timerExpiredSpy.wait(5000);

  // 5 since timer can run 5 for 5000 ms
  EXPECT_EQ(o.getCount(), 5);
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

https://doc.qt.io/qt-5/qobject.html#moveToThread

void QObject::moveToThread(QThread *targetThread)

Changes the `thread affinity` for this object and its children. The object
cannot be moved if it has a parent. Event processing will continue in the
targetThread.

`To move an object to the main thread,` use QApplication::instance() to retrieve
a pointer to the current application, and then use QApplication::thread() to
retrieve the thread in which the application lives.

For example:

myObject->moveToThread(QApplication::instance()->thread());

If targetThread is nullptr, *all event processing for this object* and its
children stops, as they are no longer `associated with any thread.`

Note that all active timers for the object will be reset. The timers are first
stopped in the current thread and restarted (with the same interval) in the
targetThread. As a result, constantly moving an object between threads can
postpone timer events indefinitely.

A QEvent::ThreadChange event is sent to this object just before the thread
affinity is changed. You can handle this event to perform any special
processing. Note that any new events that are posted to this object will be
handled in the targetThread, provided it is non-null: when it is nullptr, no
event processing for this object or its children can happen, as they are no
longer associated with any thread.

Warning: This function is not thread-safe; the current thread must be same as
the current thread affinity. In other words, this function can only "push" an
object from the current thread to another thread, it cannot "pull" an object
from any arbitrary thread to the current thread. There is one exception to this
rule however: objects with no thread affinity can be "pulled" to the current
thread.

See also thread().

There is a problem to test Q slot/signal mechanism in TEST() since

o without cv.wait() TEST ends right away so ends QThread as well.
o with cv.wait, all tests runs but thread affinity do not work.
o TEST(QtThread, ThreadAffinity_3) works as expected when run as a standalone.
  see standalone code.

Therefore, may need to run Qt event loop per test case?


TEST(QtThread, ThreadAffinity_1)
{
  // https://wiki.qt.io/QThreads_general_usage

  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  QThread *thread = new QThread;
  Thread *wo      = new Thread(m, cv);
  wo->moveToThread(thread);

  QObject::connect(thread, &QThread::started, wo, &Thread::progress);

  thread->start();

  cv.wait(lock);
}

TEST(QtThread, ThreadAffinity_2)
{
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);
  Counter a;

  QThread *thread = new QThread;
  ThreadDoNotWork *wo = new ThreadDoNotWork(a, m, cv);
  wo->moveToThread(thread);

  // start thread
  QObject::connect(thread, &QThread::started, wo, &ThreadDoNotWork::progress);

  thread->start();

  cv.wait(lock);
}

TEST(QtThread, ThreadAffinity_3)
{
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  QThread *thread = new QThread;
  ThreadWork *wo  = new ThreadWork(m, cv);
  wo->moveToThread(thread);

  // start thread
  QObject::connect(thread, &QThread::started, wo, &ThreadWork::progress);

  thread->start();

  cv.wait(lock);
}

TEST(QtThread, ThreadAffinity_4)
{
  // QCoreApplication app(argc, argv);

  // TEST(QtThread, ThreadAffinity_3)
  {
    QThread *thread       = new QThread;
    ThreadStandAloneY *wo = new ThreadStandAloneY();
    wo->moveToThread(thread);

    // start thread
    QObject::connect(thread, &QThread::started, wo,
                     &ThreadStandAloneY::progress);

    thread->start();
  }

  // return app.exec();
}

*/

TEST(QtThread, ThreadAffinity_1)
{
  // https://wiki.qt.io/QThreads_general_usage

  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  QThread *thread = new QThread;
  Thread *wo      = new Thread(m, cv);
  wo->moveToThread(thread);

  QObject::connect(thread, &QThread::started, wo, &Thread::progress);

  thread->start();

  cv.wait(lock);
}

// TEST(QtThread, useComposite)
// {
//   // so, the belows do not runs as QApplication exited already from the
//   above.
//   // https://doc.qt.io/qt-5/qthread.html
//     using namespace qt_thread_2;

//     Controller co;

//     qCritical() << "before sending operate..";
//     emit co.operate(QString("send operate"));
//     qCritical() << "after sending operate..";
// }

#if 0

/*

https://doc.qt.io/qt-5/qt.html#ConnectionType-enum

enum Qt::ConnectionType

This enum describes the types of connection that can be used between signals and
slots. In particular, it determines whether a particular signal is delivered to
a slot immediately or queued for delivery at a later time.

Constant              Value   Description

Qt::AutoConnection    0       

(Default) If the receiver lives in the thread that emits the signal,
Qt::DirectConnection is used. Otherwise, Qt::QueuedConnection is used. The
connection type is determined when the signal is emitted.

Qt::DirectConnection  1       

The slot is invoked immediately when the signal is emitted. The slot is executed
in the signalling thread.


Qt::QueuedConnection  2       

The slot is invoked when control returns to the event loop of the receiver's
thread. The slot is executed in the receiver's thread.

*/

TEST(DISABLED_QtSlot, QueuedConnection)
{
  // to try QueuedConnection but do not run doSomethingLong() even if waits for
  // its running and finish

  {
    Counter a, b;

    QObject::connect(&a, &Counter::valueChanged,
        &b, &Counter::doSomethingLong, Qt::QueuedConnection);

    qCritical() 
      << "emit signal via valueChange via QueuedConnection and runs slot";

    a.valueChanged(12);

    while (b.value() != 5)
    {
      QThread::sleep(1);
      qCritical() << "checking values";
    }

    qCritical() << "slot finished";

    EXPECT_THAT(b.value(), 5);
  }

  // send signal from main to slot in a thread and it doesn't work either.
  // since main do not have q main loop? 

  {
    std::mutex m;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(m);
    Counter a;

    QThread* thread = new QThread;
    ThreadWorkerUseCounter* wo 
      = new ThreadWorkerUseCounter(a, m, cv);
    wo->moveToThread(thread);

    // start thread
    QObject::connect(thread, &QThread::started, 
        wo, &ThreadWorkerUseCounter::progress);
    thread->start();

    cv.wait(lock);
  }
}



// ={=========================================================================
// qt-event
// note: WHY receiver do not get events???

TEST(QtEvnet, useCustomEvent)
{
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  QSharedPointer<CustomEventReceiver> receiver =
    QSharedPointer<CustomEventReceiver>::create(m, cv);

  CustomEventSender sender(receiver);

  // https://doc.qt.io/qt-5/qsignalspy.html#details
  QSignalSpy timerExpiredSpy(&sender, &CustomEventSender::timerExpired);

  // use wait to check timer expired event is fired within this tiem.
  timerExpiredSpy.wait(3000);
  EXPECT_EQ(timerExpiredSpy.count(), 1);

  /*
  std::mutex m;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lock(m);

  QSharedPointer<CustomEventReceiver> receiver =
    QSharedPointer<CustomEventReceiver>::create(m, cv);

  CustomEventSender sender(receiver);

  // sender.connectTimer(std::chrono::milliseconds(2000));

  // wait for 
  cv.wait(lock);

  qDebug() << "received from %d " << receiver->getReceived();
  */
}

#endif

// ={=========================================================================

static void GoogleTestRunner(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test. Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  int result = RUN_ALL_TESTS();

  QCoreApplication::exit(result);
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  // note: this do not exit when finishes.
  //
  // // Since Google Mock depends on Google Test, InitGoogleMock() is
  // // also responsible for initializing Google Test.  Therefore there's
  // // no need for calling testing::InitGoogleTest() separately.
  // testing::InitGoogleMock(&argc, argv);
  // RUN_ALL_TESTS();

  // This will run the functor from the application event loop.
  QTimer::singleShot(0, &app, std::bind(&GoogleTestRunner, argc, argv));

  return app.exec();
}
