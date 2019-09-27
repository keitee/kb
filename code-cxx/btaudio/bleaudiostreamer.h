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
    if ((title != rhs.title) || (artist != rhs.artist) ||
        (album != rhs.album) || (track_number != rhs.track_number) ||
        (number_of_tracks != rhs.number_of_tracks)) {
      return false;
    }

    return true;
  }

  std::string title{};
  std::string artist{};
  std::string album{};
  uint32_t track_number{};
  uint32_t number_of_tracks{};
  std::string genre{};
};

struct BleAudioFormat
{
  FormatType type{};
  uint32_t rate{};
  uint32_t channels{};
};

struct BleAudioMessage
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

  explicit BleAudioMessage(int type, std::string const &path = std::string(),
                   std::string const &property = std::string(),
                   std::string const &value = std::string())
      : type_(type), path_(path), property_(property), value_(value)
  {}

  int type_;
  std::string path_;
  std::string property_;

  // TODO debug purpose
  std::string value_;
};


// use BluetoothApi::Streamer to maintain AudioStreamer interfaces.

class BleAudioStreamer
    : public BluetoothApi::Streamer,
      public AICommon::Observer<BleAudio::IBleAudioReaderEvent>
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

  // IBleAudioReaderEvent
  void onFormatChanged(FormatType format, unsigned int samplerate,
                       unsigned int channels);
  void onBufferReady(char *buffer, size_t nbytes, size_t sampleno);

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

private:
  std::string device_path_;
  std::string transport_path_;
  std::string player_path_;

  TimerQueue disconnection_timer_;

  bool checkHasAudioSource_(std::string const &path);
  void updateMetadata_();
  void notify_(MessageType type);
  bool getTransportConfig_(a2dp_sbc_t &config);
  bool getEndpointConfig_(a2dp_sbc_t &config);

#ifndef USE_HOST_BUILD
  player_proxy_;
  transport_proxy_;
  device_proxy_;
#endif

  int fd_;
  BleAudioReader *reader_;

private:
  BleAudioMetadata metadata_;
  BleAudioFormat format_;

  MessageHandler observer_;
  void *observer_data_;
};

#endif // BLEAUDIOSTREAMER_H
