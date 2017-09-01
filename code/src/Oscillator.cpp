//
// Created by benny on 11/30/16.
//
#include "Oscillator.h"

Oscillator::Oscillator(double sr)
        : sampleRate(sr), type(OscillatorType::SINE) {
}

void Oscillator::setType(OscillatorType type) {
    this->type = type;
}

void Oscillator::setFrequency(double frequency) {
    phaseIncrement = (2 * MathConstants::PI * frequency) / sampleRate;
}

void Oscillator::setPhase(double phase) {
    this->phase = phase;
}

double Oscillator::tick() {
    double oldPhase = phase;
    phase += phaseIncrement;
    phase = fmod(phase, MathConstants::TWO_PI);

    if (type == OscillatorType::SINE)
        return sin(oldPhase);
    else if (type == OscillatorType::SAW)
        return (phase - MathConstants::PI) / MathConstants::PI;
    else if (type == OscillatorType::SQUARE)
        return phase >= MathConstants::PI ? 1.0 : 0.0;
    else if (type == OscillatorType::TRIANGLE)
        return phase >= MathConstants::PI ?
               (((MathConstants::TWO_PI - phase) / MathConstants::PI) - 0.5) * 2.0 :
               ((phase / MathConstants::PI) - 0.5) / 2.0;
    else
        return 0;
}

void Oscillator::fill(double* buffer, int lengthInSamples) {
    while (lengthInSamples--) {
        *buffer++ = tick();
    }
}
