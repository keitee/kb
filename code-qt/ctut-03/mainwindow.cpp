#include "mainwindow.h"
// #include "ui_mainwindow.h"

// used when selects "ui" form from options when ceates
// MainWindow::MainWindow(QWidget *parent) :
//     QMainWindow(parent),
//     ui(new Ui::MainWindow)
// {
//   ui->setupUi(this);
// }

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
  button_ = new QPushButton("Hello World", this);
  button_->setGeometry(10, 10, 180, 30);

  connect(button_, SIGNAL(clicked()), 
      QApplication::instance(), SLOT(quit()));
}

MainWindow::~MainWindow()
{
  // delete ui;
}
