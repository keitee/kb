#include <QTimer>

class UseTimer : public QObject
{
  Q_OBJECT

  public:
    explicit UseTimer();
    virtual ~UseTimer();

    void fire_repeat_timer();
    void fire_single_timer();
    void fire_single_timer_2();

  public slots:
  void timer_update_slot();
  void timer_single_slot();
  void timer_single_slot_2();

  // private:
  public:
    QTimer *timer_;
    int count_;
};
