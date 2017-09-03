#include "Demodulator.h"
#include "FftComplex.h"
#include <src/WavWriter.h>
#include <src/WavReader.h>
#include <src/FirFilter.h>

using namespace std;

const uint32_t DEFAULT_FFT_BINS = 1024;

Demodulator::Demodulator(const std::string& iqSourceWav, const std::string& outputWav, int carrierFrequency, Sideband sideband) {
    wavReader = make_shared<WavReader>(iqSourceWav);
    samplerate = wavReader->getHeader().sampleRate;

    AudioSettings audioSettings;
    audioSettings.sampleRate = (int) samplerate;
    audioSettings.channels = 2;

    wavWriter = make_shared<WavWriter>(audioSettings, outputWav);

    filter = make_shared<CwFilter2>();

    this->carrierFrequency = carrierFrequency;
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
//        performFFT();
//        dropUnneededSideBand();
//        lowPassFilter();
//        performInverseFFT();
//        fillAudioBufferFromFFTBuffer(audioBuffer);
        convoluteAndFillAudioBuffer(audioBuffer);
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
        auto sourceSample = complex<double>(iqSamples[i*2+0],
                                            iqSamples[i*2+1]);

        carrierPhase += carrierPhaseIncrement;
        carrierPhase = fmod(carrierPhase, M_PI * 2.0);
        auto mixedSample = complex<double>(sourceSample.real() * 100 * cos(carrierPhase),
                                           sourceSample.imag() * 100 * sin(carrierPhase));

        fftBins.push_back(mixedSample);
    }
    return true;
}

void Demodulator::performFFT() {
    Fft::transform(fftBins);
}

void Demodulator::dropUnneededSideBand() {
        const double HZ_PER_BIN =  samplerate / fftSize;
        const int BANDWIDTH_IN_HZ = 3800;

        int binOfCarrier = (int)(carrierFrequency / HZ_PER_BIN);
        int bandwidthInBins = (int)(BANDWIDTH_IN_HZ / HZ_PER_BIN);

        // exchange positive with negative frequencies
//        for (int i = 1; i < fftSize/2; i++) {
//            complex<double> tmp = fftBins[i];
//            fftBins[i] = fftBins[fftSize - i];
//            fftBins[fftSize - i] = tmp;
//        }


        // shift frequency content to bring desired transmission in baseband
        int startingBin = binOfCarrier + 1;
        int endingBin = startingBin + bandwidthInBins;

        for (int i = 0; i < bandwidthInBins; i++) {
            fftBins[i + 1] = fftBins[startingBin + i];
        }

        // invert spectrum
//        for (int i = 0; i < bandwidthInBins/2; i++) {
//            complex<double> tmp = fftBins[i+1];
//            fftBins[i+1] = fftBins[bandwidthInBins-i];
//            fftBins[bandwidthInBins-i] = tmp;
//        }

        // wipe out everything else
        for (int i = bandwidthInBins; i < fftSize; i++) {
            fftBins[i] = complex<double>(0, 0);
        }

//    if (sidebandToDecode == Sideband::USB) {
//        // drop negative frequencies
//        for (int i = 1; i <= fftSize / 2; i++) {
//            fftBins[i] = complex<double>(0, 0);
//        }
//    } else {
//        // drop positive frequencies
//        for (int i = fftSize / 2 + 1; i < fftSize; i++) {
//            fftBins[i] = complex<double>(0, 0);
//        }
//    }
}

void Demodulator::lowPassFilter() {
//    int howManyBinsToKeep = 300; // experimentally set
//    for (int i = (1 + howManyBinsToKeep); i <= fftSize / 2; i++) {
//        fftBins[i] = complex<double>(0, 0);
//    }
//    for (int i = (fftSize / 2 + 1); i < (fftSize - howManyBinsToKeep); i++) {
//        fftBins[i] = complex<double>(0, 0);
//    }
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
//        short sample = (short) ((fftBins[i].real() + fftBins[i].imag()) * 60000);
//        short sample = (short) ((fftBins[i].real()) * 60000);
        short sample = (short) (abs(fftBins[i]));
        buffer[offset++] = sample;
        buffer[offset++] = sample;
    }
}

uint32_t Demodulator::getFFTSize() const {
    return fftSize;
}

void Demodulator::convoluteAndFillAudioBuffer(short* buffer) {
    int offset = 0;
    for (int i = 0; i < fftSize; i++) {
        double realSample = fftBins[i].real() + fftBins[i].imag(); // sideband selection?
        short sample = (short) (filter->process(realSample));
        buffer[offset++] = sample;
        buffer[offset++] = sample;
    }
}
