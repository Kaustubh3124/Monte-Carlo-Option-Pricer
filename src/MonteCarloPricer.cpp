#include "../include/MonteCarloPricer.hpp"
#include <cmath>
#include <random>

MonteCarloPricer::MonteCarloPricer(int _numSimulations, int _numTimeSteps)
    : numSimulations(_numSimulations), numTimeSteps(_numTimeSteps) {}

double MonteCarloPricer::price(const Option& option, const MarketEnvironment& env) const {
    double dt = option.getMaturity() / numTimeSteps;
    
    // Math pre-calculations for the GBM formula
    double drift = (env.riskFreeRate - 0.5 * env.volatility * env.volatility) * dt;
    double volSqrtDt = env.volatility * std::sqrt(dt);

    // Setup Mersenne Twister RNG
    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> standardNormal(0.0, 1.0);

    double payoffSum = 0.0;

    for (int i = 0; i < numSimulations; ++i) {
        std::vector<double> pricePath(numTimeSteps + 1);
        pricePath[0] = env.spotPrice;

        // Generate the price path using GBM
        for (int t = 1; t <= numTimeSteps; ++t) {
            double Z = standardNormal(generator);
            pricePath[t] = pricePath[t - 1] * std::exp(drift + volSqrtDt * Z);
        }

        // Calculate payoff for this specific path
        payoffSum += option.getPayoff(pricePath);
    }

    // Average the payoffs and discount back to Present Value
    double expectedPayoff = payoffSum / numSimulations;
    return expectedPayoff * std::exp(-env.riskFreeRate * option.getMaturity());
}