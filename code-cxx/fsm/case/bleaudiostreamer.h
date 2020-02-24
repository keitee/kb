/*
 File    : bleaudiostreamer.h
 Created : 10/11/2019

 Copyright (C) 2018 Sky UK. All rights reserved.
*/
#ifndef BLEAUDIOSTREAMER_H
#define BLEAUDIOSTREAMER_H

#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "BluetoothStreamer.h"
#include <CallerThreadDispatcher.h>
#include <Common/Notifier.h>
#include <Common/Observer.h>

#include "bluetooth_device.h"
#include "bluetooth_media_endpoint.h"
#include "bluetooth_media_player.h"
#include "bluetooth_media_transport.h"

#include "bleaudioreader.h"
#include "bleaudioreaderevent.h"
#include "eventqueue.h"
#include "statemachine.h"
#include "timer_queue.h"

struct BleAudioMetadata
{
  bool operator==(BleAudioMetadata const &rhs) const
  {
    if ((m_title != rhs.m_title) || (m_artist != rhs.m_artist) ||
        (m_album != rhs.m_album) || (m_track_number != rhs.m_track_number) ||
        (m_number_of_tracks != rhs.m_number_of_tracks))
    {
      return false;
    }

    return true;
  }

  bool operator!=(BleAudioMetadata const &rhs) const { return !(*this == rhs); }

  void clear()
  {
    m_title.clear();
    m_artist.clear();
    m_album.clear();
    m_track_number     = 0;
    m_number_of_tracks = 0;
    m_genre.clear();
  }

  std::string m_title{};
  std::string m_artist{};
  std::string m_album{};
  uint32_t m_track_number{};
  uint32_t m_number_of_tracks{};
  std::string m_genre{};
};

struct BleAudioPosition
{
  bool operator==(BleAudioPosition const &rhs) const
  {
    return ((m_duration == rhs.m_duration) && (m_position == rhs.m_position));
  }

  bool operator!=(BleAudioPosition const &rhs) const { return !(*this == rhs); }

  void clear()
  {
    m_duration = 0;
    m_position = 0;
  }

  uint32_t m_duration{};
  uint32_t m_position{};
};

struct BleAudioFormat
{
  explicit BleAudioFormat() {}

  void clear()
  {
    m_type     = BluetoothApi::Streamer::FORMAT_TYPE_LAST;
    m_rate     = 0;
    m_channels = 0;
  }

  bool isValid() { return m_type != BluetoothApi::Streamer::FORMAT_TYPE_LAST; }

  FormatType m_type{BluetoothApi::Streamer::FORMAT_TYPE_LAST};
  uint32_t m_rate{};
  uint32_t m_channels{};
};

struct BleAudioBuffer
{
  explicit BleAudioBuffer() {}

  void clear()
  {
    m_data          = nullptr;
    m_size          = 0;
    m_sample_number = 0;
  }

  void *m_data{nullptr};
  uint32_t m_size{};
  uint32_t m_sample_number{};
};

struct BleAudioMessage
{
  enum
  {
    // DeviceConnectedMsg,
    TransportAddedMsg,
    TransportRemovedMsg,
    TransportPropertyChangeMsg,
    PlayerAddedMsg,
    PlayerRemovedMsg,
    PlayerPropertyChangeMsg,
    DevicePropertyChangeMsg,
    PlayerBufferReadyMsg,
    PlayerFormatChangeMsg,
    ClientStopRequestMsg,
    ClientQuitRequestMsg
  };

  explicit BleAudioMessage(int type, std::string const &path = std::string(),
                           std::string const &property = std::string(),
                           std::string const &value    = std::string(),
                           bool flag                   = false)
      : m_type(type), m_path(path), m_property(property), m_value(value),
        m_flag(flag)
  {}

  explicit BleAudioMessage(int type, BleAudioBuffer buffer)
      : m_type(type), m_play_buffer(buffer)
  {}

  explicit BleAudioMessage() = default;

  int m_type{};
  std::string m_path{};
  std::string m_property{};
  std::string m_value{};
  bool m_flag{};

  BleAudioBuffer m_play_buffer;
};

