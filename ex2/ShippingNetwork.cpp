#include "ShippingNetwork.h"

// Constructor
ShippingNetwork::ShippingNetwork(int argc, char** argv) 
: outputFile("output.dat")
{
    try{
        runProgram(argc, argv);
    }
    catch(fileNotFound exc){
        std::cerr << "ERROR opening one of the file.\n";
        exit(1);
    }
    catch(invalidFileName exc){
        std::cerr << "ERROR file name is invalid.\n";
        exit(1);
    }
    catch(invalidInput exc){
        std::cerr << "ERROR file contents are invalid.\n";
        exit(1);
    }
    catch (invalidCommandLine exc){
        std::cerr << "ERROR compilation line is invalid.\n";
        exit(1);
    }
    catch (std::string s)
    {
        std::cerr << s << std::endl;
        exit(-1);
    }

    runCommandLine();
}


// Function to run the program based on command-line arguments
void ShippingNetwork::runProgram(int argc, char** argv)
{
    if (argc < 3){
        throw invalidCommandLine();                                                 // Throw an exception if the number of arguments is insufficient
    }

    int i = 1;
    std::string args(argv[i]);
    if (args.compare("-i") != 0)
        throw invalidCommandLine();                                                 // Throw an exception if the first argument is not "-i"

    args = argv[++i];
    if (args.compare("-c") == 0)                                                    // Throw an exception if the second argument is "-c"
        throw invalidCommandLine();
    while (args.compare("-c") != 0 && args.compare("-o") != 0 && i < argc) {
        getFile(args);                                                              // Process the file indicated by the argument
        
        if (i+1 < argc)
        {
            args = argv[++i];
        }
        else break;

        // i++;
    }
    if (i+1 < argc){
        args = argv[i];
        if (args.compare("-o") == 0){
            i++;
            if (i < argc){
                args = argv[i];
                setOutputFile(args);                                                    // Set the output file based on the argument
            }
            else
                throw invalidCommandLine();                                         
        }
        else
            throw invalidCommandLine();
    }
}


