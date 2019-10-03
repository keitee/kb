/*
 *
 */

#include <functional>
#include <iostream>

#include "bleaudioreader.h"
#include "bleaudiostreamer.h"

#ifdef USE_HOST_BUILD

#include "slog.h"
#define AS_LOG_ERROR LOG_MSG
#define AS_LOG_INFO LOG_MSG

#else

#include "AS_Diag.h"
extern AS_DIAG::Context *dbusitf_logging_ctx;
#undef AS_DIAG_CONTEXT_DEFAULT
#define AS_DIAG_CONTEXT_DEFAULT (dbusitf_logging_ctx)

#endif // USE_HOST_BUILD

/* ={=========================================================================
 * @brief
 */

namespace {

std::string stringMessage(int message)
{
  switch (message)
  {
  // case BleAudioMessage::DeviceConnectedMsg:
  //   return std::string("DeviceConnectedMsg");
  //   break;
  case BleAudioMessage::TransportAddedMsg:
    return std::string("TransportAddedMsg");
    break;
  case BleAudioMessage::TransportRemovedMsg:
    return std::string("TransportRemovedMsg");
    break;
  case BleAudioMessage::TransportPropertyChangeMsg:
    return std::string("TransportPropertyChangeMsg");
    break;
  case BleAudioMessage::PlayerAddedMsg:
    return std::string("PlayerAddedMsg");
    break;
  case BleAudioMessage::PlayerRemovedMsg:
    return std::string("PlayerRemovedMsg");
    break;
  case BleAudioMessage::PlayerPropertyChangeMsg:
    return std::string("PlayerPropertyChangeMsg");
    break;
  case BleAudioMessage::DevicePropertyChangeMsg:
    return std::string("DevicePropertyChangeMsg");
    break;
  case BleAudioMessage::PlayerBufferReadyMsg:
    return std::string("PlayerBufferReadyMsg");
    break;
  case BleAudioMessage::PlayerFormatChangeMsg:
    return std::string("PlayerFormatChangeMsg");
    break;
  case BleAudioMessage::ClientStopRequestMsg:
    return std::string("ClientStopRequestMsg");
    break;
  case BleAudioMessage::ClientQuitRequestMsg:
    return std::string("ClientQuitRequestMsg");
    break;
  default:
    return std::string("unknown message");
    break;
  }
}

// ref:
// https://www.bluetooth.org/en-us/specification/assigned-numbers/service-discovery
const std::string BLE_AUDIO_SOURCE_UUID{"0000110a"};

// in ms
const size_t PLAY_REFRESH_INTERVAL{1000};

// in ms
const size_t PLAY_HALT_INTERVAL{3500};

BleAudioStreamer *singleton = nullptr;

} // namespace

/* ={=========================================================================
 * @brief
 */

void BluetoothApi::Streamer::createInstance()
{
  if (!singleton)
    singleton = new BleAudioStreamer();
}

void BluetoothApi::Streamer::destroyInstance()
{
  if (!singleton)
  {
    delete singleton;
    singleton = nullptr;
  }
}

BluetoothApi::Streamer *BluetoothApi::Streamer::instance()
{
  return singleton;
}

/* ={=========================================================================
 * @brief
 */

BleAudioStreamer::BleAudioStreamer(std::string const &name)
    : running_(true), t_(std::thread(&BleAudioStreamer::doWork_, this, name))
{
  LOG_MSG("::BleAudioStreamer");

  setupStateMachine_();
}

BleAudioStreamer::~BleAudioStreamer()
{
  LOG_MSG("::~BleAudioStreamer");

  q_.push(BleAudioMessage(BleAudioMessage::ClientQuitRequestMsg));
  t_.join();
}

/* ={=========================================================================
 * @brief Debugging purpose
 */
