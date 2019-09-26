
// File: bluetooth_media_reader.h
//
// Copyright (c) 2014 BSKYB Ltd. All rights reserved.
//

#ifndef BLUAUDIOREADER_H
#define BLUAUDIOREADER_H

#include "sbc/sbc.h"
#include <deque>
#include <stdint.h>

#include "BluetoothStreamer.h"
#include "bluetooth_media_config.h"
#include "poll_fd.h"

// appinfrastructure/AppInfrastructure/Public/Common/Notifier.h
#include <Common/Notifier.h>

using FormatType = BluetoothApi::Streamer::FormatType;

// class BleAudioReader : public PollFDEventListener public
// AICommon::Notifier<IBleRcuDeviceEvent>

class BleAudioReader : AICommon::Notifier<BleAudio::IBleAudioReaderEvent>
{
public:
  BleAudioReader(int fd, a2dp_sbc_t &config);
  ~BleAudioReader();

private:
  void readMediaFd(void);
  void onEvent(int fd, int event, void *dptr);
  void wakeup();
  void reset(void);
  void sendAudioParams(void);
  void sbcConfigure(sbc_t *sbcinfo);
  void setConfiguration(a2dp_sbc_t *configuration);
  ssize_t decode(const uint8_t *data, int sz);

  void readerThread();

  Poller m_poller;
  int m_fd;
  PollFDTag m_fd_tag;
  bool m_thread_running;
  bool m_kill_thread;
  std::thread m_thread;
  std::condition_variable m_thread_cond;
  std::mutex m_thread_lock;

  a2dp_sbc_t m_config;
  sbc_t m_sbc;
  int m_frequency;
  int m_channels;
  BluetoothApi::Streamer::FormatType m_format;

  static const int BUFLEN = 100 * 4096;
  ssize_t m_data_offset;
  uint32_t m_sample;
  uint32_t m_next_level_check;
  std::deque<double> m_levels;
  bool m_silent;

  uint8_t m_buf[BUFLEN];
  // Used only by readMediaFd()
  uint8_t m_read_buf[608 * 4];

  std::vector<BleAudioReaderListener *> m_listeners;
  std::mutex m_listener_lock;
};

#endif // BLUAUDIOREADER_H
