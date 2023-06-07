#ifndef EX2_Port_H
#define EX2_Port_H
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <sstream>

class Port
{
    public:
        // constructor
        Port(std::string&);
        
        std::string getName() const;
        std::vector<std::pair<std::shared_ptr<Port>, int>> portCargoRoute;
        std::vector<std::pair<std::shared_ptr<Port>, int>> portTimeRoute;
        std::vector<std::pair<std::shared_ptr<Port>, int>> inboundPorts;
        std::list<std::pair<std::string,int>> balance;

        void decreaseDepartureQuantity(std::string&, int);
        int getBalanceByTime(std::string&);


    private:
        
        std::string name;
        bool compareEvents(const std::string& time, const std::string& otherTime) const;
            
};

#endif //EX2_Port_H