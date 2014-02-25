#ifndef CHINTERFACE_H_
#define CHINTERFACE_H_

#include <lemon/list_graph.h>
#include "CH/CHData.h"
#include "CH/Preprocess.h"
#include "CH/CHSearch.h"
#include "CH/PathReconstruct.h"

using std::ifstream;
using lemon::ListDigraph;

/**
 * This class creates an interface for using the CH search algorithm on a graph.
 */
template <class Prior>
class CHInterface {

	typedef ListDigraph Graph;
	typedef Graph::Node Node;
	typedef Graph::ArcMap<int> Cost;
	typedef Graph::NodeMap<StaticDigraph::Node> refMap;

private:

	CHData chdata;

	CHSearch *chsearch;
	PathReconstruct *pathrec;

	refMap *forward_noderef;
	refMap *backward_noderef;

public:

	/**
	 * Creates the interface object.
	 * @param graph The ListDigraph in which we want to find shortest paths
	 * @param cost The ArcMap containing the arc costs
	 */
	CHInterface(ListDigraph& graph, Cost& cost):
		chdata(graph, cost) {
		chdata.nodes = countNodes(graph);

		chsearch = NULL;
		pathrec = NULL;

		forward_noderef = NULL;
		backward_noderef = NULL;
	}

	/**
	 * Destroys the interface object.
	 * The original graph and the arc costs won't be deleted.
	 */
	~CHInterface() {
		delete pathrec;
		delete chsearch;
	}

	/**
	 * Runs the preprocessing algorithm.
	 * The original graph will be changed.
	 */
	void createCH() {
		Preprocess<Prior> preproc(chdata);
		preproc.run();
		chsearch = new CHSearch(chdata);
		pathrec = new PathReconstruct(chdata, *chsearch);
		forward_noderef = chdata.forward_nodeRef;
		backward_noderef = chdata.backward_nodeRef;
	}

	void addOrder(vector<int> order) {
		chdata.setOrder(order);
	}

	/**
	 * The order in which the nodes were contracted.
	 */
	vector<int> getOrder() {
		return chdata.getOrder();
	}

	/**
	 * Runs the search from Node s to Node t.
	 * @param s The source node
	 * @param t The target node
	 */
	void runSearch(Node s, Node t) const {
		chsearch->run((*forward_noderef)[s], (*backward_noderef)[t]);
	}

	/**
	 * Runs the search from a new source node using the previous search results.
	 * @param s The new source node
	 */
	void runSearch_newSource(Node s) const {
		chsearch->run_newSource((*forward_noderef)[s]);
	}

	/**
	 * @return The distance between the previous search's source and target
	 */
	int dist() const {
		return chsearch->dist();
	}

	/**
	 * @return The path between the previous search's source and target
	 */
	vector<ListDigraph::Arc> getPath() const {
		return pathrec->getPath();
	}

	/**
	 * Clears the prevoius results.
	 */
	void clear() const {
		chsearch->clear();
	}
};

#endif
