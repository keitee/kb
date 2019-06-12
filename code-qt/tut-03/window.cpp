#include "window.h"
#include "ui_window.h"

/*

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
while clicking on the button. We already know that QPushButton provides the
clicked signal. We also have to know that QApplication provides the quit slot,
that closes the application.

In order to make a click on a button close the app, we have to connect the
signal clicked of the button to the quit slot of QApplication instance. We can
modify the code from the previous chapter to do this, but before that, you might
wonder how to access to the QApplication instance while you are in another
class. Actually, it is pretty simple, since there exists a static function in
QApplication, with the following signature, that is used to get it:

QApplication * QApplication::instance()

*/

Window::Window(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Window)
{
  ui->setupUi(this);

  // setFixedSize(100, 50);
  m_button = new QPushButton("Hello World", this);
  m_button->setGeometry(10, 10, 180, 30);

  // do the connection
  connect(m_button, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));
}

Window::~Window()
{
  delete ui;
}
