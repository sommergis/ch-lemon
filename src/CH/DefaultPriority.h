/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2013
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef CH_DEFAULT_PRIORITY_H
#define CH_DEFAULT_PRIORITY_H

#include <lemon/core.h>
#include <vector>
#include <lemon/list_graph.h>
#include "Utils/EdgeDiffDijkstra.h"
#include "CHData.h"
#include "Priority.h"

using namespace std;
using namespace lemon;

///A class used to calculate the priority of the nodes using values described in the article.
class DefaultPriority: public Priority {

  typedef ListDigraph Graph;
  typedef Graph::Node Node;
  typedef Graph::Arc Arc;
  typedef Graph::OutArcIt OutArcIt;
  typedef Graph::InArcIt InArcIt;
  typedef Graph::ArcMap<int> Cost;
  typedef Graph::NodeMap<int> NodeMap;
  typedef BinHeap<int,NodeMap > Heap;

private:

  Graph *_graph;
  Cost *_cost;
  ///priority
  NodeMap _state;
  Heap _prior;
  ///cost of queries, search space
  NodeMap _sp;
  ///deleted neighbours
  NodeMap _dn;
  ///edgedifference
  NodeMap _ed;
  ///dijkstra
  EdgeDiffDijkstra dijkstra;

  ///Calculates the edgedifference of the given node.
  ///\param v The node whose edge difference will be calculated.
  void edgediff(Node& v) {
    vector<Arc> new_arcs;
    int in = 0;
    int out = 0;
    int n = 0;
    int max_out = 0;
    dijkstra.addContractedNode(v);
    for (OutArcIt e(*_graph, v); e != INVALID; ++e) {
      if (_state[_graph->target(e)] == -2) continue;
      out += 1;
      if ((*_cost)[e] > max_out) max_out = (*_cost)[e];
    }
    for (InArcIt e(*_graph, v); e != INVALID; ++e) {
      Node w = _graph->source(e);
      if (_state[w] == -2) continue;
      if(_graph->id(w) == _graph->id(v)) continue;
      in += 1;
      int limit = (*_cost)[e] + max_out;
      dijkstra.addSource(w);
      while(dijkstra.nextNode() != INVALID && dijkstra.currentDist(dijkstra.nextNode()) < limit) {
        dijkstra.processNextNode();
      }
      for (OutArcIt f(*_graph, v); f != INVALID; ++f) {
        if (_state[_graph->target(f)] == -2) continue;
        if ((!dijkstra.processed(_graph->target(f))) || (dijkstra.currentDist(_graph->target(f)) > (*_cost)[e] + (*_cost)[f])) {
          n += 1;
          Arc sa = _graph->addArc(_graph->source(e),_graph->target(f));
          new_arcs.push_back(sa);
          (*_cost)[sa] = (*_cost)[e] + (*_cost)[f];
        }
      }
      dijkstra.clear();
    }
    // delete the new arcs
    int k = new_arcs.size();
    for (int i = 0; i < k; ++i) {
      _graph->erase(new_arcs[i]);
    }
    dijkstra.removeContractedNode(v);
    _ed[v] = n - in - out;
  }

  ///Updates the priority of the given neighbour of the given node.
  ///\param v The node
  ///\param w The neighbour
  void updateNeighbour(Node v, Node w) {
    // already contracted
    if (_state[w] == -2) return;
    // if it is not a loop
    if (_graph->id(v) != _graph->id(w)) {
      edgediff(w);
      _dn[w] += 1;
      _sp[w] = max(_sp[w], _sp[v]+1);
      _prior.set(w,190*_ed[w] + 120*_dn[w] + _sp[w]);
    }
  }

  ///Updates the priority of the neighbours of the given node.
  ///\param v The node
  void updateNeighbours(Node v) {
    dijkstra.addContractedNode(v);
    for (OutArcIt e(*_graph,v); e != INVALID; ++e) {
      updateNeighbour(v, _graph->target(e));
    }
    for (InArcIt e((*_graph),v); e != INVALID; ++e) {
      updateNeighbour(v, _graph->source(e));
    }
  }

  ///Calculates the initial edge differences
  void initialEdgeDifferences() {
    for (Graph::NodeIt n(*_graph); n != INVALID; ++n) {
      edgediff(n);
      _prior.set(n,190*_ed[n]);
    }
  }

public:

  DefaultPriority(CHData& chdata):
  _state(*chdata.graph, -1), _prior(_state), _sp(*chdata.graph ,0), _dn(*chdata.graph, 0), _ed(*chdata.graph, 0),
  dijkstra(*chdata.graph, *chdata.cost) {
    _graph = chdata.graph;
    _cost = chdata.cost;
  }

  void init() {
    initialEdgeDifferences();
  }

  Node nextNode() {
    Node v = INVALID;
    while (!_prior.empty()) {
      v = _prior.top();
      edgediff(v);
      _prior.set(v, 190*_ed[v] + 120*_dn[v] + _sp[v]);
      if (v == _prior.top()) {
        _prior.pop();
        return v;
      }
    }
    return v;
  }

  void finalize(Node v) {
    updateNeighbours(v);
  }
};


#endif
