#ifndef CH_CHDATA_H
#define CH_CHDATA_H

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <vector>

using lemon::ListDigraph;
using lemon::StaticDigraph;
using std::pair;
using std::vector;

/**
 * A struct containing every object that is required for CH searches.
 */
struct CHData {

	ListDigraph *graph;
	ListDigraph::ArcMap<int> *cost;
	ListDigraph::ArcMap<pair<ListDigraph::Arc,ListDigraph::Arc> > *pack;
	int nodes;

	vector<int> *order;
	bool local_order;

	StaticDigraph *forward_graph;
	StaticDigraph::ArcMap<int> *forward_cost;
	ListDigraph::NodeMap<StaticDigraph::Node> *forward_nodeRef;
	StaticDigraph::ArcMap<ListDigraph::Arc> *forward_backArcRef;

	StaticDigraph *backward_graph;
	StaticDigraph::ArcMap<int> *backward_cost;
	ListDigraph::NodeMap<StaticDigraph::Node> *backward_nodeRef;
	StaticDigraph::ArcMap<ListDigraph::Arc> *backward_backArcRef;

	CHData(ListDigraph& g, ListDigraph::ArcMap<int>& c):
		graph(&g), cost(&c) {
		local_order = true;
	}

	~CHData() {
		delete backward_backArcRef;
		delete forward_backArcRef;
		delete backward_nodeRef;
		delete forward_nodeRef;
		delete backward_cost;
		delete forward_cost;
		delete backward_graph;
		delete forward_graph;
		delete pack;
		if (local_order) delete order;
	}

	void setOrder(vector<int> ordervector) {
		local_order = false;
		order = &ordervector;
	}

	vector<int> getOrder() {
		local_order = false;
		return *order;
	}
};

#endif
