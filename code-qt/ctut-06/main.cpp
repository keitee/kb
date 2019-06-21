#include <QApplication>
#include <QProgressBar>
#include <QSlider>

/*

https://wiki.qt.io/Qt_for_Beginners

Transmitting information with signals and slots

Here is a simpler example for information transmission. It only displays a
progress bar and a slider (created by QSlider) inside a window, and while the
slider is moved, the value of the progress bar is synced with a very simple
connection.

The interesting signals and slots are:

void QSlider::valueChanged(int value);
void QProgressBar::setValue(int value);

QSlider automatically emits the signal valueChanged with the new value passed as
a parameter when the value is changed, and the method setValue of QProgressBar,
is used, as we have seen, to set the value of the progress bar.

*/

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // MainWindow w;
  // w.show();

  // create a container window
  QWidget window;
  window.setFixedSize(200, 80);

  // create a progress bar
  QProgressBar *progressBar = new QProgressBar(&window);
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->setGeometry(10, 10, 180, 30);

  // create a horizontal silder
  QSlider *slider = new QSlider(&window);
  slider->setOrientation(Qt::Horizontal);
  slider->setRange(0, 100);
  slider->setValue(0);
  slider->setGeometry(10, 40, 180, 30);

  window.show();

  // connection
  QObject::connect(slider, SIGNAL(valueChanged(int)),
      progressBar, SLOT(setValue(int)));

  return a.exec();
}