// This function is responsible for running the command line interface
void ShippingNetwork::runCommandLine()
{
    while (true)
    {
        std::string line;
        try {
            getline(std::cin, line);

            if (line == "exit")
                break;

            // load file command
            std::string subString = line.substr(0, 4);
            if (subString == "load")
            {
                // split the line by spaces
                std::stringstream ss(line);
                std::vector<std::string> vec_res;
                std::string tok;
                while (getline(ss, tok, ' '))
                    vec_res.push_back(tok);
                
                
                if (vec_res.size() != 2)
                    throw std::runtime_error("ERROR opening/reading the specified file.");
                
                std::string s = vec_res[1];
                if (!fileExists(s)){
                    throw std::runtime_error("ERROR opening/reading the specified file.");
                }

                std::string r = load(vec_res[1]);
                
                if (r != "Update was successful.")
                {
                    throw r;
                }
                std::cout << r << std::endl;
                ss.clear();
                continue;
            }

            // split the line by comma
            std::stringstream ss(line);
            std::vector<std::string> vec_res;
            std::string tok;
            while (getline(ss, tok, ','))
                vec_res.push_back(tok);

            // call outbound function
            if (vec_res.size() == 2 && vec_res[1] == "outbound")
            {
                if (!outbound(vec_res[0]))
                {
                    std::string s = vec_res[0];
                    throw s + " does not exist in the database.";
                } 
                continue;
            }

            // call inbound function
            if (vec_res.size() == 2 && vec_res[1] == "inbound")
            {
                if (!inbound(vec_res[0]))
                {
                    std::string s = vec_res[0];
                    throw s + " does not exist in the database.";
                }
                continue;
            }

            // call balance function
            if (vec_res.size() == 3 && vec_res[1] == "balance")
            {
                if (!checkTime(vec_res[2]))
                    throw std::runtime_error("Invalid date hour");

                if (!balance(vec_res[0], vec_res[2]))
                {
                    std::string s = vec_res[0];
                    throw s + " does not exist in the database.";
                }
                continue;
            }

            // call print function
            if (line == "print")
            {
                print();
                continue;
            }

            // No valid command found
            else
            {
                std::string s = "USAGE: 'load' <file> *or*\n";
                s.append("\t<node>,'inbound' *or*\n");
                s.append("\t<node>,'outbound' *or*\n");
                s.append("\t<node>,'balance',dd/mm HH:mm *or*\n");
                s.append("\t'print' *or*\n");
                s.append("\t'exit' *to terminate*");
                throw s;
            }
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch (std::string s){
            std::cout << s << std::endl;
        }
    }
}


// Loads data from a file and updates the shipping network
std::string ShippingNetwork::load(std::string& fileName)
{
    try{
        getFile(fileName);
        return "Update was successful.";
    }
    catch (std::string s)
    {
        std::string r = "Invalid input at line ";
        r += s.back();
        return r;
    }
}


// print all the outbound ports from specific given port. 
bool ShippingNetwork::outbound(std::string& portName)
{   
    for (auto& p : shippingNetwork.getGraph())
    {
        if (p->getName() == portName)
        {
            // Check if there are no outbound ports from the given port
            if (p->portTimeRoute.size() == 0)
            {
                std::cout << p->getName() << ": no outbound ports" << std::endl;
                return true;
            }
            
            // Print the outbound ports and their corresponding time
            for (auto& destPort : p->portTimeRoute)
            {
                std::cout << destPort.first->getName() << "," << destPort.second << std::endl;
            }
            return true;
        }
    }
    // The given port does not exist in the database
    return false;
}


// Prints the balance for a specific port at a given time
bool ShippingNetwork::balance(std::string& portName, std::string& time)
{
    for (auto& p : shippingNetwork.getGraph())
    {
        if (p->getName() == portName)
        {
            // Print the balance for the given port at the specified time
            std::cout << p->getBalanceByTime(time) << std::endl;
            return true;
        }
    }
    return false;
}

// Prints all the inbound ports to a specific given port
bool ShippingNetwork::inbound(std::string& portName)
{   
    for (auto& p : shippingNetwork.getGraph())
    {
        if (p->getName() == portName)
        {
            // Check if there are no inbound ports to the given port
            if (p->inboundPorts.size() == 0)
            {
                std::cout << p->getName() << ": no inbound ports" << std::endl;
                return true;
            }

            // Print the inbound ports and their corresponding time
            for (auto& inPort : p->inboundPorts)
            {
                std::cout << inPort.first->getName() << "," << inPort.second << std::endl;
            }
            return true;
        }
    }

    // The given port does not exist in the database
    return false;
}


// Prints the cargo graph and time graph to the output file
void ShippingNetwork::print()
{
    std::ofstream file(outputFile);
    if (!file){
        throw "Invalid input in file "+ outputFile + " at line 0";
    }
    
    // Print the cargo graph header
    file << "Cargo Graph:\n";
    file << std::left << std::setw(20) << "Source port";
    file << std::left << std::setw(20) << "Destination port";
    file << std::left << std::setw(20) << "Quantity" << std::endl;
    file << "-------------------------------------------------" << std::endl;

    // Print the cargo graph data
    for (auto& p : shippingNetwork.getGraph())
    {
        
        if (p->portCargoRoute.size() != 0)
        {
            
            for (auto& destPort : p->portCargoRoute)
            {
                file << std::left << std::setw(20) << " " + p->getName();
                file << std::left << std::setw(20) << " " + destPort.first->getName();
                file << std::left << std::setw(20) << "   " + std::to_string(destPort.second) << std::endl;
            }
            file << "-------------------------------------------------" << std::endl;
        }
    }

    // Print the time graph header
    file << "\n\nTime Graph:\n";
    file << std::left << std::setw(20) << "Source port";
    file << std::left << std::setw(20) << "Destination port";
    file << std::left << std::setw(20) << "Time in minutes" << std::endl;
    file << "-------------------------------------------------------" << std::endl;

    // Print the time graph data
    for (auto& p : shippingNetwork.getGraph())
    {
        
        if (p->portTimeRoute.size() != 0)
        {
            
            for (auto& destPort : p->portTimeRoute)
            {
                file << std::left << std::setw(20) << " " + p->getName();
                file << std::left << std::setw(20) << " " + destPort.first->getName();
                file << std::left << std::setw(20) << "   " + std::to_string(destPort.second) << std::endl;
                file << "-------------------------------------------------------" << std::endl;
            }
        }
    }
    file.close();
}


// Reads data from the input file and updates the shipping network based on the data
void ShippingNetwork::getFile(std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file){
        throw "Invalid input in file "+ fileName + " at line 0";
    }
    
    int count = 0;
    std::string line;
    std::vector<std::string> ports;
    std::string source = "";
    std::string sourceDepartureTime;
    std::shared_ptr<Port> sourcePort;
    std::shared_ptr<Port> firstPort;
    std::string departureTime = "";
    int cargoQuantity = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> splitValues;

        while (std::getline(ss, token, ',')) {
            splitValues.push_back(token);   
        }
        ports.push_back(splitValues[0]);

        // source port
        if (count == 0)
        {
            if (splitValues.size() != 2)
                throw invalidInput();  

            source = splitValues[0];
            sourcePort = std::make_shared<Port>(source);
            
            for (auto& p : shippingNetwork.getGraph())
            {
                if (p->getName() == source)
                {
                    sourcePort = p;
                }
            }
            firstPort = sourcePort;
            shippingNetwork.addVertex(sourcePort);
            sourceDepartureTime = splitValues[1];
            departureTime = sourceDepartureTime;
            

            // validations
            if (!checkSize(source) || !checkIsAlpha(source) || !checkTime(sourceDepartureTime))
                throw "Invalid input in file " + fileName + " at line " + std::to_string(count);
                ; 
            
            count++;
            continue;
        }

        // validations
        if (!checkSize(splitValues[0]) || !checkIsAlpha(splitValues[0]) || !checkTime(splitValues[1]) || !checkTime(splitValues[3]) || !checkWholeNumber(splitValues[2]))
            throw "Invalid input in file " + fileName + " at line " + std::to_string(count);
            
        
        // make Port
        std::shared_ptr<Port> destPort = std::make_shared<Port>(splitValues[0]);
        for (auto& p : shippingNetwork.getGraph())
        {
            if (p->getName() == splitValues[0])
            {
                destPort = p;
            }
        }
        // add port vertex
        shippingNetwork.addVertex(destPort);

        // add cargo edge
        int quantity = std::stoi(splitValues[2]);
        shippingNetwork.addCargoEdge(sourcePort, destPort, quantity, splitValues[1]);
        cargoQuantity += quantity;
        

        // calculate the time difference
        int time = calculateTime(departureTime, splitValues[1]);


        // update time graph
        shippingNetwork.addTimeEdge(firstPort, destPort, time);

        firstPort = destPort;
        departureTime = splitValues[3];
        count++;
    }

    // decrease source port quantity at departure time
    sourcePort->decreaseDepartureQuantity(sourceDepartureTime, cargoQuantity);

    file.close();
}


