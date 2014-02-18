#ifndef CHDijkstra_H
#define CHDijkstra_H

#include <stack>
#include <lemon/bin_heap.h>

using std::stack;
using lemon::INVALID;
using lemon::BinHeap;

/**
 * Template Dijkstra algorithm with support for resetting values instead of using new Map instances for greater speed.
 */
template <typename Graph>
class CHDijkstra {

protected:

	typedef typename Graph::Node Node;
	typedef typename Graph::Arc Arc;
	typedef typename Graph::template ArcMap<int> ArcMap;
	typedef typename Graph::template NodeMap<int> NodeMap;
	typedef typename Graph::OutArcIt OutArcIt;
	typedef BinHeap<int,NodeMap> Heap;
	// nodes to reset
	typedef stack<Node> ResetStack;

	Graph *graph;
	ArcMap *cost;
	NodeMap *state;
	Heap *heap;
	NodeMap *distance;
	ResetStack *reset;

public:

	/**
	 * Initializes the algorithm.
	 * @param graph The graph
	 * @param cost The arcMap with the arc costs.
	 */
	CHDijkstra(Graph& old_graph, ArcMap& old_cost):
	graph(&old_graph), cost(&old_cost) {
		state = new NodeMap(*graph, -1);
		heap = new Heap(*state);
		distance = new NodeMap(*graph);
		reset = new ResetStack();
	}

	virtual ~CHDijkstra() {
		delete state;
		delete heap;
		delete distance;
		delete reset;
	}

	/**
	 * Add a new source.
	 * @param s The source node
	 * @param d The initial distance
	 */
	void addSource(Node s, int d=0) {
		if(heap->state(s) != 0) {
			heap->push(s, d);
			reset->push(s);
		}
		else if((*heap)[s] < d) {
			heap->push(s, d);
			reset->push(s);
		}
	}

	/**
	 * @return The next node to be processed
	 */
	Node nextNode() const {
		return heap->empty() ? INVALID : heap->top();
	}

	/**
	 * @param v The node
	 * @return Whether v is processed.
	 *
	 */
	bool processed(Node v) const {
		return (heap->state(v) == -2);
	}

	/**
	 * @param v The node
	 * @return Whether v is in the heap.
	 */
	bool reached(Node v) const {
		return (heap->state(v) == -1);
	}

	/**
	 * @param v The node
	 * @return The current distance of v from the source.
	 */
	int currentDist(Node v) const {
		return processed(v) ? (*distance)[v] : (*heap)[v];
	}

	/**
	 * Precess the next node.
	 */
	Node processNextNode() {
		Node v = heap->top();
		int d = heap->prio();
		heap->pop();
		(*distance)[v] = d;
		for (OutArcIt e(*graph,v); e!=INVALID; ++e) {
			Node w = graph->target(e);
			switch(heap->state(w)) {
			case -1:
				heap->push(w, d + (*cost)[e]);
				reset->push(w);
				break;
			case 0:
				if (d + (*cost)[e] < (*heap)[w]) {
					heap->decrease(w, d + (*cost)[e]);
				}
				break;
			case -2:
				break;
			}
		}
		return v;
	}

	/**
	 * Reset every calculated value.
	 */
	void clear() {
		heap->clear();
		while (!reset->empty()) {
			Node v = reset->top();
			clearNode(v);
			reset->pop();
		}
	}

	/**
	 * Resets the calculated values of a node.
	 * @param v The node
	 */
	void virtual clearNode(Node v) {
		(*state)[v] = -1;
	}

	/**
	 * Removes a node from further searches
	 * @param v The node
	 */
	void addContractedNode(Node v) {
		(*state)[v] = -2;
	}
};

#endif
