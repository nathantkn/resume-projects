// application.cpp <Starter Code>
// Nathan Trinh
//
// Adam T Koehler, PhD
// University of Illinois Chicago
// CS 251, Fall 2023
//
// Project Original Variartion By:
// Joe Hummel, PhD
// University of Illinois at Chicago
//
// Program uses UIC’s East campus map to navigate two people
// at two different buildings to a center building by using 
// Dijkstra’s algorithm to find the shortest weighted path 
// for both people to the destination building
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "tinyxml2.h"
#include "dist.h"
#include "graph.h"
#include "osm.h"

using namespace std;
using namespace tinyxml2;

const double INF = numeric_limits<double>::max();

class prioritize {
public:
  bool operator()(const pair<long long , double>& p1, const pair<long long, double>& p2) const
  {
    return p1.second > p2.second; 
  }
};

// @brief Find the center building based on coordinates of the midpoint, skipping unreachable buildings
/// @param Buildings Vector of BuildingInfo containing building information
/// @param mid Coordinates representing the midpoint
/// @param unreachableBuildings Set of buildings that are marked as unreachable
/// @return BuildingInfo object representing the center building
BuildingInfo findCenterBuilding(vector<BuildingInfo>& Buildings, Coordinates mid, set<string> unreachableBuildings) {
  double minDist = INF;
  BuildingInfo buildingCenter;

  // Iterate over each building
  for (BuildingInfo& building : Buildings) {
    // Skip buildings marked as unreachable
    if (unreachableBuildings.count(building.Abbrev) > 0) {
      continue;
    }

    // Calculate the distance between the midpoint and the current building
    double currDist = distBetween2Points(mid.Lat, mid.Lon, building.Coords.Lat, building.Coords.Lon);

    // Update minimum distance and BuildingInfo of the center building if a closer distance is found
    if (currDist < minDist) {
      minDist = currDist;
      buildingCenter = building;
    }
  }

  // Return the BuildingInfo of the building that is closest to the midpoint
  return buildingCenter;
}

/// @brief Find the nearest node to a given building along footways
/// @param Nodes Map of node IDs to their coordinates
/// @param Footways Vector of FootwayInfo containing footway information
/// @param building BuildingInfo representing the target building
/// @return ID of the nearest node to the building along footways
long long nearestNode(map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways, BuildingInfo building) {
  double minDist = INF;
  long long foundNode;

  // Iterate over each footway
  for (auto &footway : Footways) {
    // Iterate over nodes in the current footway
    for (size_t i = 0; i < footway.Nodes.size(); i++) {
      // Get coordinates of the current node
      Coordinates currCoords = Nodes.at(footway.Nodes[i]);

      // Calculate the distance between the current node and the target building
      double currDist = distBetween2Points(currCoords.Lat, currCoords.Lon, building.Coords.Lat, building.Coords.Lon);

      // Update minimum distance and ID of the nearest node if a shorter distance is found
      if (currDist < minDist) {
        minDist = currDist;
        foundNode = currCoords.ID;
      }
    }
  }
  
  // Return the ID of the nearest node
  return foundNode;
}

/// @brief Search for a building in the data vector based on abbreviation or partial name
/// @param Buildings Vector of BuildingInfo containing building information
/// @param query Partial name or abbreviation of building to search for
/// @return BuildingInfo object of the found building
BuildingInfo searchBuilding(vector<BuildingInfo>& Buildings, string query) {
  BuildingInfo foundBuilding;

  // Search for exact match based on abbreviation
  for (BuildingInfo& building : Buildings) {
    // Return building information if match is found
    if (building.Abbrev == query) {
      return building; 
    }
  }

  // Search for partial match based on full name
  for (BuildingInfo& building : Buildings) {
    // Return building information if match is found
    if (building.Fullname.find(query) != string::npos) {
      return building;
    }
  }

  // Return empty BuildingInfo object if building is not found
  return foundBuilding;
}

