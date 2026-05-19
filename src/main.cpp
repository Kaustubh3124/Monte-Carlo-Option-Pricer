#include <iostream>
#include <iomanip>
#include "../include/MarketEnvironment.hpp"
#include "../include/EuropeanOptions.hpp"
#include "../include/ExoticOptions.hpp" // <-- Include your new file here
#include "../include/MonteCarloPricer.hpp"

int main() {
    // 1. Setup Market Environment (Spot, Rate, Volatility)
    MarketEnvironment env = {100.0, 0.05, 0.20}; 

    // 2. Instantiate Options (Strike, Maturity)
    EuropeanCall eurCall(100.0, 1.0);
    EuropeanPut eurPut(100.0, 1.0);
    AsianCall asianCall(100.0, 1.0);    // <-- Instantiate the new Asian Call
    AsianPut asianPut(100.0, 1.0);      // <-- Instantiate the new Asian Put

    // 3. Setup the Pricer (1 million paths, 252 trading days)
    MonteCarloPricer pricer(1000000, 252);

    std::cout << "Running Monte Carlo Simulation (1,000,000 paths)...\n";
    std::cout << "Pricing European and Asian Options...\n\n";

    // 4. Calculate Prices
    double eurCallPrice = pricer.price(eurCall, env);
    double eurPutPrice = pricer.price(eurPut, env);
    double asianCallPrice = pricer.price(asianCall, env); // <-- Price it
    double asianPutPrice = pricer.price(asianPut, env);   // <-- Price it

    // 5. Output Results
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "--- European Options ---\n";
    std::cout << "Call Price: $" << eurCallPrice << "\n";
    std::cout << "Put Price:  $" << eurPutPrice << "\n\n";

    std::cout << "--- Asian Options (Path Dependent) ---\n";
    std::cout << "Call Price: $" << asianCallPrice << "\n";
    std::cout << "Put Price:  $" << asianPutPrice << "\n";

    return 0;
}