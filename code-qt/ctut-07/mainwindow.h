// #ifndef MAINWINDOW_H
// #define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

/*

Emitting custom signals

Based on the previous example, we want to close the application if the button is
clicked (checked or unchecked) 10 times. We first need to implement a counter
that will count the number of clicks. 

Now, we have to create a custom signal that is used to notify other components,
that the counter has reached 10. In order to declare a signal, we have to add a

note: Even if the signal is declared as a method, there is `no need to implement`
it. The meta-object compiler is used to do this.

*/

signals:
    void counterReached();
    void signalNotConnected();

private slots:
    void slotButtonClicked(bool checked);
    void slotButtonMessageLogging(bool checked);

private:
    QPushButton *m_button;
    int m_counter;
};

// #endif // MAINWINDOW_H
