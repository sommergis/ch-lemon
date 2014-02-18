#ifndef PATHRECONSTRUCT_H
#define PATHRECONSTRUCT_H

#include <vector>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

using std::vector;
using std::pair;
using lemon::ListDigraph;
using lemon::StaticDigraph;
using lemon::INVALID;

/**
 * A class used to reconstruct the paths from a CHSearch.
 */
class PathReconstruct {

private:

	CHSearch *chsearch;
	ListDigraph::ArcMap<pair<ListDigraph::Arc, ListDigraph::Arc> > *pack;

	StaticDigraph::ArcMap<ListDigraph::Arc> *forward_backArcRef;
	StaticDigraph::ArcMap<ListDigraph::Arc> *backward_backArcRef;

	/**
	 * Recursively unpack the new arcs in the path
	 * @param path The partially completed path of original arcs
	 * @param e The current arc
	 */
	void unpack(vector<ListDigraph::Arc>& path, ListDigraph::Arc e) const {
		if ((*pack)[e].first == INVALID) {
			path.push_back(e);
		} else {
			unpack(path, (*pack)[e].first);
			unpack(path, (*pack)[e].second);
		}
	}

public:

	/**
	 * Initializes the references.
	 * @param chdata The CHData containing every necessary pointer
	 * @param chsearch The CHSearch class used to find the shortest path
	 */
	PathReconstruct(CHData& chdata, CHSearch& old_chsearch):
		chsearch(&old_chsearch) {
		pack = chdata.pack;
		forward_backArcRef = chdata.forward_backArcRef;
		backward_backArcRef = chdata.backward_backArcRef;
	}

	/**
	 * @return The sortest path containing the original arcs
	 */
	vector<ListDigraph::Arc> getPath() const {
		vector<ListDigraph::Arc> path;
		vector<StaticDigraph::Arc> forward_path = chsearch->buildForwardPath();
		vector<StaticDigraph::Arc> backward_path = chsearch->buildBackwardPath();
		for (unsigned int i = 0; i < forward_path.size(); ++i) {
			unpack(path, (*forward_backArcRef)[forward_path[i]]);
		}
		for (unsigned int i = 0; i < backward_path.size(); ++i) {
			unpack(path, (*backward_backArcRef)[backward_path[i]]);
		}
		return path;
	}

};

#endif
