// #if !defined(__STDC_FORMAT_MACROS)
// #define __STDC_FORMAT_MACROS
// #endif

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <functional>
#include <mutex>

#include "bleaudioreader.h"
#include "bleaudioreaderevent.h"

#ifdef USE_HOST_BUILD

#include "slog.h"
#define AS_LOG_ERROR LOG_MSG
#define AS_LOG_INFO LOG_MSG
#define AS_LOG_MIL LOG_MSG

ssize_t sbc_decode(sbc_t *sbc, const void *input, size_t input_len,
                          void *output, size_t output_len, size_t *written)
{
  return 0;
}

int sbc_reinit(sbc_t *sbc, unsigned long flags)
{
  return 0;
}

int sbc_init(sbc_t *sbc, unsigned long flags)
{
  return 0;
}

void sbc_finish(sbc_t *sbc)
{
  return;
}

#else

#include "AS_Diag.h"
extern AS_DIAG::Context *dbusitf_logging_ctx;
#undef AS_DIAG_CONTEXT_DEFAULT
#define AS_DIAG_CONTEXT_DEFAULT (dbusitf_logging_ctx)

// #include "sbc.h"

#endif // USE_HOST_BUILD


void BleAudioReader::wakeup()
{
  m_poller.interruptPoll();
}

void BleAudioReader::onEvent(int, int, void*)
{
  ssize_t sz;
  ssize_t decoded_sz;

  while (true) {
    sz = read(m_fd, &m_read_buf[m_data_offset],
              sizeof(m_read_buf) - m_data_offset);
    if (sz == 0) {
      // notify(onEOS());
      AS_LOG_ERROR("EOS error");
      goto read_error;
    } else if (sz < 0) {
      if (errno == EINTR)
        continue;
      else {
        // notify(onFDError(errno));
        AS_LOG_ERROR("fd error");
        goto read_error;
      }
    }
    break;
  }

  AS_LOG_INFO("read %zi bytes, offset %zi", sz, m_data_offset);
  sz += m_data_offset;
  decoded_sz = decode(m_read_buf, sz);
  if (decoded_sz == 0)
    return;
  AS_LOG_INFO("decoded %zi bytes", decoded_sz);
  if (decoded_sz < sz) {
    AS_LOG_INFO("requeuing %zi bytes", sz - decoded_sz);
    memmove(&m_read_buf[0], &m_read_buf[decoded_sz], sz - decoded_sz);
    m_data_offset = sz - decoded_sz;
  } else
    m_data_offset = 0;

  return;

read_error : {
  std::lock_guard<std::mutex> lock(m_thread_lock);
  m_kill_thread = true;
}
  return;
}

void BleAudioReader::readerThread()
{
  AS_LOG_INFO("media reader thread starting");

  pthread_attr_t attr{};
  if (pthread_getattr_np(pthread_self(), &attr)) {
    AS_LOG_ERROR("failed to pthread_getattr_np");
  }

  pthread_attr_setschedpolicy(&attr, SCHED_RR);

  // Reset decoder
  reset();

  while (true) {
    {
      std::lock_guard<std::mutex> lock(m_thread_lock);
      if (m_kill_thread)
        goto exit;
    }

    m_poller.doPoll(-1);
  }

exit:
  AS_LOG_INFO("media reader thread returning");
}

void BleAudioReader::sendAudioParams(void)
{
  int frequency;
  int channels;
  FormatType fmt;

  channels = m_sbc.mode == SBC_MODE_MONO ? 1 : 2;

  if (m_sbc.frequency == SBC_FREQ_16000)
    frequency = 16000;
  else if (m_sbc.frequency == SBC_FREQ_32000)
    frequency = 32000;
  else if (m_sbc.frequency == SBC_FREQ_44100)
    frequency = 44100;
  else if (m_sbc.frequency == SBC_FREQ_48000)
    frequency = 48000;
  else
    frequency = 0;

  if (m_sbc.endian == SBC_LE)
    fmt = BluetoothApi::Streamer::FORMAT_TYPE_PCM_S16_LE;
  else
    fmt = BluetoothApi::Streamer::FORMAT_TYPE_PCM_S16_BE;

  if ((m_sample == 0) || (frequency != m_frequency) ||
      (channels != m_channels) || (fmt != m_format)) {
    AS_LOG_MIL("audio stream format changed: freq=%d channels=%d format=%d",
               frequency, channels, fmt);
    // notify(onFormatChange(fmt, frequency, channels));
    notify(&BleAudio::IBleAudioReaderEvent::onFormatChanged, fmt, frequency, channels);
    m_frequency = frequency;
    m_channels = channels;
    m_format = fmt;
  }

  return;
}

