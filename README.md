# 📈 C++ Monte Carlo Options Pricing Engine

## Overview
This project is an object-oriented C++ derivatives pricing engine. It uses Monte Carlo simulations and Geometric Brownian Motion (GBM) to price both standard European options and complex, path-dependent Asian options.

## Architecture
The system is built with strict OOP principles for modularity and speed:
* **`Option` Base Class:** Defines the core interface and contract for derivatives.
* **Derived Classes (`EuropeanOptions`, `ExoticOptions`):** Implement specific mathematical payoff logic.
* **`MonteCarloPricer`:** The core engine that simulates asset price paths and discounts expected payoffs to present value.
* **`MarketEnvironment`:** A struct managing spot price, volatility, and risk-free rates.

## Financial Math (Geometric Brownian Motion)
The engine simulates potential future stock prices using the discrete-time GBM formula to generate price paths:
`S(t+dt) = S(t) * exp((r - 0.5 * vol^2) * dt + vol * sqrt(dt) * Z)`
*(Where `Z` is a random variable drawn from a standard normal distribution).*

## Build and Run
Compiled using standard C++17 optimizations (`-O3` flag for maximum execution speed):
```bash
g++ -O3 -std=c++17 src/main.cpp src/MonteCarloPricer.cpp -o QuantPricer
QuantPricer.exe