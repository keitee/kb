ethan-as-source/Components/Bluetooth/reader/include/BluetoothStreamer.h


namespace BluetoothApi
{
    class Streamer
    {
        /**
         * Parameters indicating the format of the stream.
         */
        typedef struct _Format
        {
            FormatType  type;               //!< format of the stream
            unsigned    sampleRate;         //!< in Hz e.g. 44100
            unsigned    channels;           //!< 1 for mono, 2 for stereo
        }   Format;

        /**
         * Messages consist of a struct containing all fields. Only the fields
         * relevant to the message type contain valid data.
         */
        typedef struct _Message
        {
            // enum
            MessageType type;               //!< type of message
            
            // enum
            // MESSAGE_TYPE_SESSION_START
            Format      audioFormat;        //!< format of the audio streamed in this session
            
            // MESSAGE_TYPE_METADATA
            std::string title;              //!< the title of the current track
            std::string artist;             //!< the artist of the current track
            std::string album;              //!< the album name of the current track
            unsigned    number;             //!< the track number of the current track
            unsigned    totalNumbers;       //!< the total number of tracks in the stream
            std::string genre;              //!< the gentre of the current track
            
            // MESSAGE_TYPE_PROGRESS
            unsigned    position;           //!< in milliseconds (There will be no more than one progress update per second)
            unsigned    duration;           //!< in milliseconds
            
            // enum
            // MESSAGE_TYPE_TRANSPORT_STATE
            TransportState transportState;  //!< transport state
            
            // enum
            // MESSAGE_TYPE_PLAYER_STATE
            PlayerState    playerState;     //!< player state
            
            // MESSAGE_TYPE_BUFFER_FILLED
            void        *buffer;            //!< the buffer pointer as provided via addBuffer
            size_t       bufferLevel;       //!< number of bytes that was filled into the buffer
            size_t       sampleNumber;      //!< Sample number of first sample, buffer must contain contiguous samples

            _Message();                     //!< constructor sets all fields to default values

        }   Message;
    }
}

    sendSessionStart(session->player_format); \

        // from AudioStreamer
        Message msg;

        msg.type = MESSAGE_TYPE_SESSION_START;
        BluetoothApi::Streamer::FormatType  format;
        unsigned int                        samplerate;
        unsigned int                        channels;


        // from a2do transport
void BluetoothMediaStreamer::onMediaTransportUp
  (BluetoothA2DPTransport *xport, BluetoothApi::Streamer::FormatType format, unsigned int samplerate, unsigned int channels)

                        session->player_format = m.format;

/*
 * metadata
*/
{
  msg.type = MESSAGE_TYPE_METADATA;
  msg.title  = metadata.title;
  msg.artist = metadata.artist;
  msg.album  = metadata.album;
  msg.number = metadata.track_number;
  msg.totalNumbers = metadata.number_of_tracks;
  msg.genre = metadata.genre;

  std::string title;
  std::string artist;
  std::string album;
  uint32_t track_number;
  uint32_t number_of_tracks;
  std::string genre;

  // use
  // comparison. necessary?
  if (session && session->player_metadata != m.metadata) {
  }

  sendMetadata(session->player_metadata);
}

    sendPlayerState(session->player_state); \

        msg.type = MESSAGE_TYPE_PLAYER_STATE;
        msg.playerState = ms.state;

    sendProgress(session->player_position); \

        msg.type = MESSAGE_TYPE_PROGRESS;
        msg.position = position.track_position;
        msg.duration = position.track_duration;

        uint32_t track_duration;
        uint32_t track_position;

        // compariso. ??
                  if (session && session->player_position != m.position) {

    // saved in ad2p transport
    std::string m_transport_path;


#define sessionIsActive(session)                ( session && session->session_in_progress && !session->teardown_in_progress )
        bool session_in_progress;

#define sessionTeardown(session) do { \
        session->teardown_in_progress = true; \

        bool teardown_in_progress;


                       if (!(session->xport_up && session->player_up))
        bool xport_up;
        bool player_up;


                    if (session && session->player_state != m.state && m.state.isValid()) {

        MediaPlayerState player_state;


                    if (session->player_is_unreliable) break;
        bool player_is_unreliable;


                        } else if (m.timer_id == session->refresh_timer) {