/// @brief Perform Dijkstra's shortest path algorithm to find all possible paths from a starting vertex
/// @param G Graph of all vertices and edges information
/// @param distances Map to store shortest distances from start vertex to each vertex
/// @param predecessors Map to store predecessors for each vertex in the shortest path
/// @param startV Starting vertex to begin the algorithm
void DijkstraShortestPath(graph<long long, double>& G, map<long long, double>& distances, 
                          map<long long, long long>& predecessors, long long startV) {
  // Priority queue to store vertices with their distances, with custom comparison function
  priority_queue<pair<long long, double>,
  vector<pair<long long, double>>,
  prioritize> unvisitedQueue;

  // Set to track visited vertices
  set<long long> visitedSet;

  // Initialize distances and predecessors maps
  for (long long& currV : G.getVertices()) {
    distances[currV] = INF;
    predecessors[currV] = 0;
    unvisitedQueue.push(make_pair(currV, INF));
  }

  // Set distance of start vertex to 0 and add to the queue
  distances[startV] = 0;
  unvisitedQueue.push(make_pair(startV, 0));

  // Loop while the remaining vertices are reachable
  while (!unvisitedQueue.empty()) {
    // Visit vertex with minimum distance from startV
    long long currV = unvisitedQueue.top().first;
    unvisitedQueue.pop();
    
    // Break if the remaining vertices have infinite distance, implying they are unreachable
    if (distances[currV] == INF) {
      break;
    }
    // Skip already visited vertices
    else if (visitedSet.count(currV) > 0) { 
      continue;
    }
    // If the vertex is not visited, mark it as visited and calculate its path
    else {
      visitedSet.insert(currV);
    }

    // Explore neighbors of the current vertex
    for (auto& adjV : G.neighbors(currV)) {
      double edgeWeight = 0;
      G.getWeight(currV, adjV, edgeWeight); // retrieve the weight between current vertex and neighbor vertex
      double alternativePathDist = distances[currV] + edgeWeight; // calculate potential shorter path to compare with calculated distance in map

      // If a shorter path from startV to adjV is found, update adjV's distance and predecessor
      if (alternativePathDist < distances[adjV]) {
        distances[adjV] = alternativePathDist;
        predecessors[adjV] = currV;
        unvisitedQueue.push(make_pair(adjV, alternativePathDist));
      }
    }
  }
}

/// @brief Get path from predecessors map and end vertex
/// @param predecessors Map of predecessors for each node in the path
/// @param endVertex The end vertex of the generated path
/// @return Vector of path from start vertex to end vertex
vector <long long> getPath(map<long long, long long> predecessors, long long endVertex) {
  vector<long long> pathVector;
  stack<long long> pathStack;

  // Start from end vertex to trace back using predecessors
  long long currV = endVertex;

  while (predecessors[currV] != 0) {
    pathStack.push(currV); // push into stack to reverse the order
    currV = predecessors[currV]; // set to next predecessor
  }
  
  // Add start vertex to the path vector
  pathVector.push_back(currV);

  // Pop elements from stack to reverse the order and add to path vector
  while (!pathStack.empty()) {
    long long currPathV = pathStack.top();
    pathVector.push_back(currPathV);
    pathStack.pop();
  }

  return pathVector;
}

/// @brief Print paths and distances for two persons to a destination node
/// @param distances1 Map of distances from source node to each node for person 1
/// @param predecessors1 Map of predecessors for each node in shortest path for person 1
/// @param distances2 Map of distances from source node to each node for person 2
/// @param predecessors2 Map of predecessors for each node in shortest path for person 2
/// @param nodeCenter Destination node
void printPathsAndDist(map<long long, double>& distances1, map<long long, long long>& predecessors1, 
                        map<long long, double>& distances2, map<long long, long long>& predecessors2, 
                        long long nodeCenter) {
  // Get paths for both buildings
  vector<long long> pathToDist1 = getPath(predecessors1, nodeCenter);
  vector<long long> pathToDist2 = getPath(predecessors2, nodeCenter);

  // Output person 1's distance and path
  cout << endl;
  cout << "Person 1's distance to dest: " << distances1[nodeCenter] << " miles" << endl;
  cout << "Path: ";
  for (size_t i = 0; i < pathToDist1.size(); i++) {
    cout << pathToDist1[i];
    if (i != pathToDist1.size() - 1) {
      cout << "->";
    }
  }

  // Output person 2's distance and path
  cout << endl;
  cout << "Person 2's distance to dest: " << distances2[nodeCenter] << " miles" << endl;
  cout << "Path: ";
  for (size_t i = 0; i < pathToDist2.size(); i++) {
    cout << pathToDist2[i];
    if (i != pathToDist2.size() - 1) {
      cout << "->";
    }
  }
}
  

