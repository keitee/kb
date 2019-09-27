
// File: BluetoothStreamer.h
//
// Copyright (c) 2014 BSKYB Ltd. All rights reserved.

#ifndef __BluetoothStreamer_H__
#define __BluetoothStreamer_H__

#include <string>

namespace BluetoothApi {
class Streamer
{
public:
  /**
   * The format type of the stream.
   */
  typedef enum _FormatType
  {
    FORMAT_TYPE_PCM_S8,     //!< signed 8 bits/sample
    FORMAT_TYPE_PCM_U8,     //!< unsigned 8 bits/sample
    FORMAT_TYPE_PCM_S16_LE, //!< signed 16 bits/sample little endian
    FORMAT_TYPE_PCM_S16_BE, //!< signed 16 bits/sample big endian
    FORMAT_TYPE_PCM_U16_LE, //!< unsigned 16 bits/sample little endian
    FORMAT_TYPE_PCM_U16_BE, //!< unsigned 16 bits/sample big endian

    FORMAT_TYPE_LAST

    //! @todo we could add FORMAT_TYPE_AAC/MP3/APTX here when supported
  } FormatType;

  /**
   * Parameters indicating the format of the stream.
   */
  typedef struct _Format
  {
    FormatType type;     //!< format of the stream
    unsigned sampleRate; //!< in Hz e.g. 44100
    unsigned channels;   //!< 1 for mono, 2 for stereo

  } Format;

  /**
   * The message type.
   */
  typedef enum _MessageType
  {
    MESSAGE_TYPE_SESSION_START, //!< a device has initiated playback
    MESSAGE_TYPE_SESSION_STOP,  //!< a playing device has stopped playback and
                                //!< terminated the session
    MESSAGE_TYPE_METADATA, //!< metadata is provided for the currently playing
                           //!< song
    MESSAGE_TYPE_PROGRESS, //!< playback progress report
    MESSAGE_TYPE_TRANSPORT_STATE, //!< transport state
    MESSAGE_TYPE_PLAYER_STATE,    //!< player (remote device) state
    MESSAGE_TYPE_BUFFER_FILLED,   //!< full pcm buffer

    MESSAGE_TYPE_LAST

  } MessageType;

  /**
   * The transport state.
   */
  typedef enum _TransportState
  {
    TRANSPORT_STATE_IDLE,
    TRANSPORT_STATE_PENDING,
    TRANSPORT_STATE_ACTIVE,

    TRANSPORT_STATE_LAST

  } TransportState;

  /**
   * The media player state.
   */
  typedef enum _PlayerState
  {
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_STOPPED,
    PLAYER_STATE_PAUSED,
    PLAYER_STATE_FORWARD_SEEK,
    PLAYER_STATE_REWIND_SEEK,
    PLAYER_STATE_ERROR,

    PLAYER_STATE_LAST

  } PlayerState;

  /**
   * Messages consist of a struct containing all fields. Only the fields
   * relevant to the message type contain valid data.
   */
  typedef struct _Message
  {
    MessageType type; //!< type of message

    // MESSAGE_TYPE_SESSION_START
    Format audioFormat; //!< format of the audio streamed in this session

    // MESSAGE_TYPE_METADATA
    std::string title;     //!< the title of the current track
    std::string artist;    //!< the artist of the current track
    std::string album;     //!< the album name of the current track
    unsigned number;       //!< the track number of the current track
    unsigned totalNumbers; //!< the total number of tracks in the stream
    std::string genre;     //!< the gentre of the current track

    // MESSAGE_TYPE_PROGRESS
    unsigned position; //!< in milliseconds (There will be no more than one
                       //!< progress update per second)
    unsigned duration; //!< in milliseconds

    // MESSAGE_TYPE_TRANSPORT_STATE
    TransportState transportState; //!< transport state

    // MESSAGE_TYPE_PLAYER_STATE
    PlayerState playerState; //!< player state

    // MESSAGE_TYPE_BUFFER_FILLED
    void *buffer;        //!< the buffer pointer as provided via addBuffer
    size_t bufferLevel;  //!< number of bytes that was filled into the buffer
    size_t sampleNumber; //!< Sample number of first sample, buffer must contain
                         //!< contiguous samples

    _Message(); //!< constructor sets all fields to default values

  } Message;

  /** @brief Function prototype of the message handler callback.
   *
   * The message handler must do no processing, but copy all relevant
   * data. Fields are not guaranteed in scope after the call the message
   * handler terminates
   */
  typedef void (*MessageHandler)(const Message &message, void *user);

public:
  // singleton pattern with managed instance lifetime
  static void createInstance(); // create instance and allow discoverability
  static void
  destroyInstance(); // destroy instance and turn off discoverability

  static Streamer *instance();

public:
  /**
   * Set the callback for messages... the callback will be called from a foreign
   * thread context.
   */
  virtual void setMessageHandler(MessageHandler handler, void *user) = 0;

  /**
   * Set bluetooth device disconnection timeout
   */
  virtual void setBluetoothTimeout(uint32_t timeout) = 0;

  /**
   * Informs the streaming device that we want to stop the stream
   */
  virtual void stop() = 0;

  /**
   * Release a data buffer.
   */
  virtual void releaseBuffer(void *buffer) = 0;

public:
  virtual ~Streamer() {}
};

} // namespace BluetoothApi

#endif // #ifndef  __BluetoothStreamer_H__
