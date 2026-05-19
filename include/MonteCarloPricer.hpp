#ifndef MONTE_CARLO_PRICER_HPP
#define MONTE_CARLO_PRICER_HPP

#include "Option.hpp"
#include "MarketEnvironment.hpp"

class MonteCarloPricer {
private:
    int numSimulations;
    int numTimeSteps;

public:
    MonteCarloPricer(int _numSimulations, int _numTimeSteps);
    
    // Core pricing function
    double price(const Option& option, const MarketEnvironment& env) const;

    // Risk Metrics (The Greeks) calculated via Finite Difference Methods
    double calculateDelta(const Option& option, const MarketEnvironment& env) const;
    double calculateGamma(const Option& option, const MarketEnvironment& env) const;
    double calculateVega(const Option& option, const MarketEnvironment& env) const;
};

#endif