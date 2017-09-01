#include <iostream>
#include <src/Demodulator.h>
#include <src/WavReader.h>

const std::string IQ_SOURCE_WAV_FILE = "data/ssb-iq.wav";
const std::string OUTPUT_WAV_FILE = "data/demodulation-output.wav";
const int CARRIER_FREQUENCY = 650;
const Sideband SIDEBAND_TO_DECODE = Sideband::LSB;

int main() {
    try {
        std::cout << "Demodulating IQ source file " << IQ_SOURCE_WAV_FILE << " into " << OUTPUT_WAV_FILE << " using a carrier at " << CARRIER_FREQUENCY << "Hz." << std::endl;
        Demodulator demodulator(IQ_SOURCE_WAV_FILE, OUTPUT_WAV_FILE, CARRIER_FREQUENCY, SIDEBAND_TO_DECODE);
        demodulator.demodulate();
        std::cout << "Done." << std::endl;
    } catch (std::string e) {
        std::cerr << "Caught exception: " << e << std::endl;
    }

    return 0;
}
