/*
 *
*/

#ifndef BLEAUDIOREADEREVENT_H
#define BLEAUDIOREADEREVENT_H


// #include "Observer.h"
#include "BluetoothStreamer.h"

#include <Common/Observer.h>


// appinfrastructure/AppInfrastructure/Public/Common/Observer.h
// appinfrastructure/AppInfrastructure/Public/Common/Polymorphic.h
// appinfrastructure/AppInfrastructure/Public/Common/Notifier.h
// appinfrastructure/AppInfrastructure/Public/Common/IDispatcher.h
// appinfrastructure/AppInfrastructure/Common/lib/include/CallerThreadDispatcher.h

using FormatType = BluetoothApi::Streamer::FormatType;

namespace BleAudio
{
  class IBleAudioReaderEvent
  {
    public:
      virtual ~IBleAudioReaderEvent() {};

      /**
       * @brief Pairing status changed notification
       * @param pairingInProgress Pairing in progress or not.
       */
      virtual void onFormatChanged(FormatType format, unsigned int samplerate, unsigned int channels) = 0;

      virtual void onBufferReady(char *buffer, size_t nbytes, size_t sampleno) = 0;

      // virtual void onFDError(int error) = 0;
      // virtual void onEOS(void) = 0;
      // virtual void onSilence(bool is_silent) = 0;
  };
} // namesapce

#endif // BLEAUDIOREADEREVENT_H
