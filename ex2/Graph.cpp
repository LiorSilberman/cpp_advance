#include "Graph.h"


// This function adds a vertex to the list of vertices in the graph if it doesn't already exist.
void Graph::addVertex(port& vert)
{
    
    if (!isVerExist(vert))
        adjacencyList.push_back(vert);
}


/* 
   add cargo edge from sorce port to dest port and add quantity weight
   This function adds a cargo edge from the source port to the destination port in the graph, along with the specified weight.
   It also updates the destination port's balance by adding the weight at the specified arrival time.
*/ 
void Graph::addCargoEdge(port& source, port& dest, int weight, std::string& destArrivalTime)
{
    if (!isCargoEdgeExitst(source, dest))
    {
        source->portCargoRoute.push_back({dest, weight});
        dest->balance.push_back({destArrivalTime, weight});
        dest->balance.sort();
        return;
    }

    dest->balance.push_back({destArrivalTime, weight});
    dest->balance.sort();
    for (auto& p : source->portCargoRoute)
    {
        if (p.first == dest)
            p.second += weight;
    }
}


/* 
   This function adds a time edge from the source port to the destination port in the graph, along with the specified weight.
   It also adds the source port to the list of inbound ports for the destination port.
*/
void Graph::addTimeEdge(port& source, port& dest, int weight)
{
    if (!isTimeEdgeExitst(source, dest))
    {
        source->portTimeRoute.push_back({dest, weight});
        dest->inboundPorts.push_back({source,weight});
        return;
    }

    // if exist, calculate average weight
    for (auto& p : source->portTimeRoute)
    {
        if (p.first == dest)
            p.second = (p.second + weight) / 2;
    }
}

// This function checks if a vertex already exists in the list of vertices in the graph.
bool Graph::isVerExist(port& vert)
{
    for (auto& it : adjacencyList)
    {
        if (it == vert)
            return true;
    }
    return false;
}

// This function checks if a cargo edge already exists between the source port and the destination port in the graph.
bool Graph::isCargoEdgeExitst(port& source, port& dest)
{
    for (auto& it : source->portCargoRoute)
    {
        if (it.first == dest)
            return true;
    }
    return false;
}


// This function checks if a time edge already exists between the source port and the destination port in the graph.
bool Graph::isTimeEdgeExitst(port& source, port& dest)
{
    for (auto& it : source->portTimeRoute)
    {
        if (it.first == dest)
            return true;  
    }
    return false;
}

// This function returns a vector containing all the vertices in the graph.
std::vector<port> Graph::getGraph()
{
    return this->adjacencyList;
}