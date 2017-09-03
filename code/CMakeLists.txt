cmake_minimum_required(VERSION 3.6)
project(demodulator)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${demodulator_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

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

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# SDL mixer setup
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

include_directories(.)

add_executable(demodulator ${SOURCE_FILES})
target_link_libraries(demodulator m ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY} ${SDL2_MIXER_LIBRARIES} )