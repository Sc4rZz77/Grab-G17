# Grab Fare Estimator (LDCW6123 Group Project, Part 2)

A console C++ program that estimates a Grab-style **upfront fare**.
It links to Part 1 (Christensen disruptive-innovation poster on Grab): Grab disrupted
metered taxis with upfront app pricing and new tiers (GrabBike, GrabCar Plus).

## Build and run
    g++ -std=c++11 -Wall -Wextra -o grabfare main.cpp
    ./grabfare          (Windows: grabfare.exe)

## Features
- Menu with 3 services (switch), time-period multiplier (if/else), promo code GRAB10 (10%, max RM5)
- Input validation for every prompt (letters, symbols, empty, out of range)
- "Estimate another fare?" loop, on-screen instructions
- Illustrative metered-taxi comparison to show the upfront-price idea

All rates are illustrative assumptions, not real Grab prices.

## Tests
    BIN=./grabfare bash tests/run_tests.sh      -> writes tests/test_results.txt
See TEST_TABLE.md for the 9 cases.