// NOTE:
// keep BluetoothApi::Streamer to maintain AudioStreamer interfaces.

class BleAudioStreamer
    : public BluetoothApi::Streamer,
      public AICommon::Observer<BleAudio::IBleAudioReaderEvent>,
      public std::enable_shared_from_this<BleAudioStreamer>
{
public:
  BleAudioStreamer(std::string const &name = std::string("BT AUDIO"));
  ~BleAudioStreamer();

  BleAudioStreamer(BleAudioStreamer const &) = delete;
  BleAudioStreamer &operator=(BleAudioStreamer const &) = delete;

  void postMessage(int message);

  enum FsmState
  {
    DeviceOffState,
    RunningSuperState,
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
    RunningStopEvent,
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
  void onTransportPropertyChange(std::string const &path,
                                 std::string const &property,
                                 std::string const &value);
  void onPlayerAdded(std::string const &path);
  void onPlayerRemoved(std::string const &path);
  void onPlayerPropertyChange(std::string const &path,
                              std::string const &property,
                              std::string const &value);
  void onDevicePropertyChange(std::string const &path,
                              std::string const &property, bool value);

public:
  // IBleAudioReaderEvent
  void onFormatChanged(FormatType format, unsigned int samplerate,
                       unsigned int channels);
  void onBufferReady(char *buffer, size_t nbytes, size_t sampleno);
  void onSilence(bool silent);

  // BluetoothApi::Streamer
public:
  void setBluetoothTimeout(uint32_t timeout) override;
  void releaseBuffer(void *buffer) override;
  void setMessageHandler(MessageHandler handler, void *user) override;
  void stop() override;

private:
  BleAudio::queue<BleAudioMessage> m_q;
  bool m_running{};
  std::thread m_t;
  std::mutex m_m;
  void doWork_(std::string const &name);

private:
  BleAudio::StateMachine m_fsm;
  void setupStateMachine_();
  std::string stringEvent_(FsmEvent);
  std::string stringState_(FsmState);
  void entered_(int state);
  void exited_(int state);
  void onEnterDeviceOnState_();
  void onEnterDeviceOffState_();
  void onEnterTransportOnState_();
  void onEnterPlayerOnState_();
  void onEnterPlayerPendingState_();
  void onEnterPlayerReadyState_();
  void onEnterPlayerRunningState_();
  void onEnterPlayerStopState_();
  void onEnterTransportIdleState_();
  void onEnterPlayerOffState_();
  void onEnterTransportOffState_();
  void onExitRunningSuperState_();

private:
  std::string m_device_path{};
  std::string m_transport_path{};
  std::string m_player_path{};

  bool checkHasAudioSource_(std::string const &path);
  bool updateMetadata_();
  bool updatePosition_();
  void clearPlayData_();
  void handleStopRequest_();
  void notify_(MessageType type,
               BleAudioMessage const &message = BleAudioMessage());
  bool getTransportConfig_(a2dp_sbc_t &config);
  bool getEndpointConfig_(a2dp_sbc_t &config);

  TimerQueue m_timer{};
  int64_t m_disconnection_timer{0};
  int64_t m_refresh_timer{0};
  uint32_t m_disconnection_timeout{0};

  int m_fd{-1};

  std::unique_ptr<BleAudioReader> m_reader{};

  bool onDisconnectionTimerExpired_();
  bool onRefreshTimerExpired_();
  bool onStopTimerExpired_();
  bool onBringUpTimerExpired_();

  BluetoothMediaPlayer *m_player_proxy{};
  BluetoothMediaTransport *m_transport_proxy{};
  BluetoothDevice *m_device_proxy{};
  BluetoothMediaEndpoint *m_endpoint_proxy{};

  std::unique_ptr<BluezProxyListener> m_player_listener;
  std::unique_ptr<BluezProxyListener> m_transport_listener;
  std::unique_ptr<BluezProxyListener> m_device_listener;

private:
  BleAudioMetadata m_play_metadata;
  BleAudioFormat m_play_format;
  BleAudioPosition m_play_position;
  std::string m_play_state{};

  MessageHandler m_observer{};
  void *m_observer_data{};
};

#endif // BLEAUDIOSTREAMER_H
