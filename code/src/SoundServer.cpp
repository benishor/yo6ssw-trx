//
// Created by benny on 11/30/16.
//

#include <src/SoundServer.h>
#include <SDL_audio.h>
#include <stdexcept>
#include <iostream>

void renderAudio(void* userData, uint8_t* audioBuffer, int bufferLengthInBytes) {
    // 2 channels * 2 bytes per sample == 4 bytes per sample frame
    uint16_t numberOfSampleFrames = (uint16_t) (bufferLengthInBytes >> 2);

    SoundProducer& producer = *static_cast<SoundProducer*>(userData);
    producer.render((int16_t*) audioBuffer, numberOfSampleFrames);
}

SoundServer::SoundServer(uint16_t desiredSampleRate, uint16_t numberOfSampleFrames, SoundProducer& producer) {
    sampleRate = desiredSampleRate;

    SDL_AudioSpec desiredAudioFormat;

    desiredAudioFormat.freq = sampleRate;
    desiredAudioFormat.format = AUDIO_S16;
    desiredAudioFormat.channels = 2;
    desiredAudioFormat.samples = numberOfSampleFrames;
    desiredAudioFormat.callback = renderAudio;
    desiredAudioFormat.userdata = static_cast<void*>(&producer);

    if (SDL_OpenAudio(&desiredAudioFormat, NULL) < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        throw std::runtime_error("Failed to open audio");
    }

    SDL_PauseAudio(0);
}

SoundServer::~SoundServer() {
    SDL_CloseAudio();
}

uint16_t SoundServer::getSampleRate() const {
    return sampleRate;
}
