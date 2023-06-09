/*********************************************
 * file:	~\code4b\graph.h                  *
 * remark:implementation of undirected graphs *
 **********************************************/

#include <iostream>
#include <algorithm>
#include <format>
#include <cassert>     // assert
#include <limits>      // std::numeric_limits

#include "graph.h"
#include "dsets.h"

// Note: graph vertices are numbered from 1 -- i.e. there is no vertex zero

// -- CONSTRUCTORS

// Create a graph with n vertices and no vertices
Graph::Graph(int n) : table(size_t(n) + 1), size{n}, n_edges{0} {
    assert(n >= 1);
}

Graph::Graph(const std::vector<Edge> &V, int n) : Graph{n} {
    for (auto e : V) {
        insertEdge(e);
    }
}

// -- MEMBER FUNCTIONS

// insert undirected edge e
// update weight if edge e is present
void Graph::insertEdge(const Edge &e) {
    assert(e.head >= 1 && e.head <= size);
    assert(e.tail >= 1 && e.tail <= size);

    auto edge_insertion = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        if (auto it = std::find_if(begin(T[e1.head]), end(T[e1.head]),
                                   [e1](const Edge &ed) { return e1.links_same_nodes(ed); });
            it == end(T[e1.head])) {
            T[e1.head].push_back(e1);  // insert new edge e1
            ++n;                       // increment the counter of edges
        } else {
            it->weight = e1.weight;  // update the weight
        }
    };

    edge_insertion(e);
    edge_insertion(e.reverse());
}

// remove undirected edge e
void Graph::removeEdge(const Edge &e) {
    assert(e.head >= 1 && e.head <= size);
    assert(e.tail >= 1 && e.tail <= size);

    auto edgeRemoval = [&T = this->table, &n = this->n_edges](const Edge &e1) {
        auto it = std::find_if(begin(T[e1.head]), end(T[e1.head]),
                               [e1](const Edge &ed) { return e1.links_same_nodes(ed); });

        assert(it != end(T[e1.head]));
        T[e1.head].erase(it);  // remove edge e1
        --n;                   // decrement the counter of edges
    };

    edgeRemoval(e);
    edgeRemoval(e.reverse());
}

// Prim's minimum spanning tree algorithm
void Graph::mstPrim() const {
    std::vector<int> dist(size_t(size) + 1, std::numeric_limits<int>::max());
    std::vector<int> path(size_t(size) + 1, 0);
    std::vector<bool> done(size_t(size) + 1, false);

    for (int i = 1; i <= size; ++i) {
        dist[i] = std::numeric_limits<int>::max();
        path[i] = 0;
        done[i] = false;
    }
    dist[1] = 0;
    done[1] = true; // To skip the first one (0, 1, 0)
    int v = 1;
    int totalWeight = 0;

    while (true)
    {
        // find_smallest_undone_distance_vertex(); Used in digraph.cpp (4B)
        int minDist = std::numeric_limits<int>::max();
        for (Edge e : table[v])
        {
            if (!done[e.tail] && dist[e.tail] > e.weight)
            {
                dist[e.tail] = e.weight;
                path[e.tail] = v;
            }
        }

        for (int i = 1; i <= size; ++i) 
        {
            if (minDist > dist[i] && !done[i])
            {
                v = i;
                minDist = dist[i];
            }
        }

        if (minDist == std::numeric_limits<int>::max())
        {
            break;
        }

        // Make the whole edge visible in the program.

        Edge E(path[v], v, dist[v]);
        std::cout << E << " \n";

        totalWeight += dist[v];
        done[v] = true;
    }
    
    std::cout << "Total weight = " << totalWeight << " ";
}

// Kruskal's minimum spanning tree algorithm
void Graph::mstKruskal() const {

    std::vector<Edge> heap;
    DSets D(size); // From dsets.hpp which is included
    std::vector<Edge> printEdge; // The edges stored in the minimum spanning tree.

    int counter = 0; // Number of edges in the minimum spanning tree.
    int totalWeight = 0;

    // Build heap with all the edges
    for (int i = 1; i < size; ++i) {
        for (Edge e : table[i])
        {
            if (i < e.tail)
            {
                heap.push_back(e);
            }
        }
    }

    // Heapify heap (min heap)
    std::make_heap(heap.begin(), heap.end(), std::greater<Edge>{});

    while (counter < size - 1)
    {
        // Find an edge with lowest cost
        Edge lowestCost = heap.front(); // Front element of the heap
        std::pop_heap(heap.begin(), heap.end(), std::greater<Edge>{}); // Move the edge with lowest weight to the front.
        heap.pop_back(); // Removes the last element of the container O(1)

        int findHead = D.find(lowestCost.head);
        int findTail = D.find(lowestCost.tail);

        // Union
        if (findHead != findTail)
        {
            D.join(findHead, findTail);
            totalWeight += lowestCost.weight;
            counter++;
            printEdge.push_back(lowestCost);
        }
    }
    for (Edge e : printEdge)
    {
        std::cout << e << "\n";
    }

    std::cout << "Total weight: " << totalWeight;
}

// print graph
void Graph::printGraph() const {
    std::cout << std::format("{:-<66}\n", '-');
    std::cout << "Vertex  adjacency lists\n";
    std::cout << std::format("{:-<66}\n", '-');

    for (int v = 1; v <= size; v++) {
        std::cout << std::format("{:4} : ", v);
        for (auto const &e : table[v]) {
            std::cout << std::format("({:2}, {:2}) ", e.tail, e.weight);
        }
        std::cout << "\n";
    }
    std::cout << std::format("{:-<66}\n", '-');
}
