#ifndef EX2_GRAPH_H
#define EX2_GRAPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include "Port.h"

typedef std::shared_ptr<Port> port;


class Graph
{
    private:
        std::vector<port> adjacencyList;

    
    public:
        // constructor
        Graph(){};

        void addVertex(port& port);
        void addCargoEdge(port& source, port& destination, int weight, std::string& time);
        void addTimeEdge(port& source, port& destination, int weight);
        
        bool isVerExist(port& port);
        bool isCargoEdgeExitst(port&, port&);
        bool isTimeEdgeExitst(port&, port&);
        std::vector<port> getGraph();
};

#endif //EX2_GRAPH_H