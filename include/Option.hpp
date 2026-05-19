#ifndef OPTION_HPP
#define OPTION_HPP

#include <vector>

class Option {
protected:
    double strike;
    double timeToMaturity;

public:
    Option(double _strike, double _timeToMaturity) 
        : strike(_strike), timeToMaturity(_timeToMaturity) {}
    
    virtual ~Option() = default;

    // We pass a vector of prices to support path-dependent options
    virtual double getPayoff(const std::vector<double>& pricePath) const = 0;
    
    double getMaturity() const { return timeToMaturity; }
    double getStrike() const { return strike; }
};

#endif