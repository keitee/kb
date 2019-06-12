#include "mainwindow.h"
#include <QApplication>

/*

https://wiki.qt.io/Qt_for_Beginners

The Meta Object

Qt provides a meta-object system. Meta-object (literally "over the object") is a
way to achieve some programming paradigms that are normally impossible to
achieve with pure C++ like:

Introspection : capability of examining a type at run-time
Asynchronous function calls

To use such meta-object capabilities in an application, one can subclass QObject
and mark it so that the "meta-object compiler (moc)" can interpret and translate
it.

Code produced by moc includes signals and slots signatures, methods that are
used to retrieve meta-information from those marked classes, properties
handling... All this information can be accessed using the following method:

const QMetaObject * QObject::metaObject () const

QMetaObject class contains all the methods that deal with meta-objects.

Important macros

The most important macro is Q_OBJECT. Signal-Slot connections and their syntax
cannot be interpreted by a regular C++ compiler. The moc is provided to
translate the QT syntax like "connect", "signals", "slots", etc into regular C++
syntax. This is done by specifying the Q_OBJECT macro in the header containing
class definitions that use such syntax.

mywidget.h

class MyWidget : public QWidget
{
 Q_OBJECT
 public:
  MyWidget(QWidget *parent = 0);
}


Others marker macros for moc are

signals
public / protected / private slots

that mark the different methods that need to be extended.

SIGNAL and SLOT are also two very important and useful macros. When a signal is
emitted, the meta-object system is used to compare the signature of the signal,
to check the connection, and to find the slot using it's signature. These macros
are actually used to convert the provided method signature into a string that
matches the one stored in the meta-object.


Creating custom signals and slots

This chapter covers the second part of signals and slots: implementing custom
signals and slots.

Creating custom slots and signals is really simple. Slots are like normal
methods, but with small decorations around, while signals need little to no
implementation at all.

Creating custom signals and slots is very simple. It is described by the
following checklist:

add Q_OBJECT macro

add signals section, and write signals prototypes

add public slots or protected slots or private slots sections, and write slots
prototypes

implement slots as normal methods

establish connections


Creating custom slots

In order to implement a slot, we first need to make the class be able to send
signals and have slots (see the previous chapter). This is done by setting the
Q_OBJECT macro in the class declaration (often in the header).

After that, a slot should be declared in the corresponding section, and
implemented as a normal method.

Finally, slots are connected to signals.

*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  m_button = new QPushButton("Hello World", this);
  m_button->setCheckable(true);

  // set the counter to 0
  m_counter = 0;

  // note: have to have this. If not, do not increase counter and not exit app.

  connect(m_button, SIGNAL(clicked(bool)),
      this, SLOT(slotButtonClicked(bool)));

  // Connecting the newly created signal to the quit slot is done as usual:

  connect(this, SIGNAL(counterReached()),
      QApplication::instance(), SLOT(quit()));
}

MainWindow::~MainWindow()
{

}

/*

We also add our custom slot. Since we are trying to react from the button being
checked, and since the corresponding signal is

void QPushButton::clicked(bool checked)

`checked` makes a button "pressed down" when clicked and need to click it
again to make "pressed up".

*/

void MainWindow::slotButtonClicked(bool checked)
{
  if (checked)
  {
    m_button->setText("Checked");
  }
  else
  {
    m_button->setText("Hello Qt World!");
  }

  ++m_counter;

  if (m_counter == 10)
  {
    emit counterReached();
  }
}
