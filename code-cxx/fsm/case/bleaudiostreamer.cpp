/*
 File    : bleaudiostreamer.cpp
 Created : 10/11/2019

 Copyright (C) 2018 Sky UK. All rights reserved.
*/

#include <functional>
#include <iostream>

#include "bleaudioreader.h"
#include "bleaudiostreamer.h"

#include "bluetooth.h"
#include "bluez_proxy.h"

#include "AS_Diag.h"
extern AS_DIAG::Context *dbusitf_logging_ctx;
#undef AS_DIAG_CONTEXT_DEFAULT
#define AS_DIAG_CONTEXT_DEFAULT (dbusitf_logging_ctx)

// #undef AS_LOG_INFO
// #define AS_LOG_INFO AS_LOG_MIL

/* ={=========================================================================
 * @brief
 */

namespace {

class BluetoothMediaTransportListener : public BluezProxyListener
{
public:
  explicit BluetoothMediaTransportListener(BleAudioStreamer *streamer)
      : m_streamer(streamer){};

private:
  void onObjectAdded(const std::string &path)
  {
    if (m_streamer)
    {
      m_streamer->onTransportAdded(path);
    }
  };

  void onObjectRemoved(const std::string &path)
  {
    if (m_streamer)
    {
      m_streamer->onTransportRemoved(path);
    }
  };

  void onPropertyChange(const std::string &path, const DBusProperty &property)
  {
    if (property.getName() == BLUEZ_MEDIATRANSPORT_PROP_STATE)
    {
      if (m_streamer)
      {
        AS_LOG_MIL("STATUS TRANSPORT value(%s)",
                   property.getValue().asString().c_str());

        m_streamer->onTransportPropertyChange(path, property.getName(),
                                              property.getValue().asString());
      }
    }
  };

  BleAudioStreamer *m_streamer;
};

class BluetoothMediaPlayerListener : public BluezProxyListener
{
public:
  explicit BluetoothMediaPlayerListener(BleAudioStreamer *streamer)
      : m_streamer(streamer){};

private:
  void onObjectAdded(const std::string &path)
  {
    if (m_streamer)
    {
      m_streamer->onPlayerAdded(path);
    }
  };

  void onObjectRemoved(const std::string &path)
  {
    if (m_streamer)
      m_streamer->onPlayerRemoved(path);
  };

  void onPropertyChange(const std::string &path, const DBusProperty &property)
  {
    auto name = property.getName();

    if (m_streamer &&
        (name == "Track" || name == "Position" || name == "Status"))
    {
      if (name == "Status")
      {
        AS_LOG_MIL("STATUS PLAYER value(%s)",
                   property.getValue().asString().c_str());
      }

      m_streamer->onPlayerPropertyChange(path, property.getName(),
                                         property.getValue().asString());
    }
  };

  BleAudioStreamer *m_streamer;
};

class BluetoothDeviceListener : public BluezProxyListener
{
public:
  explicit BluetoothDeviceListener(BleAudioStreamer *streamer)
      : m_streamer(streamer){};

private:
  void onObjectAdded(const std::string &path){
    // AS_LOG_INFO("device added (%s)", path.c_str());
  };

  void onObjectRemoved(const std::string &path){};

  void onPropertyChange(const std::string &path, const DBusProperty &property)
  {
    if (property.getName() == BLUEZ_DEVICE_PROP_CONNECTED)
    {
      if (m_streamer)
      {
        // note that property.getValue().asString().c_str() returns null
        AS_LOG_INFO("STATUS DEVICE : property (%s), value b %d",
                    property.getName().c_str(), property.getValue().asBool());

        m_streamer->onDevicePropertyChange(path, property.getName(),
                                           property.getValue().asBool());
      }
    }
  };

