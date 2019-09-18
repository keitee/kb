/*
 *
*/

#ifndef BLEAUDIOSTREAMER_H
#define BLEAUDIOSTREAMER_H

#include <thread>
#include <mutex>
#include <deque>
#include <string>

#include "eventqueue.h"
#include "statemachine.h"

struct Message
{
  enum
  {
    DeviceConnectedMsg,
    TransportAddedMsg,
    TransportRemovedMsg,
    TransportPropertyChangeMsg,
    PlayerAddedMsg,
    PlayerRemovedMsg,
    PlayerPropertyChangeMsg,
    DevicePropertyChangeMsg
  };

  explicit Message(int type, std::string const &path = std::string()
      , std::string const &property = std::string()
      , std::string const &value = std::string())
    : type_(type), path_(path), property_(property), value_(value)
  {}

  int type_;
  std::string path_;
  std::string property_;

  // TODO debug purpose
  std::string value_;
};


class BleAudioStreamer
{
  public:
    BleAudioStreamer(std::string const &name = std::string("BT AUDIO"));

    ~BleAudioStreamer() 
    {
      running_ = false;

      // to quit
      postMessage(333);
      t_.join();
    };

    void postMessage(int message);

    enum FsmState 
    {
      DeviceOffState,
      DeviceOnState,
      TransportOnState,
      PlayerOnState,
      PlayerPendingState,
      PlayerReadyState,
      PlayerRunningState,
      PlayerStopState,
      TransportIdleState,
      PlayerOffState,
      TransportOffState
    };

    enum FsmEvent 
    {
      DeviceConnectedEvent,
      DeviceDisconnectedEvent,
      TransportAddedEvent,
      PlayerAddedEvent,
      TransportPendingEvent,
      TransportActiveEvent,
      PlayerPlayingEvent,
      PlayerStoppedEvent,
      TransportIdelEvent,
      PlayerRemovedEvent,
      TransportRemovedEvent
    };


    // observer interfaces
    void onTransportAdded(std::string const &path);
    void onTransportRemoved(std::string const &path);
    void onTransportPropertyChange(std::string const &path
        , std::string const &property, std::string const &value);
    void onPlayerAdded(std::string const &path);
    void onPlayerRemoved(std::string const &path);
    void onPlayerPropertyChange(std::string const &path
        , std::string const &property, std::string const &value);
    void onDevicePropertyChange(std::string const &path
        , std::string const &property, std::string const &value);

  private:
    BleAudio::queue<::Message> q_;
    std::thread t_;
    std::mutex m_;

    bool running_;
    void doWork_(std::string const &name);

    StateMachine fsm_;

    void setupStateMachine_();

    std::string stringEvent_(FsmEvent);
    std::string stringState_(FsmState);

    void entered_(int state);
    void exited_(int state);
};

#endif // BLEAUDIOSTREAMER_H
