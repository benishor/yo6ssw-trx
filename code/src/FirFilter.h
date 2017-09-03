//
// Created by benny on 9/3/17.
//

#pragma once

class FirFilter {
public:
    FirFilter(int taps, double* coefficients);
    virtual ~FirFilter();

    double process(double input);

private:
    int taps;
    double* z;
    double* h;
};

class SsbFilter : public FirFilter {
public:
    explicit SsbFilter();
};

class CwFilter : public FirFilter {
public:
    explicit CwFilter();
};

class CwFilter2 : public FirFilter {
public:
    explicit CwFilter2();
};

