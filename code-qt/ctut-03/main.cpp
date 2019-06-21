#include <QCoreApplication>
#include <QtWidgets>
#include "mainwindow.h"

/*

o As with Tutorial 2, uses `MainWindow` but add a button in ctor and which is do
the same.

o Use signal and connect

https://wiki.qt.io/Qt_for_Beginners

Subclassing QWidget

Until now, we have put all of our code in the main function. This was not a
problem for our simple examples, but for more and more complex applications we
might want to split our code into different classes. What is often done is to
create a class that is used to display a window, and implement all the widgets
that are contained in this window as attributes of this class.

Please note that there is no need for writing a destructor for deleting
m_button. With the parenting system, when the Window instance is out of the
stack, the m_button is automatically deleted.


Signals and slots

https://doc.qt.io/qt-5/signalsandslots.html

o Signals and slots are used for communication between "objects."

we have an alternative to the callback technique: We use signals and slots. A
signal is emitted when a particular event occurs. Qt's widgets have many
predefined signals, but we can always subclass widgets to add our own signals to
them. A slot is a function that is called in response to a particular signal.
Qt's widgets have many pre-defined slots, but it is common practice to subclass
widgets and add your own slots so that you can handle the signals that you are
interested in

o The signals and slots mechanism is type safe: The signature of a signal must
  `match` the signature of the receiving slot.

  (In fact a slot may have a shorter signature than the signal it receives
  because it can ignore extra arguments.)

o Signals are public access functions and can be emitted from anywhere, but we
  recommend to only emit them from the class that defines the signal and its
  subclasses.

o When a signal is emitted, the slots connected to it are usually executed
  immediately, `just like a normal function call` When this happens, the signals
  and slots mechanism is totally `independent` of any GUI event loop. Execution
  of the code following the emit statement will occur once all slots have
  returned.  

  The situation is slightly different when using queued connections; in such a
  case, the code following the emit keyword will continue immediately, and the
  slots will be executed later.

o If several slots are connected to one signal, the slots will be executed one
  after the other, in the order they have been connected, when the signal is
  emitted.

o Signals are automatically generated by the moc and must not be implemented in
  the .cpp file. They can never have return types (i.e. use void).

o However, as slots, they can be invoked by any component, `regardless of` its
  access level, via a signal-slot connection. This means that a signal emitted
  from an instance of an arbitrary class can cause a private slot to be invoked
  in an instance of an unrelated class.

  (From SO) From another class, you can't call a private slot as a function, but
  if you emit a signal connected to that private slot, you can invoke it."

  ??? since fails on QObject::connect() when use private slot.

o You can also define slots to be virtual, which we have found quite useful in
  practice.

o slot is overloaded; Qt will select the appropriate version when you connect a
  signal to the slot. With callbacks, you'd have to find five different names
  and keep track of the types yourself.

o use function pointer

  connect(sender, &QObject::destroyed, this, &MyObject::objectDestroyed);

  Note that signal and slot arguments are not checked by the compiler when using
  this QObject::connect() overload.

o You can also connect to functors or C++11 lambdas:

  connect(sender, &QObject::destroyed, 
      this, [=](){ this->m_objects.remove(sender); });

o The string-based SIGNAL and SLOT `syntax` will detect type mismatches at
  `runtime`

  The other way to connect a signal to a slot is to use QObject::connect() and
  the SIGNAL and SLOT macros. The rule about whether to include arguments or not
  in the SIGNAL() and SLOT() macros, if the arguments have default values, is
  that the signature passed to the SIGNAL() macro must not have fewer arguments
  than the signature passed to the SLOT() macro.

  But this one won't work:

    connect(sender, SIGNAL(destroyed()), this, SLOT(objectDestroyed(QObject*)));

  because the slot will be expecting a QObject that the signal will not send.
  This connection will report a runtime error.


// ex from https://doc.qt.io/qt-5/signalsandslots.html
// shows it's object based

class Counter : public QObject
{
  Q_OBJECT

  public:
    Counter() { m_value = 0; }

    int value() const { return m_value; }

    public slots:
      void setValue(int value);

  signals:
    void valueChanged(int newValue);

  private:
    int m_value;
};

void Counter::setValue(int value)
{
  if (value != m_value) {
    m_value = value;
    emit valueChanged(value);
  }
}

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
}

o This example illustrates that objects can work together without needing to
know any information about each other. 

<qt-moc-error>

The Q_OBJECT macro is expanded by the preprocessor to declare several member
functions that are implemented by the moc; if you get compiler errors along the
lines of "undefined reference to vtable for LcdNumber", you have probably
forgotten to run the moc or to include the moc output in the link command.


Observer pattern is used everywhere in GUI applications, and often leads to some
boilerplate code. Qt was created with the idea of removing this boilerplate code
and providing a nice and clean syntax, and the signal and slots mechanism is the
answer.

Instead of having observable objects and observers, and registering them, Qt
provides two high level concepts: signals and slots.

o A signal is a message that an object can send, most of the time to inform of a
status change.

o A slot is a function that is used to accept and respond to a signal.

In order to respond to a signal, a slot must be connected to a signal. Qt
provides the method QObject::connect. It is used this way, with the two macros
SIGNAL and SLOT

FooObjectA *fooA = new FooObjectA();
FooObjectB *fooB = new FooObjectB();

QObject::connect(fooA, SIGNAL (bared()), fooB, SLOT (baz()));

assuming that FooObjectA have a bared signal, and FooObjectB have a baz slot.

You have to write the signature of the signal and the slot inside the two macros
SIGNAL and SLOT.

Remark : Basically, signals and slots are methods, that might or might not have
arguments, but that never return anything. While the notion of a signal as a
method is unusual, a slot is actually a real method, and can be called as usual
in other methods, or whilst responding to a signal.


Transmitting information

The signals and slots mechanism is useful to respond to buttons clicks, but it
can do much more than that. For example, It can also be used to communicate
information. Let's say while playing a song, a progress bar is needed to show
how much time remains before the song is over. A media player might have a class
that is used to check the progress of the media. An instance of this class might
periodically send a tick signal, with the progress value. This signal can be
connected to a QProgressBar, that can be used to display the progress.

The hypothetical class used to check the progress might have a signal that have
this signature :

void MediaProgressManager::tick(int miliseconds);

and we know from the documentation, that the QProgressBar has this slot:

void QProgressBar::setValue(int value);

You can see that the signal and the slot have the same kind of parameters,
    especially the type. If you connect a signal to a slot that does not share
    the same kind of parameters, when the connection is done (at run-time) you
    will get a warning like:

QObject::connect: Incompatible sender/receiver arguments

This is because the signal transmits the information to the slot using the
parameters. The first parameter of the signal is passed to the first one of the
slot, and the same for second, third, and so forth.

The code for the connection will look like this:

MediaProgressManager *manager = new MediaProgressManager();
QProgressBar *progress = new QProgressBar(window);

QObject::connect(manager, SIGNAL (tick(int)), progress, SLOT (setValue(int)));

You can see that you have to `provide a signature inside the SIGNAL and SLOT`
macro, providing the type of the values that are passed through the signals. You
may also provide the name of the variable if you want. (It is actually even
better).


Features of signals and slots

A signal can be connected to several slots

Many signals can be connected to a slot

A signal can be connected to a signal: it is signal relaying. The second signal
is sent if the first signal is sent.


Responding to an event

Let's try to actually make something with this app, like being able to close it
while clicking on the button. 

o We already know that QPushButton provides the clicked signal. 

o We also have to know that QApplication provides the quit slot, that closes the
application.

In order to make a click on a button close the app, we have to connect the
signal clicked of the button to the quit slot of QApplication instance. We can
modify the code from the previous chapter to do this, but before that, you might
wonder how to access to the QApplication instance while you are in another
class. Actually, it is pretty simple, since there exists a static function in
QApplication, with the following signature, that is used to get it:

QApplication * QApplication::instance()

*/

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  MainWindow w;

  // QPushButton *button = new QPushButton("Hello Qt Tutorial 2", &w);
  // button->setGeometry(20, 20, 180, 30);

  w.show();

  return a.exec();
}
