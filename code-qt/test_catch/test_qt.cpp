// https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#top

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <QDebug>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <QVariant>

/*
={=============================================================================
qstring

https://doc.qt.io/qt-5/qstring.html

The QString class provides a Unicode character string.

QString stores a string of 16-bit QChars, where each QChar corresponds to one
UTF-16 code unit. (Unicode characters with code values above 65535 are stored
using surrogate pairs, i.e., two consecutive QChars.)

In addition to QString, Qt also provides the "QByteArray" class to store raw
bytes and traditional 8-bit '\0'-terminated strings. For most purposes, QString
is the class you want to use. It is used throughout the Qt API, and the Unicode
support ensures that your applications will be easy to translate if you want to
expand your application's market at some point. The two main cases where
QByteArray is appropriate are when you need to store raw binary data, and when
memory conservation is critical (like in embedded systems).

Converting Between 8-Bit Strings and Unicode Strings

QString provides the following three functions that return a const char *
version of the string as QByteArray: toUtf8(), toLatin1(), and toLocal8Bit().

toLatin1() returns a Latin-1 (ISO 8859-1) encoded 8-bit string.

toUtf8() returns a UTF-8 encoded 8-bit string. UTF-8 is a superset of US-ASCII
(ANSI X3.4-1986) that supports the entire Unicode character set through
multibyte sequences.

toLocal8Bit() returns an 8-bit string using the system's local encoding.

To convert from one of these encodings, QString provides fromLatin1(),
fromUtf8(), and fromLocal8Bit(). Other encodings are supported through the
QTextCodec class.

As mentioned above, QString provides a lot of functions and operators that make
it easy to interoperate with const char * strings. But this functionality is a
double-edged sword: It makes QString more convenient to use if all strings are
US-ASCII or Latin-1, but there is always the risk that an implicit conversion
from or to const char * is done using the wrong 8-bit encoding. To minimize
these risks, you can turn off these implicit conversions by defining the
following two preprocessor symbols:

QT_NO_CAST_FROM_ASCII disables automatic conversions from C string literals and
pointers to Unicode.

QT_RESTRICTED_CAST_FROM_ASCII allows automatic conversions from C characters and
character arrays, but disables automatic conversions from character pointers to
Unicode.

QT_NO_CAST_TO_ASCII disables automatic conversion from QString to C strings.

One way to define these preprocessor symbols globally for your application is to
add the following entry to your qmake project file:

DEFINES += QT_NO_CAST_FROM_ASCII \ 
 QT_NO_CAST_TO_ASCII

You then need to explicitly call fromUtf8(), fromLatin1(), or fromLocal8Bit() to
construct a QString from an 8-bit string, or use the lightweight QLatin1String
class, for example:

Distinction Between Null and Empty Strings

For historical reasons, QString distinguishes between a null string and an empty
string. A null string is a string that is initialized using QString's default
constructor or by passing (const char *)0 to the constructor. An empty string is
any string with size 0. A null string is always empty, but an empty string isn't
necessarily null:

QString().isNull();               // returns true
QString().isEmpty();              // returns true

QString("").isNull();             // returns false
QString("").isEmpty();            // returns true

QString("abc").isNull();          // returns false
QString("abc").isEmpty();         // returns false

All functions except isNull() treat null strings the same as empty strings.  For
example, toUtf8().constData() returns a pointer to a '\0' character for a null
string (not a null pointer), and QString() compares equal to QString("").

We recommend that you always use the isEmpty() function and avoid isNull().

*/

TEST_CASE("convert QString to const char *", "[qstring]")
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
    REQUIRE(os.str() == "Hello stream logging");
    // EXPECT_THAT(os.str(), "Hello stream logging");
  }

  // toLatin1()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    os << message.toLatin1().constData();
    REQUIRE(os.str() == "Hello stream logging");
  }

  // toLocal8Bit()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    os << message.toLocal8Bit().constData();
    REQUIRE(os.str() == "Hello stream logging");
  }

  // qPrintable()
  {
    std::ostringstream os;
    QString message{"Hello stream logging"};
    // const char *qPrintable(const QString &str)
    os << qPrintable(message);
    REQUIRE(os.str() == "Hello stream logging");
  }
}

