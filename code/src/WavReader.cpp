//
// Created by benny on 8/31/17.
//

#include <iostream>
#include "WavReader.h"

// adapted from https://github.com/mstorsjo/fdk-aac/blob/master/wavreader.c

using namespace std;

#define TAG(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

WavReader::WavReader(const std::string filename) {
    file = ifstream(filename, ios::binary);
    if (!file) throw "Failed to open " + filename + " for reading";
    readHeader();
}

WavReader::~WavReader() {
    close();
}

void WavReader::close() {
    if (file) {
        file.close();
    }
}

uint32_t WavReader::readTag() {
    uint32_t tag = 0;
    tag = (tag << 8) | file.get();
    tag = (tag << 8) | file.get();
    tag = (tag << 8) | file.get();
    tag = (tag << 8) | file.get();
    return tag;
}

uint32_t WavReader::readUInt32() {
    uint32_t value = 0;
    value |= file.get() << 0;
    value |= file.get() << 8;
    value |= file.get() << 16;
    value |= file.get() << 24;
    return value;
}

uint16_t WavReader::readUInt16() {
    uint16_t value = 0;
    value |= file.get() << 0;
    value |= file.get() << 8;
    return value;
}

void WavReader::readHeader() {
    long dataPosition = 0;

    while (1) {
        uint32_t tag, tag2, length;
        tag = readTag();
        if (file.eof())
            break;
        length = readUInt32();
        if (!length || length >= 0x7fff0000) {
            header.streamed = 1;
            length = ~0;
        }

        if (tag != TAG('R', 'I', 'F', 'F') || length < 4) {
            file.seekg(length, ios_base::cur);
            continue;
        }

        tag2 = readTag();
        length -= 4;
        if (tag2 != TAG('W', 'A', 'V', 'E')) {
            file.seekg(length, ios_base::cur);
            continue;
        }

        // RIFF chunk found, iterate through it
        while (length >= 8) {
            uint32_t subtag, sublength;
            subtag = readTag();
            if (file.eof())
                break;
            sublength = readUInt32();
            length -= 8;
            if (length < sublength)
                break;
            if (subtag == TAG('f', 'm', 't', ' ')) {
                if (sublength < 16) {
                    // Insufficient data for 'fmt '
                    break;
                }
                header.format = readUInt16();
                header.channels = readUInt16();
                header.sampleRate = readUInt32();
                header.byteRate = readUInt32();
                header.blockAlign = readUInt16();
                header.bitsPerSample = readUInt16();
                if (header.format == 0xfffe) {
                    if (sublength < 28) {
                        // Insufficient data for waveformatex
                        break;
                    }
                    file.seekg(8, ios_base::cur);
                    header.format = readUInt32();
                    file.seekg(sublength - 28, ios_base::cur);
                } else {
                    file.seekg(sublength - 16, ios_base::cur);
                }
            } else if (subtag == TAG('d', 'a', 't', 'a')) {
                dataPosition = file.tellg();
                header.dataLength = sublength;
                if (!header.dataLength || header.streamed) {
                    header.streamed = 1;
                    return;
                }
                file.seekg(sublength, ios_base::cur);
            } else {
                file.seekg(sublength, ios_base::cur);
            }
            length -= sublength;
        }
        if (length > 0) {
            // Bad chunk?
            file.seekg(length, ios_base::cur);
        }
    }
    file.clear(); // clear previous eof
    file.seekg(dataPosition, ios_base::beg);
}

int WavReader::readData(char* where, unsigned int numberOfBytes) {
    int bytesToRead = numberOfBytes;
    if (bytesToRead > header.dataLength && !header.streamed)
        bytesToRead = header.dataLength;

    file.read(where, bytesToRead);
    streamsize readBytes = file.gcount();

    header.dataLength -= numberOfBytes;
    return (int) readBytes;
}
