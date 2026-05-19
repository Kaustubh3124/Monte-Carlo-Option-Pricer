#include "../include/MonteCarloPricer.hpp"
#include <cmath>
#include <random>

MonteCarloPricer::MonteCarloPricer(int _numSimulations, int _numTimeSteps)
    : numSimulations(_numSimulations), numTimeSteps(_numTimeSteps) {}

double MonteCarloPricer::price(const Option& option, const MarketEnvironment& env) const {
    double dt = option.getMaturity() / numTimeSteps;
    double drift = (env.riskFreeRate - 0.5 * env.volatility * env.volatility) * dt;
    double volSqrtDt = env.volatility * std::sqrt(dt);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> standardNormal(0.0, 1.0);

    double payoffSum = 0.0;
    for (int i = 0; i < numSimulations; ++i) {
        std::vector<double> pricePath(numTimeSteps + 1);
        pricePath[0] = env.spotPrice;

        for (int t = 1; t <= numTimeSteps; ++t) {
            double Z = standardNormal(generator);
            pricePath[t] = pricePath[t - 1] * std::exp(drift + volSqrtDt * Z);
        }
        payoffSum += option.getPayoff(pricePath);
    }

    double expectedPayoff = payoffSum / numSimulations;
    return expectedPayoff * std::exp(-env.riskFreeRate * option.getMaturity());
}

// Delta: Sensitivity to Spot Price change
double MonteCarloPricer::calculateDelta(const Option& option, const MarketEnvironment& env) const {
    double epsilon = 0.01 * env.spotPrice; // 1% bump
    MarketEnvironment envUp = env; envUp.spotPrice += epsilon;
    MarketEnvironment envDown = env; envDown.spotPrice -= epsilon;
    
    return (price(option, envUp) - price(option, envDown)) / (2 * epsilon);
}

// Gamma: Rate of change in Delta (Second derivative)
double MonteCarloPricer::calculateGamma(const Option& option, const MarketEnvironment& env) const {
    double epsilon = 0.01 * env.spotPrice;
    MarketEnvironment envUp = env; envUp.spotPrice += epsilon;
    MarketEnvironment envDown = env; envDown.spotPrice -= epsilon;

    double pCenter = price(option, env);
    return (price(option, envUp) - 2 * pCenter + price(option, envDown)) / (epsilon * epsilon);
}

// Vega: Sensitivity to Volatility change
double MonteCarloPricer::calculateVega(const Option& option, const MarketEnvironment& env) const {
    double epsilon = 0.01; // 1% volatility bump
    MarketEnvironment envUp = env; envUp.volatility += epsilon;
    MarketEnvironment envDown = env; envDown.volatility -= epsilon;

    return (price(option, envUp) - price(option, envDown)) / (2 * epsilon);
}