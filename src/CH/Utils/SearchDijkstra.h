#ifndef SearchDijkstra_H
#define SearchDijkstra_H

#include <lemon/static_graph.h>
#include "CHDijkstra.h"

using lemon::StaticDigraph;

/**
 * The dijkstra algorithm used for searching in the preprocessed CH graphs.
 */
class SearchDijkstra: public CHDijkstra<StaticDigraph> {

	typedef CHDijkstra<StaticDigraph> super;
	typedef StaticDigraph Graph;
	typedef Graph::NodeMap<Arc> PredMap;

private:

	PredMap *pred;
	int test;

public:

	SearchDijkstra(Graph& old_graph, ArcMap& old_cost):
	super(old_graph, old_cost) {
		pred = new PredMap(*graph, INVALID);
		test = 0;
	}

	~SearchDijkstra() {
		delete pred;
	}

	/**
	 * Process the next node without relaxing the arcs.
	 */
	Node processWithoutRelax() {
		Node v = heap->top();
		int d = heap->prio();
		heap->pop();
		(*distance)[v] = d;
		return v;
	}

	/**
	 * Reset every calculated value.
	 */
	void clear() {
		test = 0;
		super::clear();
	}

	/**
	 * Resets the calculated values of a node.
	 * \param v The node
	 */
	void clearNode(Node v) {
		super::clearNode(v);
		(*pred)[v] = INVALID;
	}

	/**
	 * Precess the next node.
	 */
	Node processNextNode() {
		++test;
		Node v = heap->top();
		int d = heap->prio();
		heap->pop();
		(*distance)[v] = d;
		for (OutArcIt e(*graph,v); e!=INVALID; ++e) {
			Node w = graph->target(e);
			switch(heap->state(w)) {
			case -1:
				heap->push(w, d + (*cost)[e]);
				(*pred)[w] = e;
				reset->push(w);
				break;
			case 0:
				if (d + (*cost)[e] < (*heap)[w]) {
					heap->decrease(w, d + (*cost)[e]);
					(*pred)[w] = e;
				}
				break;
			case -2:
				break;
			}
		}
		return v;
	}

	/**
	 * \param v The node
	 * \return The arc of the sortest path tree whose target is v.
	 */
	Arc predArc(Node v) const {
		return (*pred)[v];
	}

	/**
	 * \return The number of finalized nodes.
	 *
	 */
	int getFinalizedNum() {
		return test;
	}
};

#endif