/// @brief Main application to find path to nearest center building between 2 selected buildings
/// @param Nodes Map of node IDs to their coordinates
/// @param Footways Vector of footway information
/// @param Buildings Vector of building information
/// @param G Graph of vertices and edges information
void application (map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
                  vector<BuildingInfo>& Buildings, graph<long long, double> G) {
  string person1Building, person2Building;

  // Prompt for person 1's building
  cout << endl;
  cout << "Enter person 1's building (partial name or abbreviation), or #> ";
  getline(cin, person1Building);

  // Loop until user enters "#" for person1Building
  while (person1Building != "#") {
    // Prompt for person 2's building
    cout << "Enter person 2's building (partial name or abbreviation)> ";
    getline(cin, person2Building);

    // Search for buildings based on inputs
    BuildingInfo building1 = searchBuilding(Buildings, person1Building);
    BuildingInfo building2 = searchBuilding(Buildings, person2Building);
    
    // Enter if buildings were not found, implying an invalid input
    while (building1.Abbrev == "" || building2.Abbrev == "") {
      // Display error message based on invalid cases
      if (building1.Abbrev == "") {
        cout << "Person 1's building not found" << endl;
      }
      else if (building2.Abbrev == "") {
        cout << "Person 2's building not found" << endl;
      }
      
      // Prompt again for for person 1's building
      cout << endl;
      cout << "Enter person 1's building (partial name or abbreviation), or #> ";
      getline(cin, person1Building);

      // Exit program if user enters "#" for person1Building
      if (person1Building == "#") {
        return;
      }

      // Prompt again for for person 2's building
      cout << "Enter person 2's building (partial name or abbreviation)> ";
      getline(cin, person2Building);

      // Search for buildings again based on inputs
      building1 = searchBuilding(Buildings, person1Building);
      building2 = searchBuilding(Buildings, person2Building);
    }

    set<string> unreachableBuildings;
    map<long long, double> distances1;
    map<long long, long long> predecessors1;
    map<long long, double> distances2;
    map<long long, long long> predecessors2;
    bool foundPath = false;

    // Display information about selected buildings
    cout << endl;
    cout << "Person 1's point:" << endl;
    cout << " " << building1.Fullname << endl;
    cout << " (" << building1.Coords.Lat << ", " << building1.Coords.Lon << ")" << endl;

    cout << "Person 2's point:" << endl;
    cout << " " << building2.Fullname << endl;
    cout << " (" << building2.Coords.Lat << ", " << building2.Coords.Lon << ")" << endl;

    // Calculate midpoint between two buildings
    Coordinates midpoint = centerBetween2Points(building1.Coords.Lat, building1.Coords.Lon, building2.Coords.Lat, building2.Coords.Lon);

    // Find nearest nodes from both buildings
    long long node1 = nearestNode(Nodes, Footways, building1);
    long long node2 = nearestNode(Nodes, Footways, building2);
    
    // Loops while a path is not found for both buildings
    while (!foundPath) {
      // Locate center building
      BuildingInfo buildingCenter = findCenterBuilding(Buildings, midpoint, unreachableBuildings);

      // Find nearest node from destination building
      long long nodeCenter = nearestNode(Nodes, Footways, buildingCenter);

      // Display destination building and nearest nodes information
      if (unreachableBuildings.empty()) {
        cout << "Destination Building:" << endl;
        cout << " " << buildingCenter.Fullname << endl;
        cout << " (" << buildingCenter.Coords.Lat << ", " << buildingCenter.Coords.Lon << ")" << endl;
        
        cout << endl;
        cout << "Nearest P1 node:" << endl;
        cout << " " << node1 << endl;
        cout << " (" << Nodes.at(node1).Lat << ", " << Nodes.at(node1).Lon << ")" << endl;

        cout << "Nearest P2 node:" << endl;
        cout << " " << node2 << endl;
        cout << " (" << Nodes.at(node2).Lat << ", " << Nodes.at(node2).Lon << ")" << endl;
      }
      // Display only if first center building was unreachable
      else {
        cout << endl;
        cout << "New destination building:" << endl;
        cout << " " << buildingCenter.Fullname << endl;
        cout << " (" << buildingCenter.Coords.Lat << ", " << buildingCenter.Coords.Lon << ")" << endl;
      }

      cout << "Nearest destination node:" << endl;
      cout << " " << nodeCenter << endl;
      cout << " (" << Nodes.at(nodeCenter).Lat << ", " << Nodes.at(nodeCenter).Lon << ")" << endl;

      // Run Dijkstra’s Algorithm for first building's node
      DijkstraShortestPath(G, distances1, predecessors1, node1);

      // Check if second building is unreachable, implying no possible path
      if (distances1[node2] >= INF) {
        cout << "Sorry, destination unreachable." << endl;
        break;
      }

      // Run Dijkstra’s Algorithm for second building's node
      DijkstraShortestPath(G, distances2, predecessors2, node2);

      // Check if center building is unreachable for either person
      if (distances1[nodeCenter] >= INF || distances2[nodeCenter] >= INF) {
        // Mark the building as unreachable
        unreachableBuildings.insert(buildingCenter.Abbrev);
        cout << "At least one person was unable to reach the destination building. Finding next closest building..." << endl;
      } 
      else {
        // Output distances and paths to destination
        foundPath = true;
        printPathsAndDist(distances1, predecessors1, distances2, predecessors2, nodeCenter);
      }
    }
    
    // Prompt for another building to search
    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
  }
}

