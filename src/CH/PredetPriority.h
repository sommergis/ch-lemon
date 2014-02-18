#ifndef CH_PREDET_PRIORITY_H
#define CH_PREDET_PRIORITY_H

#include <vector>
#include <lemon/list_graph.h>
#include "CHData.h"
#include "Priority.h"

using namespace std;
using namespace lemon;

/**
 * This class uses a given order which should be available in the CHData object given to the constructor.
 */
class PredetPriority: public Priority {

	typedef ListDigraph Graph;
	typedef Graph::Node Node;

private:

	int i;
	int nodes;
	Graph *g;
	vector<int> order;

public:

	PredetPriority(CHData& chdata) {
		g = chdata.graph;
		order = *chdata.order;
		i = 0;
		nodes = chdata.nodes;
	}

	Node nextNode() {
		if (i < nodes) {
			return g->nodeFromId(order[i]);
		} else {
			return INVALID;
		}
	}

	void finalize(Node v) {
		++i;
	}
};

#endif
