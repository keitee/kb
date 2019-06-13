#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QKeyEvent>

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual bool event(QEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void customEvent(QEvent *event) override;

// private slots:
//   void slotButtonClicked(bool checked);

private:
    Ui::Window *ui;
    QPushButton *m_button;
};

#endif // WINDOW_H