void BleAudioReader::sbcConfigure(sbc_t *sbcinfo)
{
  switch (m_config.frequency) {
  case 8:
    sbcinfo->frequency = SBC_FREQ_16000;
    break;
  case 4:
    sbcinfo->frequency = SBC_FREQ_32000;
    break;
  case 2:
    sbcinfo->frequency = SBC_FREQ_44100;
    break;
  case 1:
    sbcinfo->frequency = SBC_FREQ_48000;
    break;
  default:
    AS_LOG_ERROR("Configuration for sampling value is bad?");
    break;
  }

  switch (m_config.block_length) {
  case 8:
    sbcinfo->blocks = SBC_BLK_16;
    break;
  case 4:
    sbcinfo->blocks = SBC_BLK_12;
    break;
  case 2:
    sbcinfo->blocks = SBC_BLK_8;
    break;
  case 1:
    sbcinfo->blocks = SBC_BLK_4;
    break;
  default:
    AS_LOG_ERROR("Configuration for block length is bad?");
    break;
  }

  switch (m_config.subbands) {
  case 2:
    sbcinfo->subbands = SBC_SB_4;
    break;
  case 1:
    sbcinfo->frequency = SBC_SB_8;
    break;
  default:
    AS_LOG_ERROR("Configuration for Sub band is bad?");
    break;
  }

  switch (m_config.channel_mode) {
  case 8:
    sbcinfo->mode = SBC_MODE_MONO;
    break;
  case 4:
    sbcinfo->mode = SBC_MODE_DUAL_CHANNEL;
    break;
  case 2:
    sbcinfo->mode = SBC_MODE_STEREO;
    break;
  case 1:
    sbcinfo->mode = SBC_MODE_DUAL_CHANNEL;
    break;
  default:
    AS_LOG_ERROR("Configuration for channel mode is bad?");
    break;
  }

  switch (m_config.allocation_method) {
  case 1:
    sbcinfo->mode = SBC_AM_LOUDNESS;
    break;
  case 2:
    sbcinfo->mode = SBC_AM_SNR;
    break;
  default:
    AS_LOG_ERROR("Configuration for Allocation method is bad?");
    break;
  }
  return;
}