void BleAudioStreamer::postMessage(int message)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(message));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onTransportAdded(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::TransportAddedMsg, path));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onTransportRemoved(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::TransportRemovedMsg, path));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onTransportPropertyChange(std::string const &path,
                                                 std::string const &property,
                                                 std::string const &value)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::TransportPropertyChangeMsg, path,
                          property, value));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onPlayerAdded(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::PlayerAddedMsg, path));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onPlayerRemoved(std::string const &path)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::PlayerRemovedMsg, path));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onPlayerPropertyChange(std::string const &path,
                                              std::string const &property,
                                              std::string const &value)
{
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::PlayerPropertyChangeMsg, path,
                          property, value));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onDevicePropertyChange(std::string const &path,
                                              std::string const &property,
                                              std::string const &value)
{
  // TODO: the original code do not have lock when called from dbus listener
  std::lock_guard<std::mutex> lock(m_);

  q_.push(BleAudioMessage(BleAudioMessage::DevicePropertyChangeMsg, path,
                          property, value));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onFormatChanged(FormatType type, uint32_t rate,
                                       uint32_t channels)
{
  // TODO: called from reader
  std::lock_guard<std::mutex> lock(m_);

  LOG_MSG("onFormatChange");

  play_format_.type_     = type;
  play_format_.rate_     = rate;
  play_format_.channels_ = channels;

  q_.push(BleAudioMessage(BleAudioMessage::PlayerFormatChangeMsg));
}

/* ={=========================================================================
 * @brief
 */

void BleAudioStreamer::onBufferReady(char *buffer, size_t size, size_t samples)
{
  // TODO: called from reader
  std::lock_guard<std::mutex> lock(m_);

  LOG_MSG("onBufferReady");

  // TODO:
  // make a copy of the received buffer and which will be notified to the client
  // The client has to use releaseBuffer(void *) when finishes with it

  play_buffer_.data_          = buffer;
  play_buffer_.size_          = size;
  play_buffer_.sample_number_ = samples;

  q_.push(BleAudioMessage(BleAudioMessage::PlayerBufferReadyMsg));
}

/* ={=========================================================================
 * BluetoothApi::Streamer
 * @brief The client application will set disconnection timeout in seconds of
 * device and if not, uses the default which is 0.
 */

void BleAudioStreamer::setBluetoothTimeout(uint32_t timeout)
{
  std::lock_guard<std::mutex> lock(m_);

  LOG_MSG("BleAudioStreamer::setBluetoothTimeout:");

  disconnection_timeout_ = timeout;
}

/* ={=========================================================================
 * BluetoothApi::Streamer
 * @brief Release a data buffer.
 */

void BleAudioStreamer::releaseBuffer(void *buffer)
{
  std::lock_guard<std::mutex> lock(m_);

  LOG_MSG("BleAudioStreamer::releaseBuffer:");
}

/* ={=========================================================================
 * BluetoothApi::Streamer
 * @brief Set the callback for messages.
 */

void BleAudioStreamer::setMessageHandler(MessageHandler handler, void *user)
{
  std::lock_guard<std::mutex> lock(m_);

  LOG_MSG("BleAudioStreamer::releaseBuffer:");

  observer_      = handler;
  observer_data_ = user;
}

/* ={=========================================================================
 * BluetoothApi::Streamer
 * @brief Informs the streaming device that we want to stop the stream
 */
void BleAudioStreamer::stop()
{
  std::lock_guard<std::mutex> lock(m_);

  LOG_MSG("BleAudioStreamer::stop:");

  q_.push(BleAudioMessage(BleAudioMessage::ClientStopRequestMsg));
}

/* ={--------------------------------------------------------------------------
 * timer callbacks
 */

bool BleAudioStreamer::onDisconnectionTimerExpired_()
{
#ifdef USE_HOST_BUILD
  LOG_MSG("host: dvice %s disconnected", device_path_.c_str());
#else
  if (!device_proxy_ || device_path_.empty())
  {
    // error
    LOG_MSG("device proxy and device path must be available at this point");
    return false;
  }

  LOG_MSG("disconnection timer expired and dvice %s disconnected",
          device_path_.c_str());
  device_proxy_->disconnect(device_path_);
#endif

  disconnection_timer_ = 0;
  device_path_.clear();

  return true;
}

/* ={--------------------------------------------------------------------------
 * timer callbacks
 */

bool BleAudioStreamer::onRefreshTimerExpired_()
{
#ifdef USE_HOST_BUILD
  LOG_MSG("host: refresh timer expired");
#else
  if (!player_proxy_ || player_path_.empty())
  {
    // error
    LOG_MSG("player proxy and player path must be available at this point");
    return false;
  }

  LOG_MSG("refresh timer expired and refresh player");
  player_proxy_->refresh(player_path_);
#endif

  return true;
}

/* ={--------------------------------------------------------------------------
 * timer callbacks
 */

bool BleAudioStreamer::onStopTimerExpired_()
{
#ifdef USE_HOST_BUILD
  LOG_MSG("host: stop timer expired");
#else
  if (!device_proxy_ || device_path_.empty())
  {
    // error
    LOG_MSG("device proxy and device path are not available at this point");
    return false;
  }

  LOG_MSG("stop timer expired and stop device");
  device_proxy_->disconnect(device_path_);
#endif

  return true;
}

/* ={--------------------------------------------------------------------------
 * @brief
 */

void BleAudioStreamer::doWork_(std::string const &name)
{
  pthread_setname_np(pthread_self(), name.c_str());

  while (running_)
  {
    auto message = q_.wait_and_pop();
    LOG_MSG("message is %s", stringMessage(message.type_).c_str());

    switch (message.type_)
    {
    case BleAudioMessage::DevicePropertyChangeMsg:
      if (message.property_ == "Connected")
      {
        if (message.value_ == "true")
        {
          // start fsm only when it has audio source
          if (checkHasAudioSource_(device_path_))
          {
            device_path_ = message.path_;
            fsm_.postEvent(DeviceConnectedEvent);
          }
        }
        else if (message.value_ == "false")
        {
          fsm_.postEvent(DeviceDisconnectedEvent);
        }
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

    case BleAudioMessage::TransportAddedMsg:
    {
      if (transport_path_.empty())
      {
        transport_path_ = message.path_;
        fsm_.postEvent(TransportAddedEvent);
      }
      else
      {
        LOG_MSG(
            "TransportAddedMsg receive but transport path, %s, is not empty",
            transport_path_.c_str());
      }
    }
    break;

    case BleAudioMessage::TransportRemovedMsg:
      fsm_.postEvent(TransportRemovedEvent);
      break;

    case BleAudioMessage::PlayerAddedMsg:
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
      // BleAudioMessage to carry metadata/pos update.

    case BleAudioMessage::TransportPropertyChangeMsg:
    {
      if (message.property_ == "State")
      {
        // get property value
        // std::string state{};
        // player->getStatue(message.path_, state);

        if (message.value_ == "idle")
        {
          fsm_.postEvent(TransportIdelEvent);
        }
        else if (message.value_ == "pending")
        {
          fsm_.postEvent(TransportPendingEvent);
        }
        else if (message.value_ == "active")
        {
          fsm_.postEvent(TransportActiveEvent);
        }
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

    case BleAudioMessage::PlayerPropertyChangeMsg:
    {
      if (message.property_ == "Track")
      {
        // build metadata and notify to client
        updateMetadata_();
        notify_(MESSAGE_TYPE_METADATA);
      }
      else if (message.property_ == "Position")
      {
        // build pos update and notify to AS
        updatePosition_();
        notify_(MESSAGE_TYPE_PROGRESS);
      }
      else if (message.property_ == "Status")
      {
        updateState_();

        // move to play when gets playing state and not in playing state
        if (play_state_ == "playing" && !fsm_.inState(PlayerRunningState))
        {
          fsm_.postEvent(PlayerPlayingEvent);
        }

        // move to stop when gets non-playing states while in playing state
        if (play_state_ != "playing" && fsm_.inState(PlayerRunningState))
        {
          fsm_.postEvent(PlayerStoppedEvent);
        }

        // // update client while in playing state
        // if (play_state_ == "playing" && fsm_.inState(PlayerRunningState))
        // {
        //   notify_(MESSAGE_TYPE_PLAYER_STATE);
        // }
      }
      else
      {
        LOG_MSG("PlayerPropertyChangeMsg got unknown %s property",
                message.property_.c_str());
      }
    }
    break;

    case BleAudioMessage::PlayerRemovedMsg:
      fsm_.postEvent(PlayerRemovedEvent);
      break;

    case BleAudioMessage::PlayerBufferReadyMsg:
      LOG_MSG("notify client with the filled buffer, size(%d), samples(%d)",
              play_buffer_.size_, play_buffer_.sample_number_);
      // notify_(MESSAGE_TYPE_BUFFER_FILLED);
      break;

    case BleAudioMessage::PlayerFormatChangeMsg:

      // TODO: BleAudioFormat.str()?
      LOG_MSG("notify client with the format change, type(%d), rate(%d), "
              "channel(%d)",
              play_format_.type_, play_format_.rate_, play_format_.channels_);
      break;

    case BleAudioMessage::ClientStopRequestMsg:

      LOG_MSG("get stop request from the client on player %s",
              player_path_.c_str());

      handleStopRequest_();

      break;

    case BleAudioMessage::ClientQuitRequestMsg:
      running_ = false;
      break;

    default:
      LOG_MSG("unknown message is %d", message.type_);
      break;
    } // switch
  }   // while
}

/* ={--------------------------------------------------------------------------
 * @brief FSMs
 */

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
  fsm_.addTransition(PlayerPendingState, TransportActiveEvent,
                     PlayerReadyState);
  fsm_.addTransition(PlayerReadyState, PlayerPlayingEvent, PlayerRunningState);
  fsm_.addTransition(PlayerRunningState, PlayerStoppedEvent, PlayerStopState);
  fsm_.addTransition(PlayerStopState, TransportIdelEvent, TransportIdleState);
  fsm_.addTransition(TransportIdleState, PlayerRemovedEvent, PlayerOffState);
  fsm_.addTransition(PlayerOffState, TransportRemovedEvent, TransportOffState);
  fsm_.addTransition(TransportOffState, DeviceDisconnectedEvent,
                     DeviceOffState);

  fsm_.connect(
      std::bind(&BleAudioStreamer::entered_, this, std::placeholders::_1),
      std::bind(&BleAudioStreamer::exited_, this, std::placeholders::_1));

  fsm_.setTransistionLogLevel(true);
  fsm_.setObjectName("BleAudioStreamer");

  fsm_.setInitialState(DeviceOffState);
  fsm_.start();
}

std::string BleAudioStreamer::stringState_(FsmState state)
{
  switch (state)
  {
  case DeviceOffState:
    return std::string("DeviceOffState");
    break;
  case DeviceOnState:
    return std::string("DeviceOnState");
    break;
  case TransportOnState:
    return std::string("TransportOnState");
    break;
  case PlayerOnState:
    return std::string("PlayerOnState");
    break;
  case PlayerPendingState:
    return std::string("PlayerPendingState");
    break;
  case PlayerReadyState:
    return std::string("PlayerReadyState");
    break;
  case PlayerRunningState:
    return std::string("PlayerRunningState");
    break;
  case PlayerStopState:
    return std::string("PlayerStopState");
    break;
  case TransportIdleState:
    return std::string("TransportIdleState");
    break;
  case PlayerOffState:
    return std::string("PlayerOffState");
    break;
  case TransportOffState:
    return std::string("TransportOffState");
    break;
  default:
    return std::string("unknown state");
    break;
  }
}

std::string BleAudioStreamer::stringEvent_(FsmEvent event)
{
  switch (event)
  {
  case DeviceConnectedEvent:
    return std::string("DeviceConnectedEvent");
    break;
  case DeviceDisconnectedEvent:
    return std::string("DeviceDisconnectedEvent");
    break;
  case TransportAddedEvent:
    return std::string("TransportAddedEvent");
    break;
  case PlayerAddedEvent:
    return std::string("PlayerAddedEvent");
    break;
  case TransportPendingEvent:
    return std::string("TransportAddedEvent");
    break;
  case TransportActiveEvent:
    return std::string("TransportActiveEvent");
    break;
  case PlayerPlayingEvent:
    return std::string("PlayerPlayingEvent");
    break;
  case PlayerStoppedEvent:
    return std::string("PlayerStoppedEvent");
    break;
  case TransportIdelEvent:
    return std::string("TransportIdelEvent");
    break;
  case PlayerRemovedEvent:
    return std::string("PlayerRemovedEvent");
    break;
  case TransportRemovedEvent:
    return std::string("TransportRemovedEvent");
    break;
  default:
    return std::string("unknown event");
    break;
  }
}

/* ={--------------------------------------------------------------------------
 * @brief FSM eafs
 */

void BleAudioStreamer::entered_(int state)
{
  LOG_MSG("fsm entered: %s", stringState_((FsmState)state).c_str());

  switch (state)
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

  case PlayerRunningState:
    eafPlayerRunningState_();
    break;

  case PlayerStopState:
    eafPlayerStopState_();
    break;

  case TransportIdleState:
    eafTransportIdleState_();
    break;

  case PlayerOffState:
    eafPlayerOffState_();
    break;

  case TransportOffState:
    eafTransportOffState_();
    break;
  }
}

void BleAudioStreamer::exited_(int state)
{
  LOG_MSG("fsm exited: %s", stringState_((FsmState)state).c_str());
}

void BleAudioStreamer::eafDeviceOffState_()
{
  device_path_.clear();
}

void BleAudioStreamer::eafDeviceOnState_()
{
  LOG_MSG("on DeviceOnState");

  device_path_.clear();

  disconnection_timer_ = timer_.add(
      std::chrono::milliseconds(disconnection_timeout_ * 1000), true,
      std::bind(&BleAudioStreamer::onDisconnectionTimerExpired_, this));

  if (disconnection_timer_ <= 0)
    LOG_MSG("failed to add disconnection timer");
}

void BleAudioStreamer::eafTransportOnState_()
{
  LOG_MSG("on TransportOnState");

  transport_path_.clear();

  // TODO: nothing to do?
}

/* metadata must be updated before and have been set
 */
void BleAudioStreamer::eafPlayerOnState_()
{
  LOG_MSG("on PlayerOnState");

  player_path_.clear();
  play_metadata_.clear();

  notify_(MESSAGE_TYPE_METADATA);
}

void BleAudioStreamer::eafPlayerPendingState_()
{
  LOG_MSG("on PlayerPendingState");

  uint16_t not_used1, not_used2;

  // acquire fd
#ifdef USE_HOST_BUILD
  LOG_MSG("host: try to acquire fd from transport proxy");
#else
  LOG_MSG("try to acquire fd from transport proxy");

  // TODO: not clear on what to do if this call fails. the old code has bring up
  // timer and fake player event. what are they?
  transport_proxy_->tryAcquire(transport_path_, fd_, not_used1, not_used2);
#endif
}

void BleAudioStreamer::eafPlayerReadyState_()
{
  LOG_MSG("on PlayerReadyState");

  // start a reader with fd
#ifdef USE_HOST_BUILD
  LOG_MSG("host: create a reader with fd");
#else
  LOG_MSG("create a reader with fd");

  a2dp_sbc_t config{};

  // creates a reader only when gets configuration
  if (!getTransportConfig_(config) && getEndpointConfig_(config))
  {
    reader_ = std::move(
        std::unique_ptr<BleAudioReader>(new BleAudioReader(fd_, config)));
    if (reader_)
      LOG_MSG("failed to create BluAudioReader");

    reader_.set_dispatcher(std::make_shared<CallerThreadDispatcher>());
    reader_.add_observer(this);
  }
  else
  {
    LOG_MSG("no reader created as no configuration is available");
  }
#endif
}

void BleAudioStreamer::eafPlayerRunningState_()
{
  LOG_MSG("PlayerRunningState");

  // stop disconnection timer
  timer_.remove(disconnection_timer_);
  disconnection_timer_ = 0;

  // start refresh timer
  refresh_timer_ =
      timer_.add(std::chrono::milliseconds(PLAY_REFRESH_INTERVAL), false,
                 std::bind(&BleAudioStreamer::onRefreshTimerExpired_, this));

  notify_(MESSAGE_TYPE_SESSION_START);
  notify_(MESSAGE_TYPE_METADATA);
  // notify_(MESSAGE_TYPE_PLAYER_STATE);
  notify_(MESSAGE_TYPE_PROGRESS);
}

void BleAudioStreamer::eafPlayerStopState_()
{
  LOG_MSG("PlayerStopState");

  // stop refresh timer
  timer_.remove(refresh_timer_);
  refresh_timer_ = 0;

  // start disconnection timer
  disconnection_timer_ = timer_.add(
      std::chrono::milliseconds(disconnection_timeout_ * 1000), true,
      std::bind(&BleAudioStreamer::onDisconnectionTimerExpired_, this));

  if (disconnection_timer_ <= 0)
    LOG_MSG("failed to add disconnection timer");

  LOG_MSG("PlayerStopState: started disconnection timer");

  notify_(MESSAGE_TYPE_SESSION_STOP);
}

void BleAudioStreamer::eafTransportIdleState_()
{
  LOG_MSG("TransportIdleState");

  // the client do not use this
  // noify_(MESSAGE_TYPE_TRANSPORT_STATE);
}

void BleAudioStreamer::eafPlayerOffState_()
{
  LOG_MSG("PlayerOffState");

  // clear metadata
  clearPlayData_();

  player_path_.clear();

  // release a reader
  reader_.reset();
}

void BleAudioStreamer::eafTransportOffState_()
{
  LOG_MSG("TransportOffState");

  transport_path_.clear();

  // to signal DeviceDisconnectedEvent
  q_.push(BleAudioMessage(BleAudioMessage::DevicePropertyChangeMsg, "",
                          "Connected", "false"));
}

/* ={--------------------------------------------------------------------------
 * @brief check if device to connect has audio source by checking through UUIDs
 */

bool BleAudioStreamer::checkHasAudioSource_(std::string const &path)
{
#ifndef USE_HOST_BUILD
  // TODO: device proxy
  if (device_proxy_)
  {
    std::vector<std::string> uuids;
    if (device_proxy_->getUUIDs(path, uuids))
    {
      auto it = std::find_if(
          uuids.cbegin(), uuids.cend(), [](std::string const &uuid) {
            return (uuid.substr(0, 8) == BLE_AUDIO_SOURCE_UUID) ? true : false;
          });
      return (it == uuilds.cend()) ? false : true;
    }
  }
#endif

  return false;
}

/* ={--------------------------------------------------------------------------
 * @brief get metadata from player proxy and update streamer with it
 */

void BleAudioStreamer::updateMetadata_()
{
  // TODO: or can use fsm state to check such as if it's in playing state

  if (!player_path_.empty())
  {
#ifdef USE_HOST_BUILD
    LOG_MSG("host: update play metadata");
#else
    player_proxy_->getTrackTitle(player_path_, metadata_.title);
    player_proxy_->getTrackArtist(player_path_, metadata_.artist);
    player_proxy_->getTrackAlbum(player_path_, metadata_.album);
    player_proxy_->getTrackGenre(player_path_, metadata_.genre);
    player_proxy_->getTrackNumberOfTracks(player_path_,
                                          metadata_.number_of_tracks);
    player_proxy_->getTrackNumber(player_path_, metadata_.track_number);
#endif
  }
  else
  {
    LOG_MSG("player path is not set");
  }
}

/* ={--------------------------------------------------------------------------
 * @brief get metadata from player proxy and update streamer with it
 */

void BleAudioStreamer::updatePosition_()
{
  // TODO: or can use fsm state to check such as if it's in playing state

  if (!player_path_.empty())
  {
#ifdef USE_HOST_BUILD
    LOG_MSG("host: update play position");
#else
    player_proxy_->getPosition(player_path_, play_position_.position_);
    player_proxy_->getTrackDuration(player_path_, play_position_.duration_);
#endif
  }
  else
  {
    LOG_MSG("player path is not set");
  }
}

/* ={--------------------------------------------------------------------------
 * @brief get metadata from player proxy and update streamer with it
 */

void BleAudioStreamer::updateState_()
{
  // TODO: or can use fsm state to check such as if it's in playing state

  if (!player_path_.empty())
  {
#ifdef USE_HOST_BUILD
    LOG_MSG("host: update play state");
#else
    player_proxy_->getStatus(player_path_, play_state_);
#endif
  }
  else
  {
    LOG_MSG("player path is not set");
  }
}

/* ={--------------------------------------------------------------------------
 * @brief get metadata from player proxy and update streamer with it
 */

void BleAudioStreamer::clearPlayData_()
{
  play_metadata_.clear();
  play_format_.clear();
  play_position_.clear();
  play_state_.clear();
}

/* ={--------------------------------------------------------------------------
 * @brief get metadata from player proxy and update streamer with it
 */

void BleAudioStreamer::handleStopRequest_()
{
#ifdef USE_HOST_BUILD
  LOG_MSG("host: have a stop request from the client");
#else
  if (!player_proxy_ || player_path_.empty())
  {
    LOG_MGS("player proxy or player path are not avaliable");
    return;
  }

  // TODO: the original code has lots of check and code to get player path from
  // underlying dbus if. really necessary? do not use here.

  player_proxy_->stop(player_path_);

  timer_.add(std::chrono::milliseconds(PLAY_HALT_INTERVAL), true,
             std::bind(&BleAudioStreamer::onStopTimerExpired_, this));
#endif
}

/* ={--------------------------------------------------------------------------
 * @brief update client with messages
 */

void BleAudioStreamer::notify_(MessageType type)
{
  Message msg{};

  switch (type)
  {
  case MESSAGE_TYPE_METADATA:
    msg.type         = type;
    msg.title        = play_metadata_.title_;
    msg.artist       = play_metadata_.artist_;
    msg.album        = play_metadata_.album_;
    msg.number       = play_metadata_.track_number_;
    msg.totalNumbers = play_metadata_.number_of_tracks_;
    msg.genre        = play_metadata_.genre_;
    break;

  case MESSAGE_TYPE_SESSION_START:
    msg.type                   = type;
    msg.audioFormat.type       = play_format_.type_;
    msg.audioFormat.sampleRate = play_format_.rate_;
    msg.audioFormat.channels   = play_format_.channels_;
    break;

  case MESSAGE_TYPE_SESSION_STOP:
    msg.type = type;
    break;

  case MESSAGE_TYPE_BUFFER_FILLED:
    msg.type         = type;
    msg.buffer       = play_buffer_.data_;
    msg.bufferLevel  = play_buffer_.size_;
    msg.sampleNumber = play_buffer_.sample_number_;
    break;

  case MESSAGE_TYPE_PROGRESS:
    msg.type     = type;
    msg.position = play_position_.position_;
    msg.duration = play_position_.duration_;
    break;

    // // TODO: the client do not use this
    // case MESSAGE_TYPE_PLAYER_STATE:
    // msg.type        = type;
    // msg.playerState = play_state_;
    // break;

  default:
    LOG_MSG("unknown AudioStreamer::MessageType, %d", type);
    break;
  }

  // TODO: needs a lock as the previous did?
  observer_(msg, observer_data_);
}

/* ={--------------------------------------------------------------------------
 * @brief get a configuration from transport
 */

bool BleAudioStreamer::getTransportConfig_(a2dp_sbc_t &config)
{
  // TODO: fixed size. then??
  std::vector<char> coll{};

#ifndef USE_HOST_BUILD
  if (transport_proxy_ &&
      !transport_proxy_->getConfiguration(transport_path_, coll))
  {
    LOG_MSG("failed to get config from transport");
    return false;
  }

  if (coll.size() != sizeof(a2dp_sbc_t))
    return false;

  char *pconfig = (char *)(&config);
  for (std::vector<char>::iterator it = coll.begin(); it != coll.end(); ++it)
  {
    *pconfig = *it;
    pconfig++;
  }
#endif

  return true;
}

/* ={--------------------------------------------------------------------------
 * @brief get a configuration from media endpoint
 */

bool BleAudioStreamer::getEndpointConfig_(a2dp_sbc_t &config)
{
  std::vector<std::pair<std::string, std::string>> paths;

#ifndef USE_HOST_BUILD
  if (endpoint_proxy_->getEndpointPaths(paths))
  {
    if (paths.size() > 1)
      AS_LOG_ERROR("ASSUMPTION: guessing that endpoint %s/%s (1 of %lu) is the "
                   "right one",
                   paths[0].first.c_str(), paths[0].second.c_str(),
                   (unsigned long)paths.size());
    return endpoint_proxy_->getConfiguration(paths[0].second, config);
  }
  else
  {
    AS_LOG_ERROR("failed to look up endpoint paths");
  }
#endif

  return false;
}