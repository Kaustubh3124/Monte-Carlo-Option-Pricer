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
    
    double price(const Option& option, const MarketEnvironment& env) const;
};

#endif