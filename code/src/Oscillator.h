//
// Created by benny on 11/30/16.
//

#ifndef EMUSIC_OSCILLATOR_H
#define EMUSIC_OSCILLATOR_H

#include <cmath>

class MathConstants {
public:
    constexpr static double PI = 3.1415926535;
    constexpr static double TWO_PI = 2.0 * PI;
    constexpr static double HALF_PI = 0.5 * PI;
};

/// The available waveforms that the Oscillator can output
enum class OscillatorType {
    SINE,
    SAW,
    SQUARE,
    TRIANGLE
};

/// The basic unit of producing sound.
class Oscillator {
public:
    Oscillator(double sr);

    /// Sets the waveform type this oscillator will generate
    /// @param [in] type the new waveform type
    void setType(OscillatorType type);

    /// Sets the frequency of this oscillator
    /// @param [in] frequency the new frequency. positive
    void setFrequency(double frequency);

    /// Sets the phase of this oscillator
    /// @param [in] phase the phase of the oscillator, in [0..2*PI]
    void setPhase(double phase);

    /// Ticks the oscillator and returns one sample with respect to its phase and oscillator type.
    /// The sample belongs to [-1..1] interval.
    /// @return the next sample of this oscillator, a value belonging to [-1.0..1.0]
    double tick();

    /// Fills the provided buffer with the
    void fill(double* buffer, int lengthInSamples);

private:
    double sampleRate;
    OscillatorType type;
    double phaseIncrement;
    double phase;

};


#endif //EMUSIC_OSCILLATOR_H
