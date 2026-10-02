/*
 * ============================================================================
 *  Grab Fare Estimator  (LDCW6123 Group Project - Part 2, Trimester 2620)
 * ============================================================================
 *  Link to Part 1 (Innovation Life Cycle Poster):
 *    Our poster traces Grab using Christensen's disruptive innovation model.
 *    Grab disrupted metered taxis by offering an UPFRONT, app-calculated price
 *    (instead of a meter that runs during the trip) and by adding cheaper tiers
 *    (GrabBike) and premium tiers (GrabCar Plus). This program recreates that
 *    feature: the passenger chooses a service, distance and time period, and
 *    the program shows an upfront fare next to an illustrative taxi-meter fare.
 *
 *  INPUT / OUTPUT PLAN
 *    Inputs : service type (menu), trip distance in km, time period,
 *             optional promo code, "run again?" answer
 *    Outputs: itemised fare (base + distance, time multiplier, booking fee,
 *             promo discount, total) and an illustrative taxi comparison
 *
 *  NOTE: All rates below are ILLUSTRATIVE ASSUMPTIONS for a student project.
 *        They are NOT Grab's real prices.
 * ============================================================================
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cctype>
#include <cmath>
using namespace std;

// ----------------------------- Constants -----------------------------------
const double BOOKING_FEE      = 1.00;   // flat platform fee per trip (RM)
const double PEAK_MULTIPLIER  = 1.50;   // weekday rush hour demand
const double NIGHT_MULTIPLIER = 1.25;   // late night (12am - 5am)
const double NORMAL_MULTIPLIER = 1.00;  // all other times
const double MIN_DISTANCE_KM  = 0.5;    // shortest trip we accept
const double MAX_DISTANCE_KM  = 100.0;  // longest trip we accept
const double PROMO_RATE       = 0.10;   // 10% off the ride fare
const double PROMO_CAP        = 5.00;   // discount never exceeds RM5
const string PROMO_CODE       = "GRAB10";

// Illustrative metered-taxi rates used only for the comparison line
const double TAXI_FLAG_FARE   = 3.00;   // starting fare (RM)
const double TAXI_PER_KM      = 1.60;   // per kilometre (RM)

// Holds the pricing rules of one Grab service
struct ServiceInfo {
    string name;
    double baseFare;   // RM charged at the start of the trip
    double perKm;      // RM charged for each kilometre
    double minFare;    // lowest ride fare allowed (RM)
};

// Rounds an amount to the nearest sen (2 decimal places) so the printed
// lines always add up exactly
double roundMoney(double amount) {
    return floor(amount * 100.0 + 0.5) / 100.0;
}

// ------------------------- Helper: text handling ---------------------------

// Removes spaces/tabs at both ends of a string
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

// Converts a string to upper case (so "grab10" and "GRAB10" both work)
string toUpper(string text) {
    for (size_t i = 0; i < text.size(); i++) {
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// Reads one full line; exits cleanly if the input stream is closed
string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        cout << "\nInput closed. Goodbye!\n";
        exit(0);
    }
    return trim(line);
}

// ------------------------- Helper: input validation ------------------------

// Keeps asking until the user types a whole number between low and high.
// Rejects empty input, letters, symbols, decimals and out-of-range numbers.
int readInt(const string& prompt, int low, int high) {
    while (true) {
        string line = readLine(prompt);

        if (line.empty()) {
            cout << "  Invalid input: it cannot be empty.\n";
            continue;
        }

        bool allDigits = true;
        for (size_t i = 0; i < line.size(); i++) {
            if (!isdigit(static_cast<unsigned char>(line[i]))) {
                allDigits = false;
            }
        }
        if (!allDigits || line.size() > 9) {
            cout << "  Invalid input: please type a whole number such as 1.\n";
            continue;
        }

        int value = atoi(line.c_str());
        if (value < low || value > high) {
            cout << "  Invalid choice: enter a number from " << low
                 << " to " << high << ".\n";
            continue;
        }
        return value;
    }
}

// Keeps asking until the user types a positive decimal number in range.
// Accepts digits with at most one decimal point (e.g. 7 or 7.5).
double readDistance(const string& prompt, double low, double high) {
    while (true) {
        string line = readLine(prompt);

        if (line.empty()) {
            cout << "  Invalid input: it cannot be empty.\n";
            continue;
        }

        int dotCount = 0;
        int digitCount = 0;
        bool validChars = true;
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] == '.') {
                dotCount++;
            } else if (isdigit(static_cast<unsigned char>(line[i]))) {
                digitCount++;
            } else {
                validChars = false;   // letters, minus sign, commas, etc.
            }
        }
        if (!validChars || dotCount > 1 || digitCount == 0 || line.size() > 10) {
            cout << "  Invalid input: type a number such as 8 or 8.5 (no letters or symbols).\n";
            continue;
        }

        double value = atof(line.c_str());
        if (value < low || value > high) {
            cout << fixed << setprecision(1)
                 << "  Invalid distance: must be between " << low
                 << " km and " << high << " km.\n";
            continue;
        }
        return value;
    }
}

// Asks a yes/no question; returns true for yes, false for no
bool readYesNo(const string& prompt) {
    while (true) {
        string answer = toUpper(readLine(prompt));
        if (answer == "Y" || answer == "YES") {
            return true;
        } else if (answer == "N" || answer == "NO") {
            return false;
        }
        cout << "  Invalid input: please type y or n.\n";
    }
}

// ----------------------------- Core logic ----------------------------------

// Returns the pricing rules for the chosen menu number (switch statement)
ServiceInfo getService(int choice) {
    ServiceInfo service;
    switch (choice) {
        case 1:
            service = {"GrabBike (motorcycle)", 2.00, 0.90, 4.00};
            break;
        case 2:
            service = {"GrabCar (standard car)", 4.00, 1.30, 6.00};
            break;
        case 3:
            service = {"GrabCar Plus (premium car)", 6.00, 1.80, 9.00};
            break;
        default:
            service = {"Unknown", 0.0, 0.0, 0.0};   // never reached: input is validated
            break;
    }
    return service;
}

// Converts the time-period menu number into a price multiplier (if/else)
double getMultiplier(int period, string& label) {
    if (period == 1) {
        label = "Normal hours";
        return NORMAL_MULTIPLIER;
    } else if (period == 2) {
        label = "Peak hours (high demand)";
        return PEAK_MULTIPLIER;
    } else {
        label = "Late night (12am-5am)";
        return NIGHT_MULTIPLIER;
    }
}

// Ride fare = (base + per-km charge), raised to the minimum fare if needed,
// then multiplied by the time-period multiplier
double calculateRideFare(const ServiceInfo& service, double km, double multiplier) {
    double fare = service.baseFare + service.perKm * km;
    if (fare < service.minFare) {
        fare = service.minFare;
    }
    return roundMoney(fare * multiplier);
}

// ------------------------------ Display ------------------------------------

void printWelcome() {
    cout << "\n==============================================\n";
    cout << "        GRAB FARE ESTIMATOR (Student Demo)\n";
    cout << "==============================================\n";
    cout << "How it works:\n";
    cout << "  1. Pick a service from the menu.\n";
    cout << "  2. Enter your trip distance in km (0.5 - 100).\n";
    cout << "  3. Pick the time period of your trip.\n";
    cout << "  4. Optionally enter a promo code (try GRAB10).\n";
    cout << "You will see an upfront fare, just like the Grab app.\n";
    cout << "Rates are illustrative, not real Grab prices.\n";
}

void printServiceMenu() {
    cout << "\n--- Choose a service ---\n";
    cout << "  1. GrabBike\n";
    cout << "  2. GrabCar\n";
    cout << "  3. GrabCar Plus\n";
}

void printPeriodMenu() {
    cout << "\n--- When is your trip? ---\n";
    cout << "  1. Normal hours\n";
    cout << "  2. Peak hours (weekday rush hour)\n";
    cout << "  3. Late night (12am-5am)\n";
}

// Prints the itemised fare breakdown and the taxi comparison
void printReceipt(const ServiceInfo& service, double km, const string& periodLabel,
                  double multiplier, double rideFare, double discount, double total) {
    double taxiFare = roundMoney(TAXI_FLAG_FARE + TAXI_PER_KM * km);

    cout << fixed << setprecision(2);
    cout << "\n================ FARE ESTIMATE ================\n";
    cout << left << setw(22) << "Service:"  << service.name << "\n";
    cout << left << setw(22) << "Distance:" << setprecision(1) << km << " km\n";
    cout << setprecision(2);
    cout << left << setw(22) << "Time period:" << periodLabel
         << " (x" << multiplier << ")\n";
    cout << "-----------------------------------------------\n";
    cout << left << setw(30) << "Ride fare"       << "RM " << right << setw(8) << rideFare    << "\n";
    cout << left << setw(30) << "Booking fee"     << "RM " << right << setw(8) << BOOKING_FEE << "\n";
    if (discount > 0.0) {
        cout << left << setw(30) << "Promo discount" << "-RM " << right << setw(7) << discount << "\n";
    }
    cout << "-----------------------------------------------\n";
    cout << left << setw(30) << "TOTAL (upfront price)" << "RM " << right << setw(8) << total << "\n";
    cout << "===============================================\n";
    cout << left << "For comparison, an illustrative metered taxi: RM " << taxiFare
         << " (price only known at the end of the trip)\n";
}

// --------------------------------- Main ------------------------------------
int main() {
    printWelcome();

    bool runAgain = true;
    while (runAgain) {
        // Step 1: service type
        printServiceMenu();
        int serviceChoice = readInt("Enter your choice (1-3): ", 1, 3);
        ServiceInfo service = getService(serviceChoice);

        // Step 2: distance
        double km = readDistance("\nEnter trip distance in km: ", MIN_DISTANCE_KM, MAX_DISTANCE_KM);

        // Step 3: time period
        printPeriodMenu();
        int periodChoice = readInt("Enter your choice (1-3): ", 1, 3);
        string periodLabel;
        double multiplier = getMultiplier(periodChoice, periodLabel);

        // Step 4: calculate fare
        double rideFare = calculateRideFare(service, km, multiplier);

        // Step 5: optional promo code
        double discount = 0.0;
        string code = toUpper(readLine("\nPromo code (press Enter to skip): "));
        if (code.empty()) {
            cout << "  No promo code applied.\n";
        } else if (code == PROMO_CODE) {
            discount = roundMoney(rideFare * PROMO_RATE);
            if (discount > PROMO_CAP) {
                discount = PROMO_CAP;   // cap the discount
            }
            cout << "  Promo applied!\n";
        } else {
            cout << "  Promo code not recognised - continuing without discount.\n";
        }

        // Step 6: total and output
        double total = rideFare - discount + BOOKING_FEE;
        printReceipt(service, km, periodLabel, multiplier, rideFare, discount, total);

        // Step 7: run again?
        runAgain = readYesNo("\nEstimate another fare? (y/n): ");
    }

    cout << "\nThank you for using the Grab Fare Estimator. Goodbye!\n";
    return 0;
}
