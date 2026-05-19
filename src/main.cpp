#include <iostream>
#include <iomanip>
#include "../include/MarketEnvironment.hpp"
#include "../include/EuropeanOptions.hpp"
#include "../include/MonteCarloPricer.hpp"

int main() {
    // 1. Setup Market Environment (Spot, Rate, Volatility)
    MarketEnvironment env = {100.0, 0.05, 0.20}; 

    // 2. Instantiate Options (Strike, Maturity)
    EuropeanCall callOption(100.0, 1.0);
    EuropeanPut putOption(100.0, 1.0);

    // 3. Setup the Pricer (1 million paths, 252 trading days)
    MonteCarloPricer pricer(1000000, 252);

    std::cout << "Running Monte Carlo Simulation (1,000,000 paths)...\n";

    // 4. Calculate Prices
    double callPrice = pricer.price(callOption, env);
    double putPrice = pricer.price(putOption, env);

    // 5. Output Results
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "European Call Price: $" << callPrice << "\n";
    std::cout << "European Put Price:  $" << putPrice << "\n";

    return 0;
}