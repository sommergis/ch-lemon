#ifndef CHSearch_H
#define CHSearch_H

#include <lemon/static_graph.h>
#include <vector>
#include "Utils/SearchDijkstra.h"

using std::cout;
using std::min;
using std::vector;
using lemon::StaticDigraph;

/**
 * The class responsible for searching in the preprocessed graphs.
 */
class CHSearch {

	typedef StaticDigraph Graph;
	typedef Graph::Node Node;
	typedef Graph::Arc Arc;
	typedef Graph::ArcMap<int> Cost;
	typedef Graph::OutArcIt OutArcIt;
	typedef SearchDijkstra Dijkstra;

private:

	Dijkstra *forward_dijkstra;
	Dijkstra *backward_dijkstra;
	Graph *forward_graph;
	Graph *backward_graph;
	Cost *forward_cost;
	Cost *backward_cost;
	int dist_s;
	int dist_t;
	int dmin;
	Node nodemin;
	bool source;
	bool target;


	/**
	 * Small initialization.
	 */
	void init() {
		dmin = -1;
		nodemin = INVALID;
	}

	/**
	 * Runs the search between the previously given source and target.
	 */
	void runSearch() {
		Node v;
		// while no node was found which was finalized in both directions
		while (dmin == -1) {
			if (forward_dijkstra->nextNode() != INVALID) {
				v = forward_dijkstra->processNextNode();
				dist_s = forward_dijkstra->currentDist(v);
				check(v);
			}
			else {
				runBackward();
				return;
			}
			if (backward_dijkstra->nextNode() != INVALID) {
				v = backward_dijkstra->processNextNode();
				dist_t = backward_dijkstra->currentDist(v);
				check(v);
			}
			else {
				runForward();
				return;
			}
		}
		// no more nodes to finalize and no common found
		if (dmin == -1) return;
		// found a common node than we can limit the search
		while (min(dist_s,dist_t) < dmin) {
			if (dist_s >= dmin || forward_dijkstra->nextNode() == INVALID) {
				runBackward();
				return;
			}
			else {
				v = forward_dijkstra->nextNode();
				forward_dijkstra->processNextNode();
				dist_s = forward_dijkstra->currentDist(v);
				check(v);
			}
			if (backward_dijkstra->nextNode() == INVALID || dist_t >= dmin) {
				runForward();
				return;
			}
			else {
				v = backward_dijkstra->nextNode();
				backward_dijkstra->processNextNode();
				dist_t = backward_dijkstra->currentDist(v);
				check(v);
			}
		}
	}

	/**
	 * Only runs the forward search.
	 */
	void runForward() {
		Node v;
		while (dmin == -1 && forward_dijkstra->nextNode() != INVALID) {
			v = forward_dijkstra->processNextNode();
			dist_s = forward_dijkstra->currentDist(v);
			check(v);
		}
		if (dmin == -1) return;
		while (dist_s < dmin && forward_dijkstra->nextNode() != INVALID) {
			v = forward_dijkstra->processNextNode();
			dist_s = forward_dijkstra->currentDist(v);
			check(v);
		}
	}

	/**
	 * Only runs the backward search.
	 */
	void runBackward() {
		Node v;
		while (dmin == -1 && backward_dijkstra->nextNode() != INVALID) {
			v = backward_dijkstra->processNextNode();
			dist_t = backward_dijkstra->currentDist(v);
			check(v);
		}
		if (dmin == -1) return;
		while (dist_t < dmin && backward_dijkstra->nextNode() != INVALID) {
			v = backward_dijkstra->processNextNode();
			dist_t = backward_dijkstra->currentDist(v);
			check(v);
		}
	}

	/**
	 * Check if a node is processed in both directions.
	 */
	void check(Node& v) {
		if (forward_dijkstra->processed(v) && backward_dijkstra->processed(v)) {
			if ((forward_dijkstra->currentDist(v) + backward_dijkstra->currentDist(v) < dmin) || (dmin == -1)) {
				dmin = forward_dijkstra->currentDist(v) + backward_dijkstra->currentDist(v);
				nodemin = v;
			}
		}
	}

	/**
	 * Add source.
	 * @param v The source
	 */
	void addSource(Node& v) {
		forward_dijkstra->clear();
		forward_dijkstra->addSource(v);
		dist_s = 0;
		source = true;
	}

	/**
	 * Add source.
	 * @param v The target
	 */
	void addTarget(Node& v) {
		backward_dijkstra->clear();
		backward_dijkstra->addSource(v);
		dist_t = 0;
		target = true;
	}

	/**
	 * Recursively build the forward path.
	 * @param p The current path
	 * @param v The current node
	 */
	void fpath(vector<Arc>& p, Node v) const {
		if (forward_dijkstra->predArc(v) == INVALID) return;
		else {
			fpath(p, forward_graph->source(forward_dijkstra->predArc(v)));
			p.push_back(forward_dijkstra->predArc(v));
		}
	}

	/**
	 * Recursively build the backward path.
	 * @param p The current path
	 * @param v The current node
	 */
	void bpath(vector<Arc>& p, Node v) const {
		if (backward_dijkstra->predArc(v) == INVALID) return;
		else {
			p.push_back(backward_dijkstra->predArc(v));
			bpath(p, backward_graph->source(backward_dijkstra->predArc(v)));
		}
	}

public:

	/**
	 * Initializes the search algorithm.
	 * @param chdata The CHData struct containing every necessary pointer.
	 */
	CHSearch(CHData& chdata) {
		forward_graph = chdata.forward_graph;
		forward_cost = chdata.forward_cost;
		backward_graph = chdata.backward_graph;
		backward_cost = chdata.backward_cost;

		forward_dijkstra = new Dijkstra(*forward_graph, *forward_cost);
		backward_dijkstra = new Dijkstra(*backward_graph, *backward_cost);

		dist_s = 0;
		dist_t = 0;
		dmin = -1;
		nodemin = INVALID;

		source = false;
		target = false;
	}

	~CHSearch() {
		delete forward_dijkstra;
		delete backward_dijkstra;
	}

	/**
	 * Resets all calculated data.
	 */
	void clear() {
		forward_dijkstra->clear();
		backward_dijkstra->clear();
		source = false;
		target = false;
	}

	/**
	 * Run the algorithm between the given source and target.
	 * @param s The source
	 * @param t The target
	 */
	void run(Node& s, Node& t) {
		addSource(s);
		addTarget(t);
		init();
		runSearch();
	}

	/**
	 * Run the algorithm between the new source and the previous target.
	 * @param s The new source
	 */
	void run_newSource(Node& s) {
		addSource(s);
		init();
		if (source && target) runSearch();
	}

	/**
	 * Run the algorithm between the given target and the previous source.
	 * @param t The new target
	 */
	void run_newTarget(Node& t) {
		addTarget(t);
		init();
		if (source && target) runSearch();
	}

	/**
	 * @return The previously calculated distance.
	 */
	int dist() const {
		return dmin;
	}

	/**
	 * @return The forward path
	 */
	vector<Arc> buildForwardPath() const {
		vector<Arc> path;
		if (nodemin != INVALID) {
			fpath(path, nodemin);
		}
		return path;
	}

	/**
	 * @return The backward path
	 */
	vector<Arc> buildBackwardPath() const {
		vector<Arc> path;
		if (nodemin != INVALID) {
			bpath(path, nodemin);
		}
		return path;
	}

	/**
	 * @return The number of finalized nodes
	 */
	unsigned int getFinalizedNum() const {
		return forward_dijkstra->getFinalizedNum() + backward_dijkstra->getFinalizedNum();
	}

};

#endif
