#include <iostream>
#include <iomanip>
#include "../include/MarketEnvironment.hpp"
#include "../include/EuropeanOptions.hpp"
#include "../include/ExoticOptions.hpp"
#include "../include/MonteCarloPricer.hpp"

int main() {
    MarketEnvironment env = {100.0, 0.05, 0.20}; 
    EuropeanCall eurCall(100.0, 1.0);
    MonteCarloPricer pricer(1000000, 252);

    std::cout << "Running Monte Carlo Simulation & Risk Analysis...\n\n";

    double price = pricer.price(eurCall, env);
    double delta = pricer.calculateDelta(eurCall, env);
    double gamma = pricer.calculateGamma(eurCall, env);
    double vega = pricer.calculateVega(eurCall, env);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "--- European Call Analysis ---\n";
    std::cout << "Price: $" << price << "\n";
    std::cout << "Delta:  " << delta << " (Sensitivity to Spot)\n";
    std::cout << "Gamma:  " << gamma << " (Rate of Delta change)\n";
    std::cout << "Vega:   " << vega << " (Sensitivity to Vol)\n";

    return 0;
}