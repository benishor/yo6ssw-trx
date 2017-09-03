cmake_minimum_required(VERSION 3.6)
project(demodulator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        src/AudioSettings.h
        src/Oscillator.cpp
        src/Oscillator.h
        src/SoundServer.cpp
        src/SoundServer.h
        src/WavWriter.h
        src/WavWriter.cpp
        src/FftComplex.cpp
        src/FftComplex.h
        src/RawFileSource.cpp
        src/RawFileSource.h
        src/Demodulator.cpp
        src/Demodulator.h
        src/WavReader.cpp
        src/WavReader.h src/FirFilter.cpp src/FirFilter.h)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)

include_directories(.)
include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDL_MIXER_INCLUDE_DIR})

add_executable(demodulator ${SOURCE_FILES})
target_link_libraries(demodulator m ${SDL_LIBRARY} ${SDLMIXER_LIBRARIES})