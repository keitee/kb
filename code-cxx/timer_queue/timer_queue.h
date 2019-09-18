
class TimerQueue
{
  public:
    TimerQueue();
    ~TimerQueue();

  private:
    std::mutex mtx_;
    std::thread thread_

    int timer_fd_;
    int event_fd_;

    bool callback_;

};
