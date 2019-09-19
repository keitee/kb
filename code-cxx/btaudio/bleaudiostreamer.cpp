/*
 *
*/

#include <iostream>
#include <functional>

#include "slog.h"
#include "bleaudiostreamer.h"

namespace
{
  std::string stringMessage(int message)
  {
    switch(message)
    {
      case Message::DeviceConnectedMsg:          return std::string("DeviceConnectedMsg"); break;
      case Message::TransportAddedMsg:           return std::string("TransportAddedMsg"); break;
      case Message::TransportRemovedMsg:         return std::string("TransportRemovedMsg"); break;
      case Message::TransportPropertyChangeMsg:  return std::string("TransportPropertyChangeMsg"); break;
      case Message::PlayerAddedMsg:              return std::string("PlayerAddedMsg"); break;
      case Message::PlayerRemovedMsg:            return std::string("PlayerRemovedMsg"); break;
      case Message::PlayerPropertyChangeMsg:     return std::string("PlayerPropertyChangeMsg"); break;
      default: return std::string("unknown message"); break;
    }
  }

  // ref: https://www.bluetooth.org/en-us/specification/assigned-numbers/service-discovery
  const std::string BLE_AUDIO_SOURCE_UUID{"0000110a"};

  struct BleAuidoMetadata
  {
    bool operator==(BleAuidoMetadata const &rhs) const
    {
      if ((title != rhs.title) ||
          (artist != rhs.artist) ||
          (album != rhs.album) ||
          (track_number != rhs.track_number) ||
          (number_of_tracks != rhs.number_of_tracks))
      { return false; }

      return true;
    }

    std::string title{};
    std::string artist{};
    std::string album{};
    uint32_t track_number{};
    uint32_t number_of_tracks{};
    std::string genre{};
  };
} // namespace


/*
*/

BleAudioStreamer::BleAudioStreamer(std::string const &name)
  : running_(true)
  , t_(std::thread(&BleAudioStreamer::doWork_, this, name))
{
  LOG_MSG("BleAudioStreamer ctor");

  setupStateMachine_();
}

void BleAudioStreamer::postMessage(int message)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(message));
}

void BleAudioStreamer::onTransportAdded(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::TransportAddedMsg, path));
}

void BleAudioStreamer::onTransportRemoved(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::TransportRemovedMsg, path));
}

void BleAudioStreamer::onTransportPropertyChange(std::string const &path
    , std::string const &property, std::string const &value)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::TransportPropertyChangeMsg, path, property, value));
}

void BleAudioStreamer::onPlayerAdded(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::PlayerAddedMsg, path));
}
void BleAudioStreamer::onPlayerRemoved(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::PlayerRemovedMsg, path));
}
void BleAudioStreamer::onPlayerPropertyChange(std::string const &path
    , std::string const &property, std::string const &value)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::PlayerPropertyChangeMsg, path, property, value));
}

void BleAudioStreamer::onDevicePropertyChange(std::string const &path
    , std::string const &property, std::string const &value)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(Message(Message::DevicePropertyChangeMsg, path, property, value));
}


/*
*/

