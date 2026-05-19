#ifndef EXOTIC_OPTIONS_HPP
#define EXOTIC_OPTIONS_HPP

#include "Option.hpp"
#include <algorithm>
#include <numeric>

class AsianCall : public Option {
public:
    AsianCall(double strike, double timeToMaturity) : Option(strike, timeToMaturity) {}

    double getPayoff(const std::vector<double>& pricePath) const override {
        // Safety check to prevent division by zero
        if (pricePath.empty()) return 0.0;

        // 1. Calculate the sum of all prices in the path
        double sum = std::accumulate(pricePath.begin(), pricePath.end(), 0.0);

        // 2. Calculate the arithmetic average
        double averagePrice = sum / pricePath.size();

        // 3. Return the standard call payoff using the average price
        return std::max(averagePrice - strike, 0.0);
    }
};

class AsianPut : public Option {
public:
    AsianPut(double strike, double timeToMaturity) : Option(strike, timeToMaturity) {}

    double getPayoff(const std::vector<double>& pricePath) const override {
        if (pricePath.empty()) return 0.0;
        
        double sum = std::accumulate(pricePath.begin(), pricePath.end(), 0.0);
        double averagePrice = sum / pricePath.size();

        // Return the standard put payoff using the average price
        return std::max(strike - averagePrice, 0.0);
    }
};

#endif