  BleAudioStreamer *m_streamer;
};

const char *const endpointPathTable[] = {
  "/EthanBluetooth/A2DPEndpoint1", "/EthanBluetooth/A2DPEndpoint2", nullptr};

#ifdef DEBUG_BUFFER
std::string stringMessage(int message)
{
  switch (message)
  {
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
#endif

// ref:
// https://www.bluetooth.org/en-us/specification/assigned-numbers/service-discovery
const std::string BLE_AUDIO_SOURCE_UUID{"0000110a"};

// in ms
const size_t PLAY_REFRESH_INTERVAL{1000};

// in ms
const size_t PLAY_HALT_INTERVAL{3500};

// in ms
// const size_t PLAY_BRINGUP_INTERVAL{2000};
const size_t PLAY_BRINGUP_INTERVAL{500};

std::shared_ptr<BleAudioStreamer> singleton;

} // namespace

/* ={==========================================================================
 @brief :
 BluetoothApi::Streamer interfaces

 @note  :
*/
void BluetoothApi::Streamer::createInstance()
{
  if (!singleton)
  {
    AS_LOG_INFO("BleAudioStreamer created");
    singleton = std::make_shared<BleAudioStreamer>();
  }
}

void BluetoothApi::Streamer::destroyInstance()
{
  if (singleton)
  {
    AS_LOG_INFO("BleAudioStreamer deleted");
    singleton = nullptr;
  }
}

BluetoothApi::Streamer *BluetoothApi::Streamer::instance()
{
  // return singleton;
  return singleton ? singleton.get() : nullptr;
}

void BleAudioStreamer::setBluetoothTimeout(uint32_t timeout)
{
  std::lock_guard<std::mutex> lock(m_m);

  AS_LOG_INFO("timeout to set (%d) secs", timeout);

  m_disconnection_timeout = timeout;
}

void BleAudioStreamer::releaseBuffer(void *buffer)
{
#ifdef USE_NEW
  delete[](char *) buffer;
#else
  free(buffer);
#endif
}

void BleAudioStreamer::setMessageHandler(MessageHandler handler, void *user)
{
  std::lock_guard<std::mutex> lock(m_m);

  m_observer      = handler;
  m_observer_data = user;
}

void BleAudioStreamer::stop()
{
  std::lock_guard<std::mutex> lock(m_m);

  AS_LOG_MIL("stop called");

  m_q.push(BleAudioMessage(BleAudioMessage::ClientStopRequestMsg));
}

/* ={==========================================================================
 @brief :
  construct listeners and dbus proxies which provides access to dbus and signals
  from bluex audio

 @note  :

*/
BleAudioStreamer::BleAudioStreamer(std::string const &name)
    : m_running(true), m_t(std::thread(&BleAudioStreamer::doWork_, this, name))
{
  // register endpoints with media endpoint proxy.
  // Assumption: we're using the first (and only) HCI in the system

  BluetoothMediaEndpoint *endpoint_proxy =
    Bluetooth::getInstance()->getMediaEndpoints();

  std::vector<std::string> endpoints = endpoint_proxy->getObjectPaths();
  if (endpoints.size())
  {
    for (int i = 0; endpointPathTable[i] != NULL; i++)
    {
      if (!endpoint_proxy->registerEndpoint(endpoints[0], endpointPathTable[i]))
      {
        AS_LOG_ERROR("failed to register media endpoint (%s)",
                     endpointPathTable[i]);
      }
    }
  }
  else
  {
    AS_LOG_ERROR("no media endpoint proxy available");
  }

  m_device_proxy = Bluetooth::getInstance()->getDevices();
  if (m_device_proxy)
  {
    m_device_listener = std::move(
      std::unique_ptr<BluezProxyListener>(new BluetoothDeviceListener(this)));
    if (m_device_listener)
      m_device_proxy->addListener(m_device_listener.get());
    else
      AS_LOG_ERROR("failed to create BluetoothDeviceListener");
  }
  else
  {
    AS_LOG_ERROR("failed to get BluetoothDevice");
  }

  m_player_proxy = Bluetooth::getInstance()->getMediaPlayers();
  if (m_player_proxy)
  {
    m_player_listener = std::move(std::unique_ptr<BluezProxyListener>(
      new BluetoothMediaPlayerListener(this)));
    if (m_player_listener)
      m_player_proxy->addListener(m_player_listener.get());
    else
      AS_LOG_ERROR("failed to create MediaPlayerListener");
  }
  else
  {
    AS_LOG_ERROR("failed to obtain BluetoothMediaPlayer");
  }

  m_transport_proxy = Bluetooth::getInstance()->getMediaTransports();
  if (m_transport_proxy)
  {
    m_transport_listener = std::move(std::unique_ptr<BluezProxyListener>(
      new BluetoothMediaTransportListener(this)));
    if (m_transport_listener)
      m_transport_proxy->addListener(m_transport_listener.get());
    else
      AS_LOG_ERROR("failed to create MediaTransportListener");
  }
  else
  {
    AS_LOG_ERROR("failed to obtain BluetoothMediaTransport");
  }

  m_endpoint_proxy = Bluetooth::getInstance()->getMediaEndpoints();
  if (!m_endpoint_proxy)
  {
    AS_LOG_ERROR("failed to obtain BluetoothMediaEndpoint");
  }

  setupStateMachine_();

  AS_LOG_INFO("BleAudioStreamer::BleAudioStreamer");
}

BleAudioStreamer::~BleAudioStreamer()
{
  if (m_device_proxy && m_device_listener)
    m_device_proxy->removeListener(m_device_listener.get());

  if (m_player_proxy && m_player_listener)
    m_player_proxy->removeListener(m_player_listener.get());

  if (m_transport_proxy && m_transport_listener)
    m_transport_proxy->removeListener(m_transport_listener.get());

  BluetoothMediaEndpoint *endpoint_proxy =
    Bluetooth::getInstance()->getMediaEndpoints();
  if (endpoint_proxy)
  {
    for (int i = 0; endpointPathTable[i] != NULL; i++)
      endpoint_proxy->unregisterEndpoint(endpointPathTable[i]);
  }

  m_q.push(BleAudioMessage(BleAudioMessage::ClientQuitRequestMsg));
  m_t.join();

  AS_LOG_INFO("BleAudioStreamer::~BleAudioStreamer");
}

/* ={==========================================================================
 @brief :
  debugging purpose

 @note  :

*/
void BleAudioStreamer::postMessage(int message)
{
  std::lock_guard<std::mutex> lock(m_m);

  m_q.push(BleAudioMessage(message));
}

/* ={==========================================================================
 @brief :
  Listener callbacks which gets called from Bluetooth proxies. There is no
  locking on this callbacks since there is single caller and these send it to
  queue.

 @note  :

*/
void BleAudioStreamer::onTransportAdded(std::string const &path)
{
  m_q.push(BleAudioMessage(BleAudioMessage::TransportAddedMsg, path));
}

void BleAudioStreamer::onTransportRemoved(std::string const &path)
{
  m_q.push(BleAudioMessage(BleAudioMessage::TransportRemovedMsg, path));
}

void BleAudioStreamer::onTransportPropertyChange(std::string const &path,
                                                 std::string const &property,
                                                 std::string const &value)
{
  m_q.push(BleAudioMessage(BleAudioMessage::TransportPropertyChangeMsg, path,
                           property, value));
}

void BleAudioStreamer::onPlayerAdded(std::string const &path)
{
  m_q.push(BleAudioMessage(BleAudioMessage::PlayerAddedMsg, path));
}

void BleAudioStreamer::onPlayerRemoved(std::string const &path)
{
  m_q.push(BleAudioMessage(BleAudioMessage::PlayerRemovedMsg, path));
}

void BleAudioStreamer::onPlayerPropertyChange(std::string const &path,
                                              std::string const &property,
                                              std::string const &value)
{

  m_q.push(BleAudioMessage(BleAudioMessage::PlayerPropertyChangeMsg, path,
                           property, value));
}

void BleAudioStreamer::onDevicePropertyChange(std::string const &path,
                                              std::string const &property,
                                              bool value)
{
  m_q.push(BleAudioMessage(BleAudioMessage::DevicePropertyChangeMsg, path,
                           property, std::string(), value));
}

/* ={==========================================================================
 @brief :
  Observer callbacks for events from BleAudioReader

 @note  :

*/
void BleAudioStreamer::onFormatChanged(FormatType type, uint32_t rate,
                                       uint32_t channels)
{
  AS_LOG_INFO("onFormatChange: type(%d), rate(%d), channels(%d)", type, rate,
              channels);

  m_play_format.m_type     = type;
  m_play_format.m_rate     = rate;
  m_play_format.m_channels = channels;

  m_q.push(BleAudioMessage(BleAudioMessage::PlayerFormatChangeMsg));
}

void BleAudioStreamer::onBufferReady(char *buffer, size_t size, size_t samples)
{
  // make a copy of the received buffer and which will be notified to the client
  // The client has to use releaseBuffer(void *) when finishes with it

#ifdef USE_NEW
  char *chunk = new char[size];
#else
  char *chunk = (char *)malloc(size);
#endif

  if (chunk)
  {
    memcpy(chunk, buffer, size);

    BleAudioBuffer play_buffer;

    play_buffer.m_data          = chunk;
    play_buffer.m_size          = size;
    play_buffer.m_sample_number = samples;

    m_q.push(
      BleAudioMessage(BleAudioMessage::PlayerBufferReadyMsg, play_buffer));
  }
}

// the comment from the legacy code:
//
// Transport is up and passing audible data. We start a timeout now to catch
// the scenario where the transport comes up but the player doesn't show up (or
// does not change state) in a timely manner.

void BleAudioStreamer::onSilence(bool silent)
{
  AS_LOG_MIL("onSilence: slient(%s)", silent ? "true" : "false");

  // noisy
  if ((silent == false) && !m_fsm.inState(PlayerRunningState))
  {
    AS_LOG_MIL(
      "onSilence: get noisy while not playing. SHOULD USE BRING UP TIMER");

    m_timer.add(std::chrono::milliseconds(PLAY_BRINGUP_INTERVAL), true,
                std::bind(&BleAudioStreamer::onBringUpTimerExpired_, this));
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  timer callbacks which are for timers used in BleAudioStreamer

 @note  :

*/
bool BleAudioStreamer::onDisconnectionTimerExpired_()
{
  if (!m_device_proxy)
  {
    // error
    AS_LOG_ERROR("device proxy must be available");
    return false;
  }

  AS_LOG_MIL("disconnection timer expired and dvice (%s) disconnected",
             m_device_path.c_str());

  if (false == m_device_proxy->disconnect(m_device_path))
  {
    AS_LOG_ERROR("disconnecting device failed");
  }

  m_disconnection_timer = 0;

  AS_LOG_INFO("device_path cleared");
  m_device_path.clear();

  return true;
}

bool BleAudioStreamer::onRefreshTimerExpired_()
{
  if (!m_player_proxy || m_player_path.empty())
  {
    // error
    AS_LOG_ERROR("player proxy and player path must be available");
    return false;
  }

  AS_LOG_INFO("refresh timer expired and refresh player");
  m_player_proxy->refresh(m_player_path);

  return true;
}

bool BleAudioStreamer::onStopTimerExpired_()
{
  if (!m_device_proxy || m_device_path.empty())
  {
    AS_LOG_ERROR("device proxy and device path are not available");
    return false;
  }

  AS_LOG_MIL(
    "stop timer expired, stop device, reset reader, pathes and timers");
  m_device_proxy->disconnect(m_device_path);

  // clear metadata
  clearPlayData_();

  m_device_path.clear();
  m_player_path.clear();
  m_transport_path.clear();

  // release a reader
  m_reader.reset();

  // stop refresh timer
  if (m_refresh_timer > 0)
    m_timer.remove(m_refresh_timer);
  m_refresh_timer = 0;

  // stop disconnection timer
  if (m_disconnection_timer > 0)
    m_timer.remove(m_disconnection_timer);
  m_disconnection_timer = 0;

  return true;
}

bool BleAudioStreamer::onBringUpTimerExpired_()
{
  AS_LOG_MIL("bring up timer expired and send playing state change");

  onPlayerPropertyChange(m_player_path, "Status", "playing");

  return true;
}

/* ={--------------------------------------------------------------------------
 @brief :
  main thread function of BleAudioStreamer

 @note  :

*/
void BleAudioStreamer::doWork_(std::string const &name)
{
  pthread_setname_np(pthread_self(), name.c_str());

  while (m_running)
  {
    auto message = m_q.wait_and_pop();

#ifdef DEBUG_BUFFER
    if (message.m_type != BleAudioMessage::PlayerBufferReadyMsg)
      AS_LOG_INFO("message is (%s)", stringMessage(message.m_type).c_str());
#endif

    switch (message.m_type)
    {
    case BleAudioMessage::DevicePropertyChangeMsg:
    {
      if (message.m_property == "Connected")
      {
        if (message.m_flag == true)
        {
          m_device_path = message.m_path;

          // start fsm only when it has audio source
          if (checkHasAudioSource_(m_device_path))
          {
            AS_LOG_MIL("device is conneced, (%s)", m_device_path.c_str());
            m_fsm.postEvent(DeviceConnectedEvent);
          }
        }
        else if (message.m_flag == false)
        {
          AS_LOG_MIL("device is disconneced, (%s)", m_device_path.c_str());
          m_fsm.postEvent(DeviceDisconnectedEvent);
        }
        else
        {
          AS_LOG_ERROR(
            "DevicePropertyChangeMsg got unknown (%d) property value",
            message.m_flag);
        }
      }
      else
      {
        AS_LOG_ERROR("DevicePropertyChangeMsg got unknown (%s) property",
                     message.m_property.c_str());
      }
    }
    break;

    case BleAudioMessage::TransportAddedMsg:
    {
      if (m_transport_path.empty())
      {
        m_transport_path = message.m_path;

        AS_LOG_MIL("transport is added (%s)", m_transport_path.c_str());
        m_fsm.postEvent(TransportAddedEvent);
      }
      else
      {
        AS_LOG_ERROR(
          "transport added received but transport path (%s) is not empty",
          m_transport_path.c_str());
      }
    }
    break;

    case BleAudioMessage::TransportRemovedMsg:
    {
      AS_LOG_MIL("transport is removed (%s)", m_transport_path.c_str());
      m_fsm.postEvent(TransportRemovedEvent);
    }
    break;

    case BleAudioMessage::TransportPropertyChangeMsg:
    {
      if (message.m_property == "State")
      {
        if (message.m_value == "idle")
        {
          AS_LOG_INFO("transport is idle");
          m_fsm.postEvent(TransportIdelEvent);
        }
        else if (message.m_value == "pending")
        {
          AS_LOG_INFO("transport is pending");
          m_fsm.postEvent(TransportPendingEvent);
        }
        else if (message.m_value == "active")
        {
          AS_LOG_INFO("transport is active");
          m_fsm.postEvent(TransportActiveEvent);
        }
        else
        {
          AS_LOG_ERROR(
            "TransportPropertyChangeMsg got unknown (%s) property value",
            message.m_value.c_str());
        }
      }
      else
      {
        AS_LOG_ERROR("TransportPropertyChangeMsg got unknown (%s) property",
                     message.m_property.c_str());
      }
    }
    break;

    case BleAudioMessage::PlayerPropertyChangeMsg:
    {
      if (message.m_property == "Track")
      {
        // build metadata and notify to client
        if (m_fsm.inState(PlayerRunningState) && updateMetadata_())
          notify_(MESSAGE_TYPE_METADATA);
      }
      else if (message.m_property == "Position")
      {
        // build pos update and notify to AS
        if (m_fsm.inState(PlayerRunningState) && updatePosition_())
          notify_(MESSAGE_TYPE_PROGRESS);
      }
      else if (message.m_property == "Status")
      {
        // updateState();
        m_play_state = message.m_value;

        AS_LOG_MIL("STATUS PLAYER message value(%s)", message.m_value.c_str());

        // move to play when gets playing state and not in playing state
        //
        // note that the format change comes either before or after playing
        // state and have to wait for the format ready.

        if (m_play_state == "playing" && !m_fsm.inState(PlayerRunningState) &&
            m_play_format.isValid())
        {
          AS_LOG_MIL("player is not playing and post fsm playing event");
          AS_LOG_MIL("audio format type(%d), rate(%u), channel(%u)",
                     m_play_format.m_type, m_play_format.m_rate,
                     m_play_format.m_channels);
          m_fsm.postEvent(PlayerPlayingEvent);
        }

        // move to stop when gets non-playing states while in playing state
        if (m_play_state != "playing" && m_fsm.inState(PlayerRunningState))
        {
          AS_LOG_MIL("player is playing and post fsm stopped event");
          m_fsm.postEvent(PlayerStoppedEvent);
        }
      }
      else
      {
        AS_LOG_ERROR("PlayerPropertyChangeMsg got unknown (%s) property",
                     message.m_property.c_str());
      }
    }
    break;

    case BleAudioMessage::PlayerAddedMsg:
    {
      if (m_player_path.empty())
      {
        m_player_path = message.m_path;

        AS_LOG_INFO("player is added, (%s)", m_player_path.c_str());
        m_fsm.postEvent(PlayerAddedEvent);
      }
      else
      {
        AS_LOG_ERROR(
          "PlayerAddedMsg receive but player path, (%s), is not empty",
          m_player_path.c_str());
      }
    }
    break;

    case BleAudioMessage::PlayerRemovedMsg:
    {
      AS_LOG_INFO("player is removed, (%s)", m_player_path.c_str());
      m_fsm.postEvent(PlayerRemovedEvent);
    }
    break;

    case BleAudioMessage::PlayerBufferReadyMsg:
    {
      if (m_fsm.inState(PlayerRunningState))
      {
        notify_(MESSAGE_TYPE_BUFFER_FILLED, message);
      }
      else
      {
        delete[](char *) message.m_play_buffer.m_data;
      }
    }
    break;

    case BleAudioMessage::PlayerFormatChangeMsg:
    {
      if (m_play_state != "playing" && !m_fsm.inState(PlayerRunningState) &&
          m_play_format.isValid())
      {
        AS_LOG_MIL("player is not playing and post fsm playing event");
        AS_LOG_MIL("audio format type(%d), rate(%u), channel(%u)",
                   m_play_format.m_type, m_play_format.m_rate,
                   m_play_format.m_channels);

        m_fsm.postEvent(PlayerPlayingEvent);
      }
    }
    break;

    case BleAudioMessage::ClientStopRequestMsg:
    {
      AS_LOG_INFO("received stop request from the client on player (%s)",
          m_player_path.c_str());

      handleStopRequest_();
    }
    break;

    case BleAudioMessage::ClientQuitRequestMsg:
      m_running = false;
      break;

    default:
      AS_LOG_ERROR("unknown message is %d", message.m_type);
      break;
    } // switch
  }   // while
}

/* ={--------------------------------------------------------------------------
 @brief :
  defines FSM of BleAudioStreamer and state transitions

 @note  :

*/
void BleAudioStreamer::setupStateMachine_()
{
  m_fsm.addState(DeviceOffState, "DeviceOffState");
  m_fsm.addState(RunningSuperState, "RunningSuperState");

  m_fsm.addState(RunningSuperState, DeviceOnState, "DeviceOnState");
  m_fsm.addState(RunningSuperState, TransportOnState, "TransportOnState");
  m_fsm.addState(RunningSuperState, PlayerOnState, "PlayerOnState");
  m_fsm.addState(RunningSuperState, PlayerPendingState, "PlayerPendingState");
  m_fsm.addState(RunningSuperState, PlayerReadyState, "PlayerReadyState");
  m_fsm.addState(RunningSuperState, PlayerRunningState, "PlayerRunningState");
  m_fsm.addState(RunningSuperState, PlayerStopState, "PlayerStopState");
  m_fsm.addState(RunningSuperState, TransportIdleState, "TransportIdelEvent");
  m_fsm.addState(RunningSuperState, PlayerOffState, "PlayerOffState");
  m_fsm.addState(RunningSuperState, TransportOffState, "TransportOffState");

  m_fsm.addTransition(RunningSuperState, RunningStopEvent, DeviceOffState);
  m_fsm.addTransition(DeviceOffState, DeviceConnectedEvent, DeviceOnState);
  m_fsm.addTransition(DeviceOffState, TransportAddedEvent, TransportOnState);

  m_fsm.addTransition(DeviceOnState, TransportAddedEvent, TransportOnState);
  m_fsm.addTransition(TransportOnState, PlayerAddedEvent, PlayerOnState);
  m_fsm.addTransition(PlayerOnState, TransportPendingEvent, PlayerPendingState);

  m_fsm.addTransition(DeviceOnState, PlayerAddedEvent, PlayerOnState);
  m_fsm.addTransition(PlayerOnState, TransportAddedEvent, TransportOnState);
  m_fsm.addTransition(TransportOnState, TransportPendingEvent,
                      PlayerPendingState);

  m_fsm.addTransition(PlayerPendingState, TransportActiveEvent,
                      PlayerReadyState);

  m_fsm.addTransition(PlayerReadyState, PlayerPlayingEvent, PlayerRunningState);
  m_fsm.addTransition(PlayerRunningState, PlayerStoppedEvent, PlayerStopState);

  m_fsm.addTransition(PlayerStopState, TransportIdelEvent, TransportIdleState);
  m_fsm.addTransition(PlayerStopState, PlayerPlayingEvent, PlayerRunningState);

  m_fsm.addTransition(TransportIdleState, PlayerRemovedEvent, PlayerOffState);
  m_fsm.addTransition(TransportIdleState, TransportPendingEvent,
                      PlayerPendingState);

  m_fsm.addTransition(PlayerOffState, TransportRemovedEvent, TransportOffState);
  m_fsm.addTransition(TransportOffState, DeviceDisconnectedEvent,
                      DeviceOffState);

  m_fsm.connect(
    std::bind(&BleAudioStreamer::entered_, this, std::placeholders::_1),
    std::bind(&BleAudioStreamer::exited_, this, std::placeholders::_1));

  m_fsm.setTransistionLogLevel(true);
  m_fsm.setObjectName("BleAudioStreamer");

  m_fsm.setInitialState(DeviceOffState);
  m_fsm.start();
}

std::string BleAudioStreamer::stringState_(FsmState state)
{
  switch (state)
  {
  case DeviceOffState:
    return std::string("DeviceOffState");
    break;
  case RunningSuperState:
    return std::string("RunningSuperState");
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
  case RunningStopEvent:
    return std::string("RunningStopEvent");
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
 @brief :
  FSM eafs

 @note  :

*/
void BleAudioStreamer::entered_(int state)
{
  AS_LOG_MIL("fsm entered: (%s)", stringState_((FsmState)state).c_str());

  switch (state)
  {
  case DeviceOnState:
    onEnterDeviceOnState_();
    break;

  case DeviceOffState:
    onEnterDeviceOffState_();
    break;

  case TransportOnState:
    onEnterTransportOnState_();
    break;

  case PlayerOnState:
    onEnterPlayerOnState_();
    break;

  case PlayerPendingState:
    onEnterPlayerPendingState_();
    break;

  case PlayerReadyState:
    onEnterPlayerReadyState_();
    break;

  case PlayerRunningState:
    onEnterPlayerRunningState_();
    break;

  case PlayerStopState:
    onEnterPlayerStopState_();
    break;

  case TransportIdleState:
    onEnterTransportIdleState_();
    break;

  case PlayerOffState:
    onEnterPlayerOffState_();
    break;

  case TransportOffState:
    onEnterTransportOffState_();
    break;
  }
}

void BleAudioStreamer::exited_(int state)
{
  AS_LOG_MIL("fsm exited: %s", stringState_((FsmState)state).c_str());

  switch (state)
  {
  case RunningSuperState:
    onExitRunningSuperState_();
    break;
  }
}

void BleAudioStreamer::onEnterDeviceOffState_()
{
  AS_LOG_INFO("DeviceOffState");

  // clear metadata
  clearPlayData_();
}

void BleAudioStreamer::onEnterDeviceOnState_()
{
  AS_LOG_INFO("DeviceOnState");
}

void BleAudioStreamer::onEnterTransportOnState_()
{
  AS_LOG_INFO("TransportOnState");

  m_disconnection_timer = m_timer.add(
    std::chrono::milliseconds(m_disconnection_timeout * 1000), true,
    std::bind(&BleAudioStreamer::onDisconnectionTimerExpired_, this));

  if (m_disconnection_timer <= 0)
    AS_LOG_ERROR("failed to add disconnection timer");
  else
    AS_LOG_INFO("TransportOnState: disconnection timer is added");
}

void BleAudioStreamer::onEnterPlayerOnState_()
{
  AS_LOG_INFO("PlayerOnState");
}

void BleAudioStreamer::onEnterPlayerPendingState_()
{
  AS_LOG_INFO("PlayerPendingState");

  uint16_t not_used1, not_used2;

  // clear metadata
  clearPlayData_();

  // acquire fd
  auto result =
    m_transport_proxy->tryAcquire(m_transport_path, m_fd, not_used1, not_used2);
  if (result)
    AS_LOG_INFO("acquired fd(%d) on (%s)", m_fd, m_transport_path.c_str());
  else
    AS_LOG_ERROR("failed to get fd");
}

void BleAudioStreamer::onEnterPlayerReadyState_()
{
  AS_LOG_INFO("PlayerReadyState");

  a2dp_sbc_t config{};

  // creates a reader only when gets configuration
  auto configured =
    (getTransportConfig_(config) ? true : getEndpointConfig_(config));

  if (configured)
  {
    m_reader = std::move(
      std::unique_ptr<BleAudioReader>(new BleAudioReader(m_fd, config)));

    if (!m_reader)
    {
      AS_LOG_ERROR("failed to create BluAudioReader");
      return;
    }

    AS_LOG_INFO("reader created with fd(%d) and add observers", m_fd);

    // the reader owns fd and make it clear
    m_fd = -1;

    m_reader->setDispatcher(
      std::make_shared<AICommon::CallerThreadDispatcher>());
    m_reader->addObserver(shared_from_this());
  }
  else
  {
    AS_LOG_ERROR("no reader created as no configuration is available");
  }
}

void BleAudioStreamer::onEnterPlayerRunningState_()
{
  AS_LOG_INFO("PlayerRunningState");

  // stop disconnection timer
  AS_LOG_INFO("stop disconnection timer");
  m_timer.remove(m_disconnection_timer);
  m_disconnection_timer = 0;

  // start refresh timer
  AS_LOG_INFO("start refresh timer");
  m_refresh_timer =
    m_timer.add(std::chrono::milliseconds(PLAY_REFRESH_INTERVAL), false,
                std::bind(&BleAudioStreamer::onRefreshTimerExpired_, this));

  notify_(MESSAGE_TYPE_SESSION_START);
  notify_(MESSAGE_TYPE_METADATA);
  notify_(MESSAGE_TYPE_PROGRESS);
}

void BleAudioStreamer::onEnterPlayerStopState_()
{
  AS_LOG_INFO("PlayerStopState");

  // stop refresh timer
  m_timer.remove(m_refresh_timer);
  m_refresh_timer = 0;

  // start disconnection timer
  AS_LOG_INFO("started disconnection timer (%d)", m_disconnection_timeout);
  m_disconnection_timer = m_timer.add(
    std::chrono::milliseconds(m_disconnection_timeout * 1000), true,
    std::bind(&BleAudioStreamer::onDisconnectionTimerExpired_, this));

  if (m_disconnection_timer <= 0)
    AS_LOG_ERROR("failed to add disconnection timer");

  notify_(MESSAGE_TYPE_SESSION_STOP);
}

void BleAudioStreamer::onEnterTransportIdleState_()
{
  AS_LOG_INFO("TransportIdleState");

  // release a reader
  m_reader.reset();
}

void BleAudioStreamer::onEnterPlayerOffState_()
{
  AS_LOG_INFO("PlayerOffState");

  // clear metadata
  clearPlayData_();

  m_player_path.clear();
}

void BleAudioStreamer::onEnterTransportOffState_()
{
  AS_LOG_INFO("TransportOffState");

  AS_LOG_INFO("m_transport_path claered");
  m_transport_path.clear();

  // to signal DeviceDisconnectedEvent
  m_q.push(BleAudioMessage(BleAudioMessage::DevicePropertyChangeMsg, "",
                           "Connected", "false"));
}

void BleAudioStreamer::onExitRunningSuperState_()
{
  AS_LOG_INFO("RunningSuperState");

  handleStopRequest_();
}

/* ={--------------------------------------------------------------------------
 @brief :
  check if device to connect has audio source by checking through UUIDs

 @note  :

*/
bool BleAudioStreamer::checkHasAudioSource_(std::string const &path)
{
  if (m_device_proxy && !path.empty())
  {
    std::vector<std::string> uuids;
    if (m_device_proxy->getUUIDs(path, uuids))
    {
      auto it =
        std::find_if(uuids.cbegin(), uuids.cend(), [](std::string const &uuid) {
          return (uuid.substr(0, 8) == BLE_AUDIO_SOURCE_UUID) ? true : false;
        });
      return (it == uuids.cend()) ? false : true;
    }
  }

  return false;
}

/* ={--------------------------------------------------------------------------
 @brief :
  get metadata from player proxy and update streamer with it

 @note  :

*/
bool BleAudioStreamer::updateMetadata_()
{
  if (!m_player_path.empty())
  {
    std::string title{};

    m_player_proxy->getTrackTitle(m_player_path, title);

    if (title == m_play_metadata.m_title)
      return false;

    m_play_metadata.m_title = title;
    AS_LOG_INFO("updated title (%s)", m_play_metadata.m_title.c_str());

    m_player_proxy->getTrackArtist(m_player_path, m_play_metadata.m_artist);
    m_player_proxy->getTrackAlbum(m_player_path, m_play_metadata.m_album);
    m_player_proxy->getTrackGenre(m_player_path, m_play_metadata.m_genre);
    m_player_proxy->getTrackNumberOfTracks(m_player_path,
                                           m_play_metadata.m_number_of_tracks);
    m_player_proxy->getTrackNumber(m_player_path,
                                   m_play_metadata.m_track_number);

    return true;
  }
  else
  {
    AS_LOG_ERROR("player path is not set");
    return false;
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  update position of play with the current values

 @note  :

*/
bool BleAudioStreamer::updatePosition_()
{
  if (!m_player_path.empty())
  {
    BleAudioPosition position{};

    m_player_proxy->getPosition(m_player_path, position.m_position);
    m_player_proxy->getTrackDuration(m_player_path, position.m_duration);

    if (position == m_play_position)
      return false;

    m_play_position = position;

    return true;
  }
  else
  {
    AS_LOG_ERROR("player path is not set");
    return false;
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  clear all internal data related to current audio play

 @note  :

*/
void BleAudioStreamer::clearPlayData_()
{
  m_play_metadata.clear();
  m_play_format.clear();
  m_play_position.clear();
  m_play_state.clear();
}

/* ={--------------------------------------------------------------------------
 @brief :
  handle stop request from the client and make a halt timer

 @note  :

*/
void BleAudioStreamer::handleStopRequest_()
{
  if (m_player_proxy && !m_player_path.empty())
  {
    AS_LOG_INFO("player proxy or player path are set. request proxy to stop");
    m_player_proxy->stop(m_player_path);
  }
}

/* ={--------------------------------------------------------------------------
 @brief :
  notify observers with events and data which are of interest

 @note  :

*/
void BleAudioStreamer::notify_(MessageType type, BleAudioMessage const &message)
{
  Message msg{};

  switch (type)
  {
  case MESSAGE_TYPE_METADATA:
    AS_LOG_MIL("MESSAGE_TYPE_METADATA: title (%s)",
               m_play_metadata.m_title.c_str());
    msg.type         = type;
    msg.title        = m_play_metadata.m_title;
    msg.artist       = m_play_metadata.m_artist;
    msg.album        = m_play_metadata.m_album;
    msg.number       = m_play_metadata.m_track_number;
    msg.totalNumbers = m_play_metadata.m_number_of_tracks;
    msg.genre        = m_play_metadata.m_genre;
    break;

  case MESSAGE_TYPE_SESSION_START:
    AS_LOG_MIL("MESSAGE_TYPE_SESSION_START: type(%d), rate(%u), channel(%u)",
               m_play_format.m_type, m_play_format.m_rate,
               m_play_format.m_channels);

    msg.type                   = type;
    msg.audioFormat.type       = m_play_format.m_type;
    msg.audioFormat.sampleRate = m_play_format.m_rate;
    msg.audioFormat.channels   = m_play_format.m_channels;
    break;

  case MESSAGE_TYPE_SESSION_STOP:
    AS_LOG_MIL("MESSAGE_TYPE_SESSION_STOP");
    msg.type = type;
    break;

  case MESSAGE_TYPE_BUFFER_FILLED:

    // #ifdef DEBUG_BUFFER
    if (message.m_play_buffer.m_size != 0)
    {
      AS_LOG_INFO("MESSAGE_TYPE_BUFFER_FILLED. 0x%p, %u, %u",
                  message.m_play_buffer.m_data, message.m_play_buffer.m_size,
                  message.m_play_buffer.m_sample_number);
    }
    // #endif

    msg.type         = type;
    msg.buffer       = message.m_play_buffer.m_data;
    msg.bufferLevel  = message.m_play_buffer.m_size;
    msg.sampleNumber = message.m_play_buffer.m_sample_number;
    break;

  case MESSAGE_TYPE_PROGRESS:
    AS_LOG_INFO("MESSAGE_TYPE_PROGRESS: pos(%d), duration(%d)",
                m_play_position.m_position, m_play_position.m_duration);

    msg.type     = type;
    msg.position = m_play_position.m_position;
    msg.duration = m_play_position.m_duration;
    break;

  default:
    AS_LOG_ERROR("unknown AudioStreamer::MessageType, %d", type);
    break;
  }

  m_observer(msg, m_observer_data);
}

/* ={--------------------------------------------------------------------------
 @brief :
  get audio configuration from Transport and Endpoint interface

 @note  :

*/
bool BleAudioStreamer::getTransportConfig_(a2dp_sbc_t &config)
{
  std::vector<char> coll{};

  if (m_transport_proxy &&
      !m_transport_proxy->getConfiguration(m_transport_path, coll))
  {
    AS_LOG_ERROR("failed to get config from transport");
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

  AS_LOG_INFO("have got transport config");
  return true;
}

bool BleAudioStreamer::getEndpointConfig_(a2dp_sbc_t &config)
{
  std::vector<std::pair<std::string, std::string>> paths;

  if (m_endpoint_proxy->getEndpointPaths(paths))
  {
    if (paths.size() > 1)
      AS_LOG_INFO(
        "ASSUMPTION: guessing that endpoint (%s)/(%s) (1 of %lu) is the "
        "right one",
        paths[0].first.c_str(), paths[0].second.c_str(),
        (unsigned long)paths.size());

    auto result = m_endpoint_proxy->getConfiguration(paths[0].second, config);
    if (!result)
    {
      AS_LOG_ERROR("failed to get endpoint config");
    }

    return result;
  }
  else
  {
    AS_LOG_ERROR("failed to look up endpoint paths");
  }

  return false;
}