void BleAudioStreamer::doWork_(std::string const &name)
{
  pthread_setname_np(pthread_self(), name.c_str()); 

  while (running_)
  {
    auto message = q_.wait_and_pop();
    LOG_MSG("message is %s", stringMessage(message.type_).c_str());

    switch(message.type_)
    {
      case Message::DevicePropertyChangeMsg:
        if (message.property_ == "Connected")
        {
          if (message.value_ == "true")
          {
            device_path_ = message.path_;

            fsm_.postEvent(DeviceConnectedEvent);
          }
          else if (message.value_ == "false")
            fsm_.postEvent(DeviceDisconnectedEvent);
          else
          {
            LOG_MSG("DevicePropertyChangeMsg got unknown %s property value",
                message.value_.c_str());
          }
        }
        else
        {
          LOG_MSG("DevicePropertyChangeMsg got unknown %s property",
              message.property_.c_str());
        }

        break;

      case Message::TransportAddedMsg:
        {
          if (transport_path_.empty())
          {
            transport_path_ = message.path_;
            fsm_.postEvent(TransportAddedEvent);
          }
          else
          {
            LOG_MSG("TransportAddedMsg receive but transport path, %s, is not empty",
                transport_path_.c_str());
          }
        }
        break;

      case Message::PlayerAddedMsg:
        {
          if (player_path_.empty())
          {
            player_path_ = message.path_;
            fsm_.postEvent(PlayerAddedEvent);
          }
          else
          {
            LOG_MSG("PlayerAddedMsg receive but player path, %s, is not empty",
                transport_path_.c_str());
          }
        }
        break;

      // TODO: use either state = p.getValue().asString() or
      // proxy->getState() as player Status do?
      //
      // have to use PlayerPropertyChangeMsg way since otherwise have to expand
      // Message to carry metadata/pos update.

      case Message::TransportPropertyChangeMsg:
        {
          if (message.property_ == "State")
          {
            // get property value
            // std::string state{};
            // player->getStatue(message.path_, state); 

            if (message.value_ == "idle")
              fsm_.postEvent(TransportIdelEvent);
            else if(message.value_ == "pending")
              fsm_.postEvent(TransportPendingEvent);
            else if(message.value_ == "active")
              fsm_.postEvent(TransportActiveEvent);
            else
            {
              LOG_MSG("TransportPropertyChangeMsg got unknown %s property value", 
                  message.value_.c_str());
            }
          }
          else
          {
            LOG_MSG("TransportPropertyChangeMsg got unknown %s property", 
                message.property_.c_str());
          }
        }
        break;

      case Message::PlayerPropertyChangeMsg:
        {
          if (message.property_ == "Track")
          {
            // build metadata and notify to client
            updateMetadata_();
            notify_(MESSAGE_TYPE_METADATA);
          }
          else if(message.property_ == "Position")
          {
            // build pos update and notify to AS
            // have to access proxy
          }
          else if(message.property_ == "Status")
          {
            // get property value
            // std::string state{};
            // player->getStatue(message.path_, state); 

            (message.value_ == "playing" 
             ? fsm_.postEvent(PlayerPlayingEvent)
             : fsm_.postEvent(PlayerStoppedEvent));
          }
          else
          {
            LOG_MSG("PlayerPropertyChangeMsg got unknown %s property", 
                message.property_.c_str());
          }
        }
        break;

      case Message::PlayerRemovedMsg:
        fsm_.postEvent(PlayerRemovedEvent);
        break;

      case Message::TransportRemovedMsg:
        fsm_.postEvent(TransportRemovedEvent);
        break;
    } // switch
  } // while
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

  fsm_.setTransistionLogLevel(true);
  fsm_.setObjectName("BleAudioStreamer");

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


/* check if device to connect has audio source by checking through UUIDs
 */
bool BleAudioStreamer::checkHasAudioSource_(std::string const *path)
{
  // TODO: device proxy
  if (device_proxy_)
  {
    std::vector<std::string> uuids;
    if (device_proxy_->getUUIDs(path, uuids))
    {
      auto it = std::find_if(uuids.cbegin(), uuids.cend(),
          [](std::string const &uuid)
          {
            return (uuid.substr(0, 8) == BLE_AUDIO_SOURCE_UUID) ? true : false;
          });
      return (it == uuilds.cend()) ? false : true;
    }
  }

  return false;
}


/* get metadata from player proxy and update streamer with it
 */
void BleAudioStreamer::updateMetadata_()
{
  if (!player_path_.empty())
  {
    player_proxy_->getTrackTitle(player_path_, metadata_.title);
    player_proxy_->getTrackArtist(player_path_, metadata_.artist);
    player_proxy_->getTrackAlbum(player_path_, metadata_.album);
    player_proxy_->getTrackGenre(player_path_, metadata_.genre);
    player_proxy_->getTrackNumberOfTracks(player_path_, metadata_.number_of_tracks);
    player_proxy_->getTrackNumber(player_path_, metadata_.track_number);
  }
  else
  {
    LOG_MSG("player path is not set");
  }
}


/* update client with messages
 */
void BleAudioStreamer::notify_(MessageType type)
{
  Message msg{};
  bool updated{false};

  switch(type)
  {
    case MESSAGE_TYPE_METADATA:
      msg.type          = MESSAGE_TYPE_METADATA;
      msg.title         = metadata_.title;
      msg.artist        = metadata_.artist;
      msg.album         = metadata_.album;
      msg.number        = metadata_.track_number;
      msg.totalNumbers  = metadata_.number_of_tracks;
      msg.genre         = metadata_.genre;
      updated = true;
      break;

    default:
      LOG_MSG("unknown AudioStreamer::MessageType, %d", type);
      break;
  }

  if (updated)
  {
    // TODO: needs a lock as the previous did?
    observer_(msg, observer_data_); 
  }
}


/* eafs
*/

void BleAudioStreamer::entered_(int state)
{
  LOG_MSG("fsm entered: %s", stringState_((FsmState)state).c_str());

  switch(state)
  {
    case DeviceOnState:
      eafDeviceOnState_();
      break;

    case DeviceOffState:
      eafDeviceOffState_();
      break;

    case TransportOnState:
      eafTransportOnState_();
      break;

    case PlayerOnState:
      eafTransportOnState_();
      break;

    case PlayerPendingState:
      eafPlayerPendingState_();
      break;

    case PlayerReadyState:
      eafPlayerReadyState_();
      break;
  }
}

void BleAudioStreamer::exited_(int state)
{
  LOG_MSG("fsm exited: %s", stringState_((FsmState)state).c_str());
}

void BleAudioStreamer::eafDeviceOnState_()
{
  if (checkHasAudioSource_(device_path_))
  {
    // TODO: start disconnection timer
  }
}

void BleAudioStreamer::eafDeviceOffState_()
{
  if (disconnection_timer_)
  {
    // TODO: stop disconnection timer
  }

  device_path_.clear();
}

void BleAudioStreamer::eafTransportOnState_()
{
  // TODO: nothing to do?
}


/* metadata must be updated before and have been set
 */
void BleAudioStreamer::eafPlayerOnState_()
{
  notify_(MESSAGE_TYPE_METADATA);
}

void BleAudioStreamer::eafPlayerPendingState_()
{
  uint16_t not_used1, not_used2;

  // acquire fd
#ifdef BLEAUDIO_DEBUG
  LOG_MSG("debug: have got fd from transport proxy");
#else
  // TODO: not clear on what to do if this call fails. the old code has bring up
  // timer and fake player event. what are they?
  transport_proxy_->tryAcquire(transport_path_, fd_, not_used1, not_used2); 
#endif
}

void BleAudioStreamer::eafPlayerReadyState_()
{
  // start a reader with fd 
#ifdef BLEAUDIO_DEBUG
  LOG_MSG("debug: have created a reader which reads data from fd");
#else
  // TODO: config?
  // bool BluetoothMediaFsm::getTransportConfig(const std::string &path, a2dp_sbc_t &config)
  // TODO: relationship with Reader?
  // TODO: use of unique_ptr<>?
  reader_ = new BluetoothMediaReader(fd_, config);
#endif
}