TEST_CASE("format QString", "[qstring]")
{
  // First, arg(i) replaces %1. Then arg(total) replaces %2. Finally,
  // arg(fileName) replaces %3.
  //
  // arg() is better:
  //
  // One advantage of using arg() over asprintf() is that the order of the
  // `numbered place markers` can change, if the application's strings are
  // translated into other languages, but each arg() will still replace the
  // lowest numbered unreplaced place marker, no matter where it appears. 
  //
  // Also, if place marker %i appears more than once in the string, the arg()
  // replaces all of them.
  //
  // QString QString::asprintf(const char *cformat, ...)
  //
  // Warning: We do not recommend using QString::asprintf() in new Qt code.
  //
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
    REQUIRE(os.str() == "Processing file 3 of 10: readme");
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
    REQUIRE(os.str() == "Processing file 3 of 10: readme");
  }
}

TEST_CASE("explore QString operations", "[qstring]")
{

  // int QString::indexOf
  // (const QString &str, int from = 0, Qt::CaseSensitivity cs =
  // Qt::CaseSensitive) const
  //
  // Returns the index position of the first occurrence of the string str in this
  // string, searching forward from index position from. Returns -1 if str is not
  // found.

  SECTION( "QString::indexOf" ) 
  {
    QString coll{"QtString supports indexOf method"};

    REQUIRE(coll.indexOf("indexOf") == 18);

    REQUIRE(coll.indexOf("indexof") == -1);
  }

  SECTION( "QString::startsWith" ) 
  {
    QString coll{"value=10"};

    REQUIRE(coll.startsWith("value=") == true);

    REQUIRE(coll.mid(6) == "10");
  }

// int QString::compare
//  (const QString &other, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
// 
// This function overloads compare().
// 
// Lexically compares this string with the other string and returns an integer
// less than, equal to, or greater than zero if this string is less than, equal
// to, or greater than the other string.

  SECTION( "QString::compare" ) 
  {
    QString coll{"compare"};

    // equal
    REQUIRE(coll.compare("compare") == 0);
  }
}

/*
={=============================================================================
qbytearray

https://doc.qt.io/qt-5/qbytearray.html

Detailed Description

QByteArray can be used to store both raw bytes (including '\0's) and traditional
8-bit '\0'-terminated strings. Using QByteArray is much more convenient than
using const char *. Behind the scenes, it always ensures that the data is
followed by a '\0' terminator, and uses implicit sharing (copy-on-write) to
reduce memory usage and avoid needless copying of data.

In addition to QByteArray, Qt also provides the QString class to store string
data. For most purposes, QString is the class you want to use. It stores 16-bit
Unicode characters, making it easy to store non-ASCII/non-Latin-1 characters in
your application. Furthermore, QString is used throughout in the Qt API. The two
main cases where QByteArray is appropriate are when you need to store raw binary
data, and when memory conservation is critical (e.g., with Qt for Embedded
Linux).

One way to initialize a QByteArray is simply to pass a const char * to its
constructor. For example, the following code creates a byte array of size 5
containing the data "Hello":

QByteArray ba("Hello");

Although the size() is 5, the byte array also maintains an extra '\0' character
at the end so that if a function is used that asks for a pointer to the
underlying data (e.g. a call to data()), the data pointed to is guaranteed to be
'\0'-terminated.


To obtain a pointer to the actual character data, call data() or constData().

char *QByteArray::data()

Returns a pointer to the data stored in the byte array. The pointer can be used
to access and modify the bytes that compose the array. The data is
'\0'-terminated, i.e. the number of bytes in the returned character string is
size() + 1 for the '\0' terminator.

Example:

QByteArray ba("Hello world");
char *data = ba.data();
while (*data) {
    cout << "[" << *data << "]" << endl;
    ++data;
}

The pointer remains valid as long as the byte array isn't reallocated or
destroyed. For read-only access, constData() is faster because it never causes a
deep copy to occur.

This function is mostly useful to pass a byte array to a function that accepts a
const char *.

This '\0' byte is automatically provided by QByteArray and is not counted in
size().


QByteArrayLiteral(ba)

The macro generates the data for a QByteArray out of the string literal ba at
compile time. Creating a QByteArray from it is free in this case, and the
generated byte array data is stored in the read-only segment of the compiled
object file.

For instance:

QByteArray ba = QByteArrayLiteral("byte array contents");

Using QByteArrayLiteral instead of a double quoted plain C++ string literal can
significantly speed up creation of QByteArray instances from data known at
compile time.

See also QStringLiteral.


https://doc.qt.io/qt-5/qstring.html#QStringLiteral

QStringLiteral(str)

The macro generates the data for a QString out of the string literal str at
compile time. Creating a QString from it is free in this case, and the generated
string data is stored in the read-only segment of the compiled object file.

*/

