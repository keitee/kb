#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    count_(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timer_expired()));
  timer->start(1000);
}

void MainWindow::timer_expired()
{
    ++count_;
    QString mesg = QString("timer expired %1").arg(count_);
    ui->textEdit->setPlainText(mesg);
}

void MainWindow::on_pushButton_2_clicked()
{
  ui->textEdit->setPlainText("Single Shot timer is started");
  QTimer *timer = new QTimer(this);
  timer->setSingleShot(true);
  timer->setInterval(2000);
  timer->start();
  connect(timer, SIGNAL(timeout()), this, SLOT(single_timer_expired()));
}

void MainWindow::single_timer_expired()
{
    ui->textEdit->setPlainText("Single Shot timer is expired");
}

void MainWindow::on_pushButton_3_clicked()
{
    QElapsedTimer *timer = new QElapsedTimer();

    for (int i = 0; i < 2000000; ++i)
    {
        QString mesg = "mesg";
    }

    QString mesg = QString("timer elapsed %1 ms").arg(timer->elapsed());
    ui->textEdit->setPlainText(mesg);
}
