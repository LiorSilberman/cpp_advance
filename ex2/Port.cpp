#include "Port.h"

// Constructor
Port::Port(std::string& portName)
: name(portName)
{}


// Returns the name of the port
std::string Port::getName() const
{
    return this->name;
}


// Returns the balance quantity at a specific time
int Port::getBalanceByTime(std::string& time)
{
    int quantityAtTime = 0;
    for (auto& t : balance)
    {
        if (compareEvents(t.first, time))
        {
            quantityAtTime += t.second;
        }
    }
    return  quantityAtTime;
}


// Decreases the departure quantity at a specific time
void Port::decreaseDepartureQuantity(std::string& time, int quantity)
{
    balance.push_back({time, -quantity});
    balance.sort();
}


// Compares two time values.
bool Port::compareEvents(const std::string& time, const std::string& otherTime) const
{
    // Convert strings to tm structures
    std::tm tm1 = {};
    std::tm tm2 = {};
    std::istringstream ss1(time);
    std::istringstream ss2(otherTime);

    ss1 >> std::get_time(&tm1, "%d/%m %H:%M");
    ss2 >> std::get_time(&tm2, "%d/%m %H:%M");

    // Convert tm structures to time_t values
    std::time_t t1 = std::mktime(&tm1);
    std::time_t t2 = std::mktime(&tm2);

    // Compare time_t values
    return t1 <= t2;
}