TEST_CASE("create QByteArry", "[qstring]")
{
  {
    QByteArray coll{"U%03hhu SkyQ EC201"};

    // qDebug("QByteArray: %s", coll.data());
    // std::cout << "QByteArray: " << coll.data() << std::endl;

    std::ostringstream os;
    os << coll.data();
    REQUIRE(os.str() == "U%03hhu SkyQ EC201");
  }

  {
    QByteArray coll = QByteArrayLiteral("U%03hhu SkyQ EC201");

    // error since QByteArrayLiteral is macro
    // QByteArray coll = QByteArrayLiteral{"U%03hhu SkyQ EC201"};

    std::ostringstream os;
    os << coll.data();
    REQUIRE(os.str() == "U%03hhu SkyQ EC201");
  }

  {
    QString coll = QStringLiteral("U%03hhu SkyQ EC201");

    std::ostringstream os;
    os << qPrintable(coll);
    REQUIRE(os.str() == "U%03hhu SkyQ EC201");
  }
}

/*
={=============================================================================
qstringlist

https://doc.qt.io/qt-5/qstringlist.html

Detailed Description

QStringList inherits from QList<QString>. Like QList, QStringList is implicitly
shared. It provides fast index-based access as well as fast insertions and
removals. Passing string lists as value parameters is both fast and safe.

*/

TEST_CASE("create QStringList", "[qstringlist]")
{
  QStringList coll1 = {"Arial", "Helvetica", "Times"};

  // Adding Strings
  // Strings can be added to a list using the insert() append(), operator+=()
  // and operator<<() functions.
  //
  // operator<<() can be used to conveniently add multiple elements to a list:

  QStringList coll2;
  coll2 << "Arial";
  coll2 << "Helvetica";
  coll2 << "Times";

  REQUIRE(coll1 == coll2);
}

TEST_CASE("iterate QStringList", "[qstringlist]")
{
  {
    QStringList coll1 = {"Arial", "Helvetica", "Times"};
    std::ostringstream os;

    auto iter = coll1.constBegin();
    for (iter; iter != coll1.constEnd(); ++iter)
      os << (*iter).toLocal8Bit().constData() << ", ";

    REQUIRE(os.str() == "Arial, Helvetica, Times, ");
  }

  {
    QStringList coll1 = {"Arial", "Helvetica", "Times"};
    std::ostringstream os;

    auto iter = coll1.constBegin();
    for (iter; iter != coll1.constEnd(); ++iter)
      os << qPrintable(*iter) << ", ";

    REQUIRE(os.str() == "Arial, Helvetica, Times, ");
  }

  {
    QStringList coll1 = {"Arial", "Helvetica", "Times"};
    std::ostringstream os;

    for (auto const &e : coll1)
      os << qPrintable(e) << ", ";

    REQUIRE(os.str() == "Arial, Helvetica, Times, ");
  }
}