ssize_t BleAudioReader::decode(const uint8_t *data, int sz)
{
  int framelen = 0;
  int frame;
  uint8_t *p;
  int to_decode;
  struct rtp_payload *payload;
  size_t outcount;
  size_t decoded_len = 0;
  ssize_t offset = 0;

  p = (uint8_t *)data + sizeof(struct rtp_header) + sizeof(*payload);
  to_decode = sz - sizeof(struct rtp_header) - sizeof(*payload);
  payload = (struct rtp_payload *)((uint8_t *)data + sizeof(struct rtp_header));

  if (to_decode <= 0)
    return 0;

  /* None of the fields in the payload which are accessed below are wider than
   * a byte. Endianness is handled by the RTP structure definitions at the top
   * of this source file.
   */
  AS_LOG_INFO("Decoding %d bytes", sz);
  AS_LOG_INFO(
      "Header: CC %d, Extension %d, P %d, V %d, Payload Type %d, Marker %d",
      ((struct rtp_header *)data)->cc, ((struct rtp_header *)data)->x,
      ((struct rtp_header *)data)->p, ((struct rtp_header *)data)->v,
      ((struct rtp_header *)data)->pt, ((struct rtp_header *)data)->m);
  AS_LOG_INFO("Payload: Frame Count %d, RFA0 %d, Fragment %d, %d, %d",
              payload->frame_count, payload->rfa0, payload->is_last_fragment,
              payload->is_first_fragment, payload->is_fragmented);
  AS_LOG_INFO("Left %d", to_decode);

  for (frame = 0; frame < payload->frame_count; frame++) {
    framelen = sbc_decode(&m_sbc, p + offset, to_decode - offset,
                          m_buf + decoded_len, BUFLEN - decoded_len, &outcount);
    if (framelen > 0) {
      /* send audio params if they've changed */
      sendAudioParams();

      AS_LOG_INFO("frame length %i, offset %zi, output size %zd", framelen,
                  offset, outcount);

      // each sample is 16 bits. Keep a count of samples
      m_sample += (outcount / sizeof(uint16_t) / m_channels);

      decoded_len += outcount;
      offset += framelen;
      if (to_decode == offset)
        break;
      if (to_decode < offset) {
        AS_LOG_INFO("parsed passed end of frame?");
        break;
      }
    } else if (framelen == SBC_DECODE_ERR_BAD_SYNC_BYTE)
      AS_LOG_ERROR("Buffer does not start with sync word (0x%x)?", *p);
    else if (framelen == SBC_DECODE_ERR_STREAM_TOO_SHORT)
      AS_LOG_ERROR("Buffer is too short?");
    else if (framelen == SBC_DECODE_ERR_BAD_CRC)
      AS_LOG_ERROR("CRC error");
    else if (framelen == SBC_DECODE_ERR_OUT_OF_BOUNDS)
      AS_LOG_ERROR("Frame bitpool out of bounds");
  }

  // track buffer level every 1/10 of a second
  if (m_next_level_check == (m_sample / (m_frequency / 10))) {
    double level = 0;
    uint16_t *pdata = (uint16_t *)m_buf;
    for (size_t s = 0; s < decoded_len; s++)
      level += pdata[s];
    m_levels.push_back(level / decoded_len);
    if (m_levels.size() > 3) {
      bool is_silent = true;
      m_levels.pop_front();
      for (size_t s = 0; s < m_levels.size(); s++) {
        if (m_levels[s] != 0.0) {
          is_silent = false;
          break;
        }
      }
      if (m_silent != is_silent) {
        m_silent = is_silent;
        // notify(onSilence(m_silent));
        AS_LOG_ERROR("onSilence");
      }
    }
    m_next_level_check += 1;
  }

  if (decoded_len > 0) {
    notify(&BleAudio::IBleAudioReaderEvent::onBufferReady, (char*)m_buf, decoded_len, m_sample);
    return offset + sizeof(struct rtp_header) + sizeof(*payload);
  }

  return 0;
}

void BleAudioReader::reset(void)
{
  sbc_reinit(&m_sbc, 0);
  m_sbc.endian = SBC_BE;
  m_sample = 0;
  m_next_level_check = 0;
  m_silent = true;
}

void BleAudioReader::setConfiguration(a2dp_sbc_t *configuration)
{
  if (configuration == NULL)
    return;

  memcpy(&m_config, configuration, sizeof(a2dp_sbc_t));
  reset();
  sbcConfigure(&m_sbc);

  return;
}

BleAudioReader::BleAudioReader(int fd, a2dp_sbc_t &config)
    : m_fd(fd), m_thread_running(false), m_kill_thread(false), m_data_offset(0)
{
  int ret;
  pthread_attr_t attr;

  assert(m_fd >= 0);

  sbc_init(&m_sbc, 0);

  memset(m_buf, 0, sizeof(m_buf));
  memset(m_read_buf, 0, sizeof(m_read_buf));

  setConfiguration(&config);

  m_fd_tag = m_poller.addFD(m_fd, POLLIN | POLLPRI, true, this, nullptr);

  m_thread = std::thread(&BleAudioReader::readerThread, this);
}

BleAudioReader::~BleAudioReader()
{
  {
    std::lock_guard<std::mutex> lock(m_thread_lock);
    m_kill_thread = true;
  }

  wakeup();

  m_thread.join();

  m_poller.removeFD(m_fd_tag);
  close(m_fd);

  sbc_finish(&m_sbc);
}
