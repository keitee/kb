#include <iostream>
#include <set>
#include <memory>
#include <exception>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QList>
#include <QString>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcEditor, "qtc.editor");

using namespace std;

// ={=========================================================================
// -expect

TEST(Qt, OrderOfArg)
{
  int value{100};

  EXPECT_THAT(value, 100);
  EXPECT_THAT(100, value);

  // // t_ex_mock.cpp:262: Failure
  // // Value of: value
  // // Expected: is equal to 101
  // //   Actual: 100 (of type int)
  //
  // EXPECT_THAT(value, 101);

  // //
  // // t_ex_mock.cpp:263: Failure
  // // Value of: 101
  // // Expected: is equal to 100
  // //   Actual: 101 (of type int)
  // EXPECT_THAT(101, value);
}

// to see which makes better messages
//
// [ RUN      ] Gtest.ExpectOrEq
// gtest.cpp:283: Failure
// Value of: 101
// Expected: is equal to 100
//   Actual: 101 (of type int)
//
// gtest.cpp:284: Failure
// Expected equality of these values:
//   101
//   value
//     Which is: 100
// [  FAILED  ] Gtest.ExpectOrEq (0 ms)
// [----------] 2 tests from Gtest (0 ms total)

TEST(Qt, ExpectOrEq)
{
  // QCoreApplication a(argc, argv); 
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
  // return a.exec();
}

int main(int argc, char **argv)
{
  // Since Google Mock depends on Google Test, InitGoogleMock() is
  // also responsible for initializing Google Test.  Therefore there's
  // no need for calling testing::InitGoogleTest() separately.
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