// Sets the output file for printing the cargo and time graphs
void ShippingNetwork::setOutputFile(std::string &fileName) {
    this->outputFile = fileName;
}


// Check the size of the port name
bool ShippingNetwork::checkSize(const std::string& portName)
{
    return portName.size() <= 16;
}


// Checks if the port name consists of alphabetic characters and spaces only
bool ShippingNetwork::checkIsAlpha(const std::string& portName)
{
    for (char c : portName)
    {
        if (!std::isalpha(c) && c != ' ')
        {
            return false;
        }
    }
    return true;
}

// Checks the format of the time string
bool ShippingNetwork::checkTime(const std::string& time)
{
    // Check the length of the string
    if (time.length() != 12 && time.length() != 11) {
        return false;
    }

    // Check the format
    if (time[2] != '/' || time[5] != ' ' || time[8] != ':') {
        return false;
    }

    // Check the values of each component
    for (int i = 0; i < 11; i++) {
        if (i != 2 && i != 5 && i != 8) {
            if (!std::isdigit(time[i])) {
                return false;
            }
        }
    }

    // Extract the components
    std::string dayStr = time.substr(0, 2);
    std::string monthStr = time.substr(3, 2);
    std::string hourStr = time.substr(6, 2);
    std::string minuteStr = time.substr(9, 2);

    // Convert components to integers
    int day = std::stoi(dayStr);
    int month = std::stoi(monthStr);
    int hour = std::stoi(hourStr);
    int minute = std::stoi(minuteStr);

    // Check the ranges of the components
    if (day < 1 || day > 31 || month < 1 || month > 12 || hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return false;
    }

    return true;
}


// Checks if the quantity string represents a positive whole number
bool ShippingNetwork::checkWholeNumber(const std::string& quantity)
{
    // Check if the string is empty
    if (quantity.empty()) {
        return false;
    }

    // Check if all characters are digits
    for (char c : quantity) {
        if (!std::isdigit(c)) {
            return false;
        }
    }

    // Check if the number is positive
    int number = std::stoi(quantity);
    if (number <= 0) {
        return false;
    }
    return true;
}

// Calculates the time difference in minutes between two time strings
int ShippingNetwork::calculateTime(std::string& departureTime, std::string& arrivalTime)
{
    std::tm timeStruct1 = {};
    std::tm timeStruct2 = {};
    std::istringstream ss1(departureTime);
    std::istringstream ss2(arrivalTime);
    ss1 >> std::get_time(&timeStruct1, "%d/%m %H:%M");
    ss2 >> std::get_time(&timeStruct2, "%d/%m %H:%M");
    
    std::time_t time1Seconds = std::mktime(&timeStruct1);
    std::time_t time2Seconds = std::mktime(&timeStruct2);

    std::time_t timeDifferenceSeconds = std::difftime(time2Seconds, time1Seconds);
    return timeDifferenceSeconds / 60;
}


// Checks if a file exists
bool ShippingNetwork::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}