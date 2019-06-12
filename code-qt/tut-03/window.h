#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

private:
    Ui::Window *ui;
    QPushButton *m_button;
};

#endif // WINDOW_H
