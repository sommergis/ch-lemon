#ifndef EdgeDiffDijkstra_H
#define EdgeDiffDijkstra_H

#include <lemon/list_graph.h>
#include "CHDijkstra.h"

using lemon::ListDigraph;

class EdgeDiffDijkstra: public CHDijkstra<ListDigraph> {

	typedef CHDijkstra<ListDigraph> super;
	typedef ListDigraph Graph;

private:

	int hoplimit;
	NodeMap *level;

public:

	EdgeDiffDijkstra(Graph& old_graph, ArcMap& old_cost):
	super(old_graph, old_cost) {
		hoplimit = 5;
		level = new NodeMap(*graph, 0);
	}

	~EdgeDiffDijkstra() {
		delete level;
	}

	void removeContractedNode(Node v) {
		(*state)[v] = -1;
	}

	void setLimit(int i) {
		hoplimit = i;
	}

	void clearNode(Node v) {
		super::clearNode(v);
		(*level)[v] = 0;
	}

	Node processNextNode() {
		Node v = heap->top();
		int d = heap->prio();
		heap->pop();
		(*distance)[v] = d;
		if ((*level)[v] >= hoplimit) return v;
		for (OutArcIt e(*graph,v); e!=INVALID; ++e) {
			Node w = graph->target(e);
			switch(heap->state(w)) {
			case -1:
				heap->push(w, d + (*cost)[e]);
				reset->push(w);
				(*level)[w] = (*level)[v] + 1;
				break;
			case 0:
				if (d + (*cost)[e] < (*heap)[w]) {
					heap->decrease(w, d + (*cost)[e]);
					(*level)[w] = (*level)[v] + 1;
				}
				break;
			case -2:
				break;
			}
		}
		return v;
	}
};

#endif