/*
={=============================================================================
qlist
*/

TEST_CASE("explore QList operations", "[qlist]")
{
  SECTION("QList::prepend") 
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

    REQUIRE(os.str() == "three, two, one, ");
  }
}

/*
={=============================================================================
qqueue
*/

TEST_CASE("explore QQueue operations", "[qqueue]")
{
  SECTION("QList::enqueue") 
  {
    QQueue<int> coll;

    coll.enqueue(1);
    coll.enqueue(2);
    coll.enqueue(3);

    REQUIRE(coll.size() == 3);

    // T QQueue::dequeue()
    // Removes the head item in the queue and returns it. This function assumes
    // that the queue isn't empty.

    REQUIRE(coll.dequeue() == 1);
    REQUIRE(coll.size() == 2);
  }
}

/*
={=============================================================================
qvariant

https://doc.qt.io/qt-5/qvariant.html#QVariantMap-typedef

A QVariant object holds *a single value of a single type()* at a time. (Some
type()s are multi-valued, for example a string list.) You can find out what
type, T, the variant holds, convert it to a different type using convert(), get
its value using one of the toT() functions (e.g., toSize()) and check whether
the type can be converted to a particular type using canConvert().

/Qt/5.12.3/Src/qtbase/src/corelib/kernel/qvariant.cpp

see how it's documented in the code

/! \fn template<typename T> T QVariant::value() const

    Returns the stored value converted to the template type \c{T}.
    Call canConvert() to find out whether a type can be converted.
    If the value cannot be converted, a \l{default-constructed value}
    will be returned.

    If the type \c{T} is supported by QVariant, this function behaves
    exactly as toString(), toInt() etc.

    Example:

    \snippet code/src_corelib_kernel_qvariant.cpp 5

    If the QVariant contains a pointer to a type derived from QObject then
    \c{T} may be any QObject type. If the pointer stored in the QVariant can be
    qobject_cast to T, then that result is returned. Otherwise a null pointer is
    returned. Note that this only works for QObject subclasses which use the
    Q_OBJECT macro.

    If the QVariant contains a sequential container and \c{T} is QVariantList, the
    elements of the container will be converted into \l {QVariant}s and returned as a QVariantList.

    \snippet code/src_corelib_kernel_qvariant.cpp 9

    \sa setValue(), fromValue(), canConvert(), Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE()


/Qt/5.12.3/Src/qtbase/src/corelib/kernel/qvariant.h

    template<typename T>
    inline T value() const
    { return qvariant_cast<T>(*this); }

// from https://doc.qt.io/qt-5/qvariant.html#value

T QVariant::value() const

Returns the stored value "converted to the template type T". Call canConvert()
to find out whether a type can be converted. If the value cannot be converted, a
default-constructed value will be returned.

If the type T is supported by QVariant, this function behaves exactly as
toString(), toInt() etc.


bool QVariant::canConvert() const
Returns true if the variant can be converted to `the template type T`, otherwise
false.


QVariantList
  
A QVariant containing a sequential container will also return true for this
function if the targetTypeId is QVariantList. It is possible to iterate over the
contents of the container without extracting it as a (copied) QVariantList:

typedef QVariant::QVariantList
Synonym for QList<QVariant>.

If the QVariant contains a sequential container and T is QVariantList, the
elements of the container will be converted into QVariants and returned as a
QVariantList.

*/

