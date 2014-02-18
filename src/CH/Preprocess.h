#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <vector>
#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <lemon/adaptors.h>
#include "Utils/ContractDijkstra.h"
#include "CHData.h"

/*
using std::max;
using std::cout;
using std::vector;
using std::pair;
using std::make_pair;
using lemon::ListDigraph;
using lemon::rnd;
using lemon::StaticDigraph;
using lemon::SubDigraph;
*/

using namespace std;
using namespace lemon;

/**
 * The class used to preprocess the graph.
 */
template <class Prior>
class Preprocess {

	typedef ListDigraph Graph;
	typedef Graph::Node Node;
	typedef Graph::Arc Arc;
	typedef Graph::OutArcIt OutArcIt;
	typedef Graph::InArcIt InArcIt;
	typedef Graph::ArcMap<int> Cost;
	typedef Graph::NodeMap<int> NodeMap;

	typedef SubDigraph<Graph, Graph::NodeMap<bool>, Graph::ArcMap<bool> > MySubDigraph;

private:

	CHData *data;
	Graph *g;
	Cost *c;
	vector<int> *ordervector;

	Graph::NodeMap<bool> finalized;

	Prior priority;
	NodeMap searchorder;

	ListDigraph::ArcMap<pair<ListDigraph::Arc,ListDigraph::Arc> > *pack;

	ContractDijkstra dijkstra;

	/**
	 * Contracts the given node.
	 * @param v The node to be contracted
	 */
	void contract(Node& v) {
		// max cost of the out arcs
		int max_out = 0;
		// "delete" node from further searches
		dijkstra.addContractedNode(v);
		for (OutArcIt e(*g, v); e != INVALID; ++e) {
			if (finalized[g->target(e)]) continue;
			if ((*c)[e] > max_out) max_out = (*c)[e];
		}
		// determining new arcs
		for (InArcIt e(*g, v); e != INVALID; ++e) {
			Node w = g->source(e);
			if (finalized[w]) continue;
			if(g->id(w) == g->id(v)) continue;
			int limit = (*c)[e] + max_out;
			dijkstra.addSource(w);
			while(dijkstra.nextNode() != INVALID && dijkstra.currentDist(dijkstra.nextNode()) <= limit) {
				dijkstra.processNextNode();
			}
			for (OutArcIt f(*g, v); f != INVALID; ++f) {
				if (finalized[g->target(f)]) continue;
				if(g->id(g->target(f)) == g->id(v)) continue;
				int id = g->id(g->target(f));
				if ((!dijkstra.processed(g->target(f))) || (dijkstra.currentDist(g->target(f)) > (*c)[e] + (*c)[f])) {
					Arc t = INVALID;
					for (OutArcIt r(*g, w); r != INVALID; ++r) {
						if (g->id(g->target(r)) == id) {
							t = r;
							break;
						}
					}
					if (t == INVALID) {
						Arc sa = g->addArc(g->source(e),g->target(f));
						(*c)[sa] = (*c)[e] + (*c)[f];
						(*pack)[sa] = make_pair(e,f);
					}
					else if ((*c)[t] > (*c)[e] + (*c)[f]) {
						(*c)[t] = (*c)[e] + (*c)[f];
						(*pack)[t] = make_pair(e,f);
					}
				}
			}
			dijkstra.clear();
		}
	}

	/**
	 * Creates the search graphs and adds their pointers to the CHData struct given in the contructor.
	 */
	void createSearchGraphs() {
		Graph::NodeMap<bool> nf1(*g,true);
		Graph::NodeMap<bool> nf2(*g,true);
		Graph::ArcMap<bool> af1(*g,false);
		Graph::ArcMap<bool> af2(*g,false);
		// forward edges
		MySubDigraph sg1(*g,nf1,af1);
		// backward edges
		MySubDigraph sg2(*g,nf2,af2);

		vector<Arc> to_reverse;
		vector<Arc> not_to_reverse;

		for (Graph::ArcIt e(*g); e != INVALID; ++e) {
			Node u = g->source(e);
			Node v = g->target(e);
			if (searchorder[u] < searchorder[v]) not_to_reverse.push_back(e);
			else if (searchorder[u] > searchorder[v]) to_reverse.push_back(e);
		}

		for (unsigned int i = 0; i < to_reverse.size(); ++i) {
			g->reverseArc(to_reverse[i]);
			af2[to_reverse[i]] = true;
		}

		for (unsigned int i = 0; i < not_to_reverse.size(); ++i) {
			af1[not_to_reverse[i]] = true;
		}

		data->forward_graph = new StaticDigraph();
		data->forward_cost = new StaticDigraph::ArcMap<int>(*(data->forward_graph));
		data->forward_nodeRef = new Graph::NodeMap<StaticDigraph::Node>(*g);
		data->forward_backArcRef = new StaticDigraph::ArcMap<Arc>(*(data->forward_graph));

		data->backward_graph = new StaticDigraph();
		data->backward_cost = new StaticDigraph::ArcMap<int>(*(data->backward_graph));
		data->backward_nodeRef = new Graph::NodeMap<StaticDigraph::Node>(*g);
		data->backward_backArcRef = new StaticDigraph::ArcMap<Arc>(*(data->backward_graph));

		DigraphCopy<MySubDigraph, StaticDigraph> cg1(sg1, *(data->forward_graph));
		cg1.nodeRef(*(data->forward_nodeRef));
		cg1.arcMap(*c, *(data->forward_cost));
		cg1.arcCrossRef(*(data->forward_backArcRef));
		cg1.run();

		DigraphCopy<MySubDigraph, StaticDigraph> cg2(sg2, *(data->backward_graph));
		cg2.nodeRef(*(data->backward_nodeRef));
		cg2.arcMap(*c, *(data->backward_cost));
		cg2.arcCrossRef(*(data->backward_backArcRef));
		cg2.run();
	}

public:

	/**
	 * Initializes the preprocessing algorithm.
	 * @param chdata The CHData struct containing the graph and the arc costs
	 */
	Preprocess(CHData& chdata):
	data(&chdata), finalized(*data->graph, false), priority(*data), searchorder(*data->graph), dijkstra(*data->graph, *data->cost) {

		g = data->graph;
		c = data->cost;
		ordervector = NULL;

		pack = new Graph::ArcMap<pair<Arc,Arc> >(*g, make_pair(INVALID, INVALID));
		data->pack = pack;
	}

	/**
	 * Run the preprocessing algorithm.
	 */
	void run() {
		ordervector = new vector<int>(data->nodes);
		int order = 0;
		priority.init();
		Node v = priority.nextNode();
		while (v != INVALID) {
			searchorder[v] = order;
			(*ordervector)[order] = g->id(v);
			contract(v);
			priority.finalize(v);
			finalized[v] = true;
			order++;

			v = priority.nextNode();
		}
		createSearchGraphs();
		if (data->local_order) data->order = ordervector;
		else delete ordervector;
	}
};

#endif
