//
// Created by benny on 8/29/17.
//

#include "RawFileSource.h"

using namespace std;

RawFileSource::RawFileSource(string filename) {
    file = ifstream(filename, ios::binary);
    if (!file) throw "Could not open file " + filename + " for reading";
}

RawFileSource::~RawFileSource() {
    file.close();
}

complex<double> RawFileSource::tick() {
    short i, q;
    file.read((char*) &i, sizeof(short));
    file.read((char*) &q, sizeof(short));
    // convert from [-32767,32767] signed integers to [-1.0, 1.0] doubles
    double realI = i / 32767.0;
    double realQ = q / 32767.0;
    return complex<double>(realI, realQ);
}
