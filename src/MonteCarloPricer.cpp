#include "../include/MonteCarloPricer.hpp"
#include <cmath>
#include <random>

// Constructor initializes simulation precision parameters
MonteCarloPricer::MonteCarloPricer(int _numSimulations, int _numTimeSteps)
    : numSimulations(_numSimulations), numTimeSteps(_numTimeSteps) {}

double MonteCarloPricer::price(const Option& option, const MarketEnvironment& env) const {
    double dt = option.getMaturity() / numTimeSteps;
    
    // Pre-calculating drift and volatility components to optimize the inner simulation loop
    double drift = (env.riskFreeRate - 0.5 * env.volatility * env.volatility) * dt;
    double volSqrtDt = env.volatility * std::sqrt(dt);

    // Using Mersenne Twister for high-quality pseudo-random number generation
    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> standardNormal(0.0, 1.0);

    double payoffSum = 0.0;
    for (int i = 0; i < numSimulations; ++i) {
        std::vector<double> pricePath(numTimeSteps + 1);
        pricePath[0] = env.spotPrice;

        // Simulating the underlying asset path using discrete-time Geometric Brownian Motion
        for (int t = 1; t <= numTimeSteps; ++t) {
            double Z = standardNormal(generator);
            pricePath[t] = pricePath[t - 1] * std::exp(drift + volSqrtDt * Z);
        }
        // Polymorphic call to the specific option's payoff logic
        payoffSum += option.getPayoff(pricePath);
    }

    // Calculating the risk-neutral expected payoff and discounting to present value
    double expectedPayoff = payoffSum / numSimulations;
    return expectedPayoff * std::exp(-env.riskFreeRate * option.getMaturity());
}

/*
 * Risk Metrics (The Greeks)
 * Calculated using the Central Finite Difference Method.
 * These measure price sensitivity to various market parameters.
 */

// Delta: First-order derivative of option price with respect to underlying asset price
double MonteCarloPricer::calculateDelta(const Option& option, const MarketEnvironment& env) const {
    double epsilon = 0.01 * env.spotPrice; 
    MarketEnvironment envUp = env; envUp.spotPrice += epsilon;
    MarketEnvironment envDown = env; envDown.spotPrice -= epsilon;
    
    return (price(option, envUp) - price(option, envDown)) / (2 * epsilon);
}

// Gamma: Second-order derivative of option price with respect to underlying asset price
double MonteCarloPricer::calculateGamma(const Option& option, const MarketEnvironment& env) const {
    double epsilon = 0.01 * env.spotPrice;
    MarketEnvironment envUp = env; envUp.spotPrice += epsilon;
    MarketEnvironment envDown = env; envDown.spotPrice -= epsilon;

    double pCenter = price(option, env);
    return (price(option, envUp) - 2 * pCenter + price(option, envDown)) / (epsilon * epsilon);
}

// Vega: Sensitivity of the option price to a 1% change in underlying volatility
double MonteCarloPricer::calculateVega(const Option& option, const MarketEnvironment& env) const {
    double epsilon = 0.01; 
    MarketEnvironment envUp = env; envUp.volatility += epsilon;
    MarketEnvironment envDown = env; envDown.volatility -= epsilon;

    return (price(option, envUp) - price(option, envDown)) / (2 * epsilon);
}