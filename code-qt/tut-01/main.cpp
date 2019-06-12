#include <QApplication>
#include <QtWidgets>

/*

https://wiki.qt.io/Qt_for_Beginners

<event-loop>

QApplication is a very important class. It takes care of input arguments, but
also a lot of other things, and most notably, the event loop. The event loop is
a loop that waits for user input in GUI applications.

When calling app.exec() the event loop is launched.


<attribute-or-property>

Qt extends this system of attributes and getters and setters to something called
property. A property is a value of any type that can be accessed, be modified or
constant, and can notify a change. The property system is useful, especially in
the third part (QML). For now, we will use "attribute" or "property" to do the
same thing.


<qobject>

QObject is the most basic class in Qt. Most of classes in Qt inherit from this
class. QObject provides some very powerful capabilities like:

o object name : you can set a name, as a string, to an object and search for
  objects by names.

o parenting system (described in the following section)

o signals and slots (described in the next chapter)

o event management


Parenting system

Parenting system is a convenient way of dealing with objects in Qt, especially
widgets. Any object that inherits from QObject can have a parent and children.
This hierarchy tree makes many things convenient:

When an object is destroyed, all of its children are destroyed as well. So,
calling delete becomes optional in certain cases.

All QObjects have findChild and findChildren methods that can be used to search
for children of a given object.

Child widgets in a QWidget automatically appear inside the parent widget.

*/

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QPushButton button(" Hello Qt Widget");

  // o The following snippet that creates a QPushButton inside a QPushButton:
  // o button(parent) <- button2(child)
  // o button2 is on top of button

  QPushButton button2(" Other", &button);

  button.setToolTip(" Hello Tootip");

  QFont font("Hack");
  button.setFont(font);

  button.setIcon(QIcon::fromTheme("face-smile"));

  button.show();

  return a.exec();
}
