#ifndef ContractDijkstra_H
#define ContractDijkstra_H

#include <lemon/list_graph.h>
#include "CHDijkstra.h"

using lemon::ListDigraph;

class ContractDijkstra: public CHDijkstra<ListDigraph> {

private:

	typedef CHDijkstra<ListDigraph> super;
	typedef ListDigraph Graph;

public:

	ContractDijkstra(Graph& old_graph, ArcMap& old_cost):
	super(old_graph, old_cost) {}

};

#endif
