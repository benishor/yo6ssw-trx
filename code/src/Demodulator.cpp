#include "Demodulator.h"
#include <src/WavWriter.h>
#include <src/WavReader.h>
#include <src/FirFilter.h>
#include <cstring>

using namespace std;

Demodulator::Demodulator(const std::string& iqSourceWav, const std::string& outputWav, int carrierFrequency, Sideband sideband) {
    wavReader = make_shared<WavReader>(iqSourceWav);
    samplerate = wavReader->getHeader().sampleRate;
    outputWavFilename = outputWav;

    filters[0] = make_shared<SsbFilter>();
    filters[1] = make_shared<CwFilter>();
    filters[2] = make_shared<CwFilter2>();

    setCarrierFrequency(carrierFrequency);
    carrierPhase = 0;
    sidebandToDecode = sideband;
}

void Demodulator::demodulate() {
    AudioSettings audioSettings;
    audioSettings.sampleRate = (int) samplerate;
    audioSettings.channels = 2;
    wavWriter = make_shared<WavWriter>(audioSettings, outputWavFilename);

    auto audioBuffer = new short[1024 * 2];

    wavWriter->begin();
    while (readComplexSamplesAndDownmix()) {
        convoluteAndFillAudioBuffer(audioBuffer);
        wavWriter->write(audioBuffer, 1024);
    }
    wavWriter->end();
}

bool Demodulator::readComplexSamplesAndDownmix() {
    static short* iqSamples = nullptr;
    if (iqSamples == nullptr) {
        iqSamples = new short[1024 * 2];
    }

    if (wavReader->readData((char*) iqSamples, 1024 * 4) != 1024 * 4)
        return false;

    complexSamples.clear();
    for (int i = 0; i < 1024; i++) {
        auto sourceSample = complex<double>(iqSamples[i * 2 + 0],
                                            iqSamples[i * 2 + 1]);

        carrierPhase += carrierPhaseIncrement;
        carrierPhase = fmod(carrierPhase, M_PI * 2.0);
        auto mixedSample = complex<double>(sourceSample.real() * 100 * cos(carrierPhase),
                                           sourceSample.imag() * 100 * sin(carrierPhase));

        complexSamples.push_back(mixedSample);
    }
    return true;
}

void Demodulator::convoluteAndFillAudioBuffer(short* buffer) {
    int offset = 0;
    for (int i = 0; i < 1024; i++) {
        double realSample = sidebandToDecode == Sideband::USB ?
                            complexSamples[i].real() + complexSamples[i].imag() :
                            complexSamples[i].imag() - complexSamples[i].real();
        auto sample = (short) (filters[filterIndex]->process(realSample));
        buffer[offset++] = sample;
        buffer[offset++] = sample;
    }
}

void Demodulator::demodulateInto(short* buffer, int howManySamples) {
    // read samples
    if (wavReader->readData((char*) buffer, static_cast<unsigned int>(howManySamples * 4)) != howManySamples * 4) {
        wavReader->rewind();
        memset(buffer, 0, static_cast<size_t>(howManySamples * 4));
        return;
    }

    // downmix
    complexSamples.clear();
    for (int i = 0; i < howManySamples; i++) {
        auto sourceSample = complex<double>(buffer[i * 2 + 0],
                                            buffer[i * 2 + 1]);

        carrierPhase += carrierPhaseIncrement;
        carrierPhase = fmod(carrierPhase, M_PI * 2.0);
        auto mixedSample = complex<double>(sourceSample.real() * 100 * cos(carrierPhase),
                                           sourceSample.imag() * 100 * sin(carrierPhase));

        complexSamples.push_back(mixedSample);
    }

    // filter and obtain audio
    int offset = 0;
    for (int i = 0; i < howManySamples; i++) {
        double realSample = sidebandToDecode == Sideband::USB ?
                            complexSamples[i].real() + complexSamples[i].imag() :
                            complexSamples[i].imag() - complexSamples[i].real();
        auto sample = (short) (filters[filterIndex]->process(realSample));
        buffer[offset++] = sample;
        buffer[offset++] = sample;
    }
}

void Demodulator::setCarrierFrequency(int carrierFrequency) {
    this->carrierFrequency = carrierFrequency;
    carrierPhaseIncrement = 2.0 * M_PI * carrierFrequency / samplerate;
}

double Demodulator::getSamplerate() const {
    return samplerate;
}

double Demodulator::getCarrierFrequency() const {
    return carrierFrequency;
}

Sideband Demodulator::getSidebandToDecode() const {
    return sidebandToDecode;
}

void Demodulator::setSidebandToDecode(Sideband sidebandToDecode) {
    Demodulator::sidebandToDecode = sidebandToDecode;
}

void Demodulator::nextFilter() {
    filterIndex++;
    filterIndex %= 3;
}

std::string Demodulator::getFilterName() const {
    switch (filterIndex) {
        case 0 :
            return "SSB";
        case 1 :
            return "CW-1";
        case 2 :
            return "CW-2";
        default:
            return "unknown";
    }
}
