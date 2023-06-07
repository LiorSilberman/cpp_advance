#ifndef EX2_SHIPPINGNETWORK_H
#define EX2_SHIPPINGNETWORK_H

#include "Graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <ctime>
#include <sstream>
#include <stdexcept> 


class ShippingNetwork
{
    private:
        Graph shippingNetwork;
        std::string outputFile;

        void runProgram(int argc,char** argv);
        void runCommandLine();
        void getFile(std::string &fileName);
        bool checkSize(const std::string&);
        bool checkIsAlpha(const std::string&);
        bool checkTime(const std::string&);
        bool checkWholeNumber(const std::string&);
        int calculateTime(std::string&, std::string&);

        std::string load(std::string&);
        bool outbound(std::string&);
        bool inbound(std::string&);
        bool balance(std::string&, std::string&);
        void print();
        bool fileExists(const std::string&);


        class invalidCommandLine{};
        class fileNotFound{};
        class invalidFileName{};
        class invalidInput{};
        class invalidCommand{};

    public:
        ShippingNetwork(int argc, char** argv);
        void setOutputFile(std::string &fileName);
};

#endif //EX2_SHIPPINGNETWORK_H