/// @brief Populate graph with vertices and edges based on provided information
/// @param Nodes Map of node IDs to their coordinates
/// @param Footways Vector of footway information
/// @param Buildings Vector of building information
/// @param G Graph to be populated
void populateGraph (
    map<long long, Coordinates>& Nodes, vector<FootwayInfo>& Footways,
    vector<BuildingInfo>& Buildings, graph<long long, double>& G) {
  
  // Add vertices to graph for each node
  for (auto &vertex : Nodes) {
    G.addVertex(vertex.first);
  }

  // Add edges to graph based on footway information
  for (auto &footway : Footways) {
    // Iterate over nodes in the footway
    for (size_t i = 0; i < footway.Nodes.size() - 1; i++) {
      // Retrieve coordinates of two consecutive nodes
      Coordinates c1 = Nodes.at(footway.Nodes[i]);
      Coordinates c2 = Nodes.at(footway.Nodes[i + 1]);

      // Calculate distance between the nodes
      double distance = distBetween2Points(c1.Lat, c1.Lon, c2.Lat, c2.Lon);

      // Add edges in both directions
      G.addEdge(c1.ID, c2.ID, distance);
      G.addEdge(c2.ID, c1.ID, distance);
    }
  }
}

int main() {
  graph<long long, double> G;
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  // Load XML-based map file
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  // Read the nodes, which are the various known positions on the map
  int nodeCount = ReadMapNodes(xmldoc, Nodes);

  // Read the footways, which are the walking paths
  int footwayCount = ReadFootways(xmldoc, Footways);

  // Read the university buildings
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  // Stats
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

  populateGraph(Nodes, Footways, Buildings, G);

  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;

  // Execute Application
  application(Nodes, Footways, Buildings, G);

  cout << "** Done **" << endl;
  return 0;
}
