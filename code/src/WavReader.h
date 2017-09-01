//
// Created by benny on 8/31/17.
//
#pragma once

#include <string>
#include <fstream>

struct WavHeader {
    uint32_t dataLength = 0;
    int format = 0;
    int sampleRate = 0;
    int bitsPerSample = 0;
    int channels = 0;
    int byteRate = 0;
    int blockAlign = 0;
    int streamed = 0;
};

class WavReader {
public:
    explicit WavReader(const std::string filename);
    virtual ~WavReader();

    WavHeader getHeader() const { return header; };
    int readData(char* where, unsigned int numberOfBytes);
    void close();

private:
    std::ifstream file;
    WavHeader header;

    void readHeader();
    uint32_t readTag();
    uint32_t readUInt32();
    uint16_t readUInt16();
};
