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
class FirFilter;

class Demodulator {
public:
    explicit Demodulator(const std::string& iqSourceWav, const std::string& outputWav, int carrierFrequency, Sideband sideband);
    void demodulate();
    double getSamplerate() const;
    double getCarrierFrequency() const;
    Sideband getSidebandToDecode() const;
    void setSidebandToDecode(Sideband sidebandToDecode);
    void demodulateInto(short* buffer, int howManySamples);
    void setCarrierFrequency(int carrierFrequency);
    void nextFilter();
    std::string getFilterName() const;

private:
    bool readComplexSamplesAndDownmix();
    void convoluteAndFillAudioBuffer(short* audioBuffer);

    std::shared_ptr<WavWriter> wavWriter;
    std::shared_ptr<WavReader> wavReader;
    int filterIndex = 0;
    std::shared_ptr<FirFilter> filters[3];
    std::vector<std::complex<double>> complexSamples;
    std::string outputWavFilename;
    double carrierFrequency;
    double carrierPhase;
    double carrierPhaseIncrement;
    double samplerate;
    Sideband sidebandToDecode;
};

