//
// Created by benny on 8/29/17.
//
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <complex>

enum class Sideband {
    LSB,
    USB
};

class WavReader;
class WavWriter;

class Demodulator {
public:
    explicit Demodulator(const std::string& iqSourceWav, const std::string& outputWav, int carrierFrequency, Sideband sideband);
    void setFFTSize(uint32_t bins);
    uint32_t getFFTSize() const;
    void demodulate();

private:
    bool fillFFTBufferWithSourceIQSamples();
    void performFFT();
    void dropUnneededSideBand();
    void lowPassFilter();
    void performInverseFFT();
    void fillAudioBufferFromFFTBuffer(short* buffer);

    std::shared_ptr<WavWriter> wavWriter;
    std::shared_ptr<WavReader> wavReader;
    double carrierPhase;
    double carrierPhaseIncrement;
    uint32_t fftSize;
    std::vector<std::complex<double>> fftBins;
    double samplerate;
    Sideband sidebandToDecode;
    std::shared_ptr<short> audioBuffer;
};

