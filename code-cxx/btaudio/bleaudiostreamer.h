/*
 *
 */

#ifndef BLEAUDIOSTREAMER_H
#define BLEAUDIOSTREAMER_H

#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "BluetoothStreamer.h"
#include "CallerThreadDispatcher.h"
#include "Notifier.h"
#include "Observer.h"

#include "bleaudioreader.h"
#include "bleaudioreaderevent.h"
#include "eventqueue.h"
#include "statemachine.h"
#include "timer_queue.h"

struct BleAudioMetadata
{
  bool operator==(BleAudioMetadata const &rhs) const
  {
    if ((title_ != rhs.title_) || (artist_ != rhs.artist_) ||
        (album_ != rhs.album_) || (track_number_ != rhs.track_number_) ||
        (number_of_tracks_ != rhs.number_of_tracks_))
    {
      return false;
    }

    return true;
  }

  bool operator!=(BleAudioMetadata const &rhs) const { return !(*this == rhs); }

  void clear()
  {
    title_.clear();
    artist_.clear();
    album_.clear();
    track_number_     = 0;
    number_of_tracks_ = 0;
    genre_.clear();
  }

  std::string title_{};
  std::string artist_{};
  std::string album_{};
  uint32_t track_number_{};
  uint32_t number_of_tracks_{};
  std::string genre_{};
};

struct BleAudioPosition
{
  bool operator==(BleAudioPosition const &rhs) const
  {
    return ((duration_ == rhs.duration_) && (position_ == rhs.position_));
  }

  bool operator!=(BleAudioPosition const &rhs) const { return !(*this == rhs); }

  void clear()
  {
    duration_ = 0;
    position_ = 0;
  }

  uint32_t duration_{};
  uint32_t position_{};
};

struct BleAudioFormat
{
  explicit BleAudioFormat() {}

  void clear()
  {
    type_     = BluetoothApi::Streamer::FORMAT_TYPE_LAST;
    rate_     = 0;
    channels_ = 0;
  }

  FormatType type_{};
  size_t rate_{};
  size_t channels_{};
};

struct BleAudioBuffer
{
  explicit BleAudioBuffer() {}

  void clear()
  {
    data_          = nullptr;
    size_          = 0;
    sample_number_ = 0;
  }

  void *data_{nullptr};
  size_t size_{};
  size_t sample_number_{};
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
                           std::string const &value    = std::string())
      : type_(type), path_(path), property_(property), value_(value)
  {}

  // explicit BleAudioMessage(int type) : type_(type) {}

  explicit BleAudioMessage() = default;

  // TODO size_t?
  int type_{};
  std::string path_{};
  std::string property_{};

  // TODO debug purpose
  std::string value_{};
};

// use BluetoothApi::Streamer to maintain AudioStreamer interfaces.

class BleAudioStreamer
    : public BluetoothApi::Streamer,
      public AICommon::Observer<BleAudio::IBleAudioReaderEvent>
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
  void onTransportPropertyChange(std::string const &path,
                                 std::string const &property,
                                 std::string const &value);
  void onPlayerAdded(std::string const &path);
  void onPlayerRemoved(std::string const &path);
  void onPlayerPropertyChange(std::string const &path,
                              std::string const &property,
                              std::string const &value);
  void onDevicePropertyChange(std::string const &path,
                              std::string const &property,
                              std::string const &value);

public:
  // IBleAudioReaderEvent
  void onFormatChanged(FormatType format, unsigned int samplerate,
                       unsigned int channels);
  void onBufferReady(char *buffer, size_t nbytes, size_t sampleno);

  // BluetoothApi::Streamer
public:
  void setBluetoothTimeout(uint32_t timeout) override;
  void releaseBuffer(void *buffer) override;
  void setMessageHandler(MessageHandler handler, void *user) override;
  void stop() override;

private:
  BleAudio::queue<BleAudioMessage> q_;
  std::thread t_;
  std::mutex m_;
  bool running_;
  void doWork_(std::string const &name);

private:
  StateMachine fsm_;
  void setupStateMachine_();
  std::string stringEvent_(FsmEvent);
  std::string stringState_(FsmState);
  void entered_(int state);
  void exited_(int state);
  void eafDeviceOnState_();
  void eafDeviceOffState_();
  void eafTransportOnState_();
  void eafPlayerOnState_();
  void eafPlayerPendingState_();
  void eafPlayerReadyState_();
  void eafPlayerRunningState_();
  void eafPlayerStopState_();
  void eafTransportIdleState_();
  void eafPlayerOffState_();
  void eafTransportOffState_();

private:
  std::string device_path_;
  std::string transport_path_;
  std::string player_path_;

  bool checkHasAudioSource_(std::string const &path);
  void updateMetadata_();
  void updatePosition_();
  void updateState_();
  void clearPlayData_();
  void handleStopRequest_();
  void notify_(MessageType type);
  bool getTransportConfig_(a2dp_sbc_t &config);
  bool getEndpointConfig_(a2dp_sbc_t &config);

  TimerQueue timer_{};
  int64_t disconnection_timer_{0};
  int64_t refresh_timer_{0};
  uint32_t disconnection_timeout_{0};

  int fd_{};
  std::unique_ptr<BleAudioReader> reader_{};

  bool onDisconnectionTimerExpired_();
  bool onRefreshTimerExpired_();
  bool onStopTimerExpired_();

#ifndef USE_HOST_BUILD
  player_proxy_;
  transport_proxy_;
  device_proxy_;
#endif

private:
  BleAudioMetadata play_metadata_;
  BleAudioFormat play_format_;
  BleAudioBuffer play_buffer_;
  BleAudioPosition play_position_;
  std::string play_state_;

  MessageHandler observer_{};
  void *observer_data_{};
};

#endif // BLEAUDIOSTREAMER_H
