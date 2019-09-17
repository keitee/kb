/*
 *
*/

#include <iostream>
#include <functional>
#include "bleaudiostreamer.h"

namespace
{
  class Message
  {
    public:

      // explicit Message(MessageType type, std::string const &path) 
      //   : type_(type), path_(path), property_()
      // {}

      // explicit Message(MessageType type, std::string const &path) 
      //   : type_(type), path_(path), property_()
      // {}

      explicit Message(MessageType type, std::string const &path, std::string const &property) 
        : type_(type), path_(path), property_(property)
      {}

      enum MessageType 
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

    private:
      MessageType type_;
      std::string path_{};
      std::string property_{};
  };

  std::string stringMessage(MessageType message)
  {
    switch(state)
    {
      case DeviceConnectedMsg:          return std::string("DeviceConnectedMsg"); break;
      case TransportAddedMsg:           return std::string("TransportAddedMsg"); break;
      case TransportRemovedMsg:         return std::string("TransportRemovedMsg"); break;
      case TransportPropertyChangeMsg:  return std::string("TransportPropertyChangeMsg"); break;
      case PlayerAddedMsg:              return std::string("PlayerAddedMsg"); break;
      case PlayerRemovedMsg:            return std::string("PlayerRemovedMsg"); break;
      case PlayerPropertyChangeMsg:     return std::string("PlayerPropertyChangeMsg"); break;
      default: return std::string("unknown message"); break;
    }
  }
} // namespace


/*
*/

BleAudioStreamer::BleAudioStreamer(std::string const &name)
  : running_(true)
  , t_(std::thread(&BleAudioStreamer::doWork_, this, name))
{
  std::cout << "BleAudioStreamer ctor" << std::endl;

  setupStateMachine_();
}

void BleAudioStreamer::postMessage(int message)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(message);
}

void onTransportAdded(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(TransportAddedMsg, path));
}

void onTransportRemoved(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(TransportRemovedMsg, path));
}

void BleAudioStreamer::onTransportPropertyChange(std::string const &path, std::string const &property)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(TransportPropertyChangeMsg, path));
}

void BleAudioStreamer::onPlayerAdded(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(PlayerAddedMsg, path));
}
void BleAudioStreamer::onPlayerRemoved(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(PlayerRemovedMsg, path));
}
void BleAudioStreamer::onPlayerPropertyChange(std::string const &path, std::string const &property)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(PlayerPropertyChangeMsg, path));
}

void BleAudioStreamer::onDevicePropertyChange(std::string const &path, std::string const &property)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(DevicePropertyChangedMsg, path));
}


/*
*/

void BleAudioStreamer::doWork_(std::string const &name)
{
  pthread_setname_np(pthread_self(), name.c_str()); 

  while (running_)
  {
    // std::cout << "wait on q" << std::endl;
    auto message = q_.wait_and_pop();
    std::cout << "event got is " << stringEvent_((FsmEvent)message) << std::endl;
    fsm_.postEvent(message);
  }
}

void BleAudioStreamer::setupStateMachine_()
{
  fsm_.addState(DeviceOffState, "DeviceOffState");
  fsm_.addState(DeviceOnState, "DeviceOnState");
  fsm_.addState(TransportOnState, "TransportOnState");
  fsm_.addState(PlayerOnState, "PlayerOnState");
  fsm_.addState(PlayerPendingState, "PlayerPendingState");
  fsm_.addState(PlayerReadyState, "PlayerReadyState");
  fsm_.addState(PlayerRunningState, "PlayerRunningState");
  fsm_.addState(PlayerStopState, "PlayerStopState");
  fsm_.addState(TransportIdleState, "TransportIdelEvent");
  fsm_.addState(PlayerOffState, "PlayerOffState");
  fsm_.addState(TransportOffState, "TransportOffState");

  fsm_.addTransition(DeviceOffState, DeviceConnectedEvent, DeviceOnState);
  fsm_.addTransition(DeviceOnState, TransportAddedEvent, TransportOnState);
  fsm_.addTransition(TransportOnState, PlayerAddedEvent, PlayerOnState);
  fsm_.addTransition(PlayerOnState, TransportPendingEvent, PlayerPendingState);
  fsm_.addTransition(PlayerPendingState, TransportActiveEvent, PlayerReadyState);
  fsm_.addTransition(PlayerReadyState, PlayerPlayingEvent, PlayerRunningState);
  fsm_.addTransition(PlayerRunningState, PlayerStoppedEvent, PlayerStopState);
  fsm_.addTransition(PlayerStopState, TransportIdelEvent, TransportIdleState);
  fsm_.addTransition(TransportIdleState, PlayerRemovedEvent, PlayerOffState);
  fsm_.addTransition(PlayerOffState, TransportRemovedEvent, TransportOffState);
  fsm_.addTransition(TransportOffState, DeviceDisconnectedEvent, DeviceOffState);

  fsm_.connect(std::bind(&BleAudioStreamer::entered_, this, std::placeholders::_1),
      std::bind(&BleAudioStreamer::exited_, this, std::placeholders::_1));

  fsm_.setInitialState(DeviceOffState);
  fsm_.start();
}

std::string BleAudioStreamer::stringState_(FsmState state)
{
  switch(state)
  {
    case DeviceOffState:      return std::string("DeviceOffState"); break;
    case DeviceOnState:       return std::string("DeviceOnState"); break;
    case TransportOnState:    return std::string("TransportOnState"); break;
    case PlayerOnState:       return std::string("PlayerOnState"); break;
    case PlayerPendingState:  return std::string("PlayerPendingState"); break;
    case PlayerReadyState:    return std::string("PlayerReadyState"); break;
    case PlayerRunningState:  return std::string("PlayerRunningState"); break;
    case PlayerStopState:     return std::string("PlayerStopState"); break;
    case TransportIdleState:  return std::string("TransportIdleState"); break;
    case PlayerOffState:      return std::string("PlayerOffState"); break;
    case TransportOffState:   return std::string("TransportOffState"); break;
    default: return std::string("unknown state"); break;
  }
}

std::string BleAudioStreamer::stringEvent_(FsmEvent event)
{
  switch(event)
  {
    case DeviceConnectedEvent:    return std::string("DeviceConnectedEvent"); break;
    case DeviceDisconnectedEvent: return std::string("DeviceDisconnectedEvent"); break;
    case TransportAddedEvent:     return std::string("TransportAddedEvent"); break;
    case PlayerAddedEvent:        return std::string("PlayerAddedEvent"); break;
    case TransportPendingEvent:   return std::string("TransportAddedEvent"); break;
    case TransportActiveEvent:    return std::string("TransportActiveEvent"); break;
    case PlayerPlayingEvent:      return std::string("PlayerPlayingEvent"); break;
    case PlayerStoppedEvent:      return std::string("PlayerStoppedEvent"); break;
    case TransportIdelEvent:      return std::string("TransportIdelEvent"); break;
    case PlayerRemovedEvent:      return std::string("PlayerRemovedEvent"); break;
    case TransportRemovedEvent:   return std::string("TransportRemovedEvent"); break;
    default: return std::string("unknown event"); break;
  }
}

void BleAudioStreamer::entered_(int state)
{
  std::cout << "fsm entered: " << stringState_((FsmState)state) << std::endl;
}

void BleAudioStreamer::exited_(int state)
{
  std::cout << "fsm exited: " << stringState_((FsmState)state) << std::endl;
}