TEST_CASE("convert QVariant", "[qvariant]")
{
  {
    QVariant v;

    v = 7;

    REQUIRE(7 == v.toInt());

    // compile error since "7" is not QString
    // EXPECT_THAT("7", v.toString());
    REQUIRE(QString("7") == v.toString());

    // const char *QVariant::typeName() const
    // Returns the name of the type stored in the variant. The returned strings
    // describe the C++ datatype used to store the data: for example, "QFont",
    // "QString", or "QVariantList". An Invalid variant returns 0.

    // causes falures
    // /home/keitee/git/kb/code-qt/qtest/qtest.cpp:427: Failure
    // Value of: "int"
    // Expected: is equal to 0x7fae3adc4d6a pointing to "int"
    //   Actual: "int" (of type char [4])
    // EXPECT_THAT("int", v.typeName());

    // https://doc.qt.io/qt-5/qbytearray.html
    REQUIRE(QByteArray("int") == QByteArray(v.typeName()));

    REQUIRE(v.toInt() == v.value<int>());        // same as v.toInt()
    REQUIRE(v.toString() == v.value<QString>()); // same as v.toString()
  }

  {
    QVariant v{42};

    REQUIRE(v.canConvert<int>() == true);
    REQUIRE(v.canConvert<QString>() == true);
  }
}


/*

QVariant QVariant::fromValue(const T &value)

Returns a QVariant containing a copy of value. Behaves exactly like setValue()
otherwise.

code:

template<typename T>
static inline QVariant fromValue(const T &value)
{ return qVariantFromValue(value); }


void QVariant::setValue(const T &value)

Stores a copy of value. If T is a type that QVariant doesn't support, QMetaType
is used to store the value. A compile error will occur if QMetaType doesn't
handle the type.

code:

template<typename T>
inline void QVariant::setValue(const T &avalue)
{ qVariantSetValue(*this, avalue); }

*/

TEST_CASE("store QVariant", "[qvariant]")
{
  SECTION("QVariant::setValue") 
  {
    {
      QVariant v1;
      v1.setValue(5);

      QVariant v2{5};

      REQUIRE(v1 == v2);
    }

    {
      QVariant v1;
      v1.setValue(5);

      QVariant v2;
      v2.fromValue(5);

      REQUIRE(v1 == v2);
    }
  }

  // output:
  // QVariant(int, 7)
  // QVariant(int, 11)
  // QVariant(int, 42)

  SECTION("QVariant::fromValue")
  {
    QList<int> coll{7, 11, 42};
    QVariant v = QVariant::fromValue(coll);

    REQUIRE(QByteArray("QList<int>") == QByteArray(v.typeName()));

    // qDebug() << v;

    if (v.canConvert<QVariantList>())
    {
      QSequentialIterable it = v.value<QSequentialIterable>();

      // cxx-range-for
      for (const QVariant &v : it)
        qDebug() << v;
    }
  }

  // You can even store QList<QVariant> and QMap<QString, QVariant> values in a
  // variant, so you can easily construct arbitrarily complex data structures of
  // arbitrary types. This is very powerful and versatile, but may prove less
  // memory and speed efficient than storing specific types in standard data
  // structures.

  SECTION("to and from QStringList") 
  {
    QStringList coll1 = {"Arial", "Helvetica", "Times"};
    QVariant v(coll1);

    REQUIRE(v.canConvert<QStringList>() == true);

    QStringList coll2 = v.value<QStringList>();
    REQUIRE(coll1 == coll2);
  }
}

TEST_CASE("null QVariant", "[qvariant]")
{
  // QVariant also supports the notion of null values, where you can have a
  // defined type with no value set. However, note that QVariant types can only
  // be cast when they have had a value set.

  // bool QVariant::isNull() const
  //
  // Returns true if this is a null variant, false otherwise. A variant is
  // considered null if it contains no initialized value, or the contained value
  // is nullptr or is an instance of a built-in type that has an isNull method,
  // in which case the result would be the same as calling isNull on the wrapped
  // object.
  //
  // Warning: Null variants is not a single state and two null variants may
  // easily return false on the == operator if they do not contain similar null
  // values.

  QVariant x;

  REQUIRE(x.isNull() == true);

  // OK
  // QVariant y((QString()));

  // OK.
  QVariant y(QString{});

  REQUIRE(y.isNull() == true);

  QVariant z(QString(""));

  REQUIRE(z.isNull() == false);
}
