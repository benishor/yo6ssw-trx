#include "Demodulator.h"
#include "FftComplex.h"
#include <src/WavWriter.h>
#include <src/WavReader.h>

using namespace std;

const uint32_t DEFAULT_FFT_BINS = 1024;

Demodulator::Demodulator(const std::string& iqSourceWav, const std::string& outputWav, int carrierFrequency, Sideband sideband) {
    wavReader = make_shared<WavReader>(iqSourceWav);
    samplerate = wavReader->getHeader().sampleRate;

    AudioSettings audioSettings;
    audioSettings.sampleRate = (int) samplerate;
    audioSettings.channels = 2;

    wavWriter = make_shared<WavWriter>(audioSettings, outputWav);

    carrierPhaseIncrement = 2.0 * M_PI * carrierFrequency / samplerate;
    carrierPhase = 0;

    sidebandToDecode = sideband;
    setFFTSize(DEFAULT_FFT_BINS);
}

void Demodulator::setFFTSize(uint32_t bins) {
    fftSize = bins;
    fftBins.reserve(fftSize);
    fftBins.clear();
}


void Demodulator::demodulate() {
    auto audioBuffer = new short[fftSize * 2];

    wavWriter->begin();
    while (fillFFTBufferWithSourceIQSamples()) {
        performFFT();
        dropUnneededSideBand();
        lowPassFilter();
        performInverseFFT();
        fillAudioBufferFromFFTBuffer(audioBuffer);
        wavWriter->write(audioBuffer, fftSize);
    }
    wavWriter->end();
}

bool Demodulator::fillFFTBufferWithSourceIQSamples() {
    static short* iqSamples = nullptr;
    if (iqSamples == nullptr) {
        iqSamples = new short[fftSize * 2];
    }

    if (wavReader->readData((char*) iqSamples, fftSize * 4) != fftSize * 4)
        return false;

    fftBins.clear();
    for (int i = 0; i < fftSize; i++) {
        auto sourceSample = complex<double>(iqSamples[i*2+0] / 32767.0,
                                            iqSamples[i*2+1] / 32767.0);

        carrierPhase += carrierPhaseIncrement;
        carrierPhase = fmod(carrierPhase, M_PI * 2.0);
        auto mixedSample = complex<double>(sourceSample.real() * cos(carrierPhase),
                                           sourceSample.imag() * -sin(carrierPhase));

        fftBins.push_back(mixedSample);
    }
    return true;
}

void Demodulator::performFFT() {
    Fft::transform(fftBins);
}

void Demodulator::dropUnneededSideBand() {
    if (sidebandToDecode == Sideband::USB) {
        // drop negative frequencies
        for (int i = 1; i <= fftSize / 2; i++) {
            fftBins[i] = complex<double>(0, 0);
        }
    } else {
        // drop positive frequencies
        for (int i = fftSize / 2 + 1; i < fftSize; i++) {
            fftBins[i] = complex<double>(0, 0);
        }
    }
}

void Demodulator::lowPassFilter() {
    int howManyBinsToKeep = 300; // experimentally set
    for (int i = (1 + howManyBinsToKeep); i <= fftSize / 2; i++) {
        fftBins[i] = complex<double>(0, 0);
    }
    for (int i = (fftSize / 2 + 1); i < (fftSize - howManyBinsToKeep); i++) {
        fftBins[i] = complex<double>(0, 0);
    }
}

void Demodulator::performInverseFFT() {
    Fft::inverseTransform(fftBins);
    for (auto& sample : fftBins) {
        sample /= static_cast<double>(fftSize);
    }
}

void Demodulator::fillAudioBufferFromFFTBuffer(short* buffer) {
    int offset = 0;
    for (int i = 0; i < fftSize; i++) {
        short sample = (short) ((fftBins[i].real() + fftBins[i].imag()) * 60000);
        buffer[offset++] = sample;
        buffer[offset++] = sample;
    }
}

uint32_t Demodulator::getFFTSize() const {
    return fftSize;
}
