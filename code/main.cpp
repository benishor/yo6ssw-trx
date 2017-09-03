#include <iostream>
#include <src/Demodulator.h>
#include <src/WavReader.h>
#include <iomanip>
#include <src/SoundServer.h>
#include <SDL.h>

const std::string OUTPUT_WAV_FILE = "data/demodulation-output.wav";
const int CARRIER_FREQUENCY = 10;
const Sideband SIDEBAND_TO_DECODE = Sideband::LSB;


class Radio : public SoundProducer {
public:
    explicit Radio(std::string iqSource) {
        demodulator = std::make_shared<Demodulator>(iqSource, OUTPUT_WAV_FILE, CARRIER_FREQUENCY, SIDEBAND_TO_DECODE);
        std::cout << "Tuned to " << demodulator->getCarrierFrequency() << std::endl;
    }

    void render(int16_t* buffer, uint16_t numberOfSampleFrames) override {
        demodulator->demodulateInto(buffer, numberOfSampleFrames);
    }

    void tuneUp() {
        if (demodulator->getCarrierFrequency() < demodulator->getSamplerate() / 2) {
            demodulator->setCarrierFrequency(static_cast<int>(demodulator->getCarrierFrequency() + 10));
            std::cout << "Tuned to " << demodulator->getCarrierFrequency() << std::endl;
        }
    }

    void tuneDown() {
        if (demodulator->getCarrierFrequency() > 10) {
            demodulator->setCarrierFrequency(static_cast<int>(demodulator->getCarrierFrequency() - 10));
            std::cout << "Tuned to " << demodulator->getCarrierFrequency() << std::endl;
        }
    }

    uint16_t getSamplerate() {
        return static_cast<uint16_t>(demodulator->getSamplerate());
    }

    void toggleSideband() {
        demodulator->setSidebandToDecode(demodulator->getSidebandToDecode() == Sideband::USB ? Sideband::LSB : Sideband::USB);
        std::cout << "Sideband set to " << (demodulator->getSidebandToDecode() == Sideband::USB ? "USB" : "LSB") << std::endl;
    }

    void toggleFilter() {
        demodulator->nextFilter();
        std::cout << "Switched to filter " << demodulator->getFilterName() << std::endl;
    }

private:
    std::shared_ptr<Demodulator> demodulator;
};

void usage();

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
    }

    try {
        Radio radio(argv[1]);
        SoundServer soundServer(radio.getSamplerate(), 2048, radio);

        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window* window = nullptr;
        SDL_Surface* screenSurface = nullptr;
        window = SDL_CreateWindow("Radio", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN);
        screenSurface = SDL_GetWindowSurface(window);
        SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0x12, 0x12, 0x12));
        SDL_UpdateWindowSurface(window);

        bool quit = false;
        //Event handler
        SDL_Event e{};
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_UP:
                            radio.tuneUp();
                            break;
                        case SDLK_DOWN:
                            radio.tuneDown();
                            break;
                        case SDLK_LEFT:
                            radio.toggleSideband();
                            break;
                        case SDLK_RIGHT:
                            radio.toggleFilter();
                            break;
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        SDL_DestroyWindow(window);
        SDL_Quit();

    } catch (std::string& e) {
        std::cerr << "Caught exception: " << e << std::endl;
        return 1;
    }

    return 0;
}

void usage() {
    std::cerr << "Usage: demodulator iq-filename.wav" << std::endl;
    exit(1);
}
