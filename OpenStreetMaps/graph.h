// graph.h <Starter Code>
// Nathan Trinh
//
// Basic graph class using adjacency graph representation.
// Contains helper function to construct, edit and retrieve 
// information for the graph
//
// Adam T Koehler, PhD
// University of Illinois Chicago
// CS 251, Fall 2023
//
// Project Original Variartion By:
// Joe Hummel, PhD
// University of Illinois at Chicago
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <map>
#include <set>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
  private:
    map<VertexT, map<VertexT, WeightT>> adjList;
    vector<VertexT> verticesList;

  public:

    /// @brief Empty constructor for the graph
    graph() {
      adjList = {}; 
    }
    
    /// @brief Delete all graph data, including its keys (vertices) and values (maps of vertex neighbors) 
    void clear() {
      // Iterate through each vertex in graph to clear its value (maps of vertex neighbors)
      for (auto& vertex : adjList) {
        vertex.second.clear();
      }

      adjList.clear();
    }
    
    /// @brief Assignment operator clear the current graph and makes a copy of the "other" graph
    /// @param other The other graph to copy from
    /// @return A reference to the current graph after assignment
    graph& operator=(const graph &other) {
      // Check for self-assignment, return current graph if they are the same
      if (this == &other) {
        return *this;
      }

      clear();

      // Iterate through each vertex and its values map of the other graph
      for (auto& vertexPair : other.adjList) {
        VertexT vertex = vertexPair.first;
        auto& edgeMap = vertexPair.second;

        adjList[vertex]; // add copied vertex into graph

        // Iterate through each edge and its weight of the other graph
        for (auto& edgePair : edgeMap) {
          VertexT neighbor = edgePair.first;
          WeightT weight = edgePair.second;

          adjList[vertex][neighbor] = weight; // add copied weight into vertex's map
        }
      }

      return *this;
    }

    /// @brief Returns number of vertices in the graph
    int NumVertices() const {
      return adjList.size();
    }

    /// @brief Returns number of edges in the graph
    int NumEdges() const {
      int sum = 0;

      // Iterate through each vertex in graph to add number of edges
      for (auto& vertex : adjList) {
        sum += vertex.second.size();
      }

      return sum;
    }

    /// @brief Adds a new vertex into the graph
    /// @param v vertex to be added
    /// @return true if vertex is successfully added, false if vertex already exist in graph
    bool addVertex(VertexT v) {
      // Check and return false if vertex is already in graph
      if (adjList.count(v) != 0) {
        return false;
      }

      // Create new edge map for the vertex to insert in graph
      map<VertexT, WeightT> newEdgeMap; 
      adjList[v] = newEdgeMap;

      verticesList.push_back(v); // push into vector of vertices

      return true;
    }

    /// @brief Adds a new edge between two vertices into the graph
    /// @param from Vertex that the edge starts from
    /// @param to Vertex that the edge points to
    /// @param weight Weight of the new edge
    /// @return True if edge is successfully added, false if either vertex does not exist in graph
    bool addEdge(VertexT from, VertexT to, WeightT weight) {
      // Check and return false if either vertex is not in graph
      if (adjList.count(from) == 0 || adjList.count(to) == 0) {
        return false;
      }

      // Add the new edge with weight into graph
      adjList[from][to] = weight; 

      return true;
    }

    /// @brief Get the weight of an edge between two vertices
    /// @param from Vertex that the edge starts from
    /// @param to Vertex that the edge points to
    /// @param weight Passed-by-reference variable to store the edge's weight
    /// @return True if weight is successfully retrieved
    /// False if either vertex does not exist in the graph or if there is no edge between the two vertices
    bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
      // Check and return false if either vertex is not in graph
      if (adjList.count(from) == 0 || adjList.count(to) == 0) {
        return false;
      }

      // Check and return false if edge between two vertices does not exist
      if (adjList.at(from).count(to) == 0) {
        return false;
      }

      // Retrieve and assign the weight of edge to passed-by-reference variable
      weight = adjList.at(from).at(to);

      return true;
    }

    /// @brief Get the set of neighbors for a given vertex in the graph
    /// @param v Vertex for which neighbors are to be retrieved
    /// @return Set of neighbors vertices of given vertex
    set<VertexT> neighbors(VertexT v) const {
      set<VertexT> neighborSet;

      // Check if the vertex exists in the graph
      if (adjList.count(v) != 0) {
        // Iterate over neighbors of the given vertex
        for (auto& neighborPair : adjList.at(v)) {
          // Insert the neighbor vertex into the set
          neighborSet.insert(neighborPair.first);
        }
      }

      return neighborSet;
    }

    /// @brief Get the vector of vertices in the graph
    /// @return Vector of vertices in the graph
    vector<VertexT> getVertices() const {
      return verticesList;
    }

    /// @brief Dump the graph information to the output stream
    /// @param output The output stream to which the graph information will be written
    void dump(ostream& output) const {
      output << "***************************************************" << endl;
      output << "********************* GRAPH ***********************" << endl;

      // Display the number of vertices and edges
      output << "**Num vertices: " << NumVertices() << endl;
      output << "**Num edges: " << NumEdges() << endl;

      // Display the list of vertices
      output << endl;
      output << "**Vertices:" << endl;
      for (auto& vertex : verticesList) {
        output << " " << vertex << endl;
      }

      // Display the list of edges
      output << endl;
      output << "**Edges:" << endl;
      for (auto& vertexPair : adjList) {
        VertexT row = vertexPair.first;
        output << " row " << row << ": ";

        // Iterate over edges for the current vertex
        for (auto& edgePair : vertexPair.second) {
          VertexT col = edgePair.first;
          WeightT weight = edgePair.second;

          // Display the edge information
          output << "(" << col << "," << weight << ") ";
        }
        output << endl;
      }
      output << "**************************************************" << endl;
    }
}; 