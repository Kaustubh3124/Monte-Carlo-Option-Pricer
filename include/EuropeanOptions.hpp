#ifndef EUROPEAN_OPTIONS_HPP
#define EUROPEAN_OPTIONS_HPP

#include "Option.hpp"
#include <algorithm>

class EuropeanCall : public Option {
public:
    EuropeanCall(double strike, double timeToMaturity) : Option(strike, timeToMaturity) {}

    double getPayoff(const std::vector<double>& pricePath) const override {
        double finalPrice = pricePath.back();
        return std::max(finalPrice - strike, 0.0);
    }
};

class EuropeanPut : public Option {
public:
    EuropeanPut(double strike, double timeToMaturity) : Option(strike, timeToMaturity) {}

    double getPayoff(const std::vector<double>& pricePath) const override {
        double finalPrice = pricePath.back();
        return std::max(strike - finalPrice, 0.0);
    }
};

#endif