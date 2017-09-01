//
// Created by benny on 8/29/17.
//
#pragma once

#include <string>
#include <fstream>
#include <complex>

struct RawFileSource {
    RawFileSource(std::string filename);
    ~RawFileSource();
    std::complex<double> tick();

private:
    std::ifstream file;
};
