//
// Created by benny on 11/26/16.
//

#ifndef EMUSIC_SOUNDSERVER_H
#define EMUSIC_SOUNDSERVER_H

#include <cstdint>

/// Abstraction for all things able to generate sound. It holds a callback
/// that the client code calls whenever it wants sound data to be produced,
/// regardless of where it will be used.
class SoundProducer {
public:
    /// Fills the provided buffer with the required number of samples.
    /// The format of the buffer is stereo, interleaved which means it has
    /// the following layout: 0:left,1:right,2:left,3:right...
    /// @param [in] buffer the buffer to be filled with produced sound data
    /// @param [in] numberOfSampleFrames the number of sample frames to fill the buffer with. Do recall that a sample frame means two samples since we have a stereo format
    virtual void render(int16_t buffer[], uint16_t numberOfSampleFrames) = 0;
};

/// The interface to the sound system, hiding details about the system used.
class SoundServer {
public:
    /// Attempts to create a sound server with respect to the provided buffer size.
    /// @param [in] desiredSampleRate the desired output sample rate
    /// @param [in] numberOfSampleFrames the number of sample frames. A sample frame is a stereo sample of the internal used format (S16 in this case)
    /// @param [in] producer the sound producer
    SoundServer(uint16_t desiredSampleRate, uint16_t numberOfSampleFrames, SoundProducer& producer);
    virtual ~SoundServer();

    uint16_t getSampleRate() const;

private:
    uint16_t sampleRate;
};

#endif //EMUSIC_SOUNDSERVER_H
