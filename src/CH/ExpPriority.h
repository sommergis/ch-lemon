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

#ifndef CH_EXP_PRIORITY_H
#define CH_EXP_PRIORITY_H

#include "CHData.h"

#include <lemon/core.h>
#include <vector>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <lemon/adaptors.h>
#include <lemon/random.h>
#include "CHSearch.h"
#include "Utils/ContractDijkstra.h"
#include "Utils/EdgeDiffDijkstra.h"
#include "Priority.h"

using namespace std;
using namespace lemon;

///A class used to calculate the priority.
///
///Used to test experimental values.
class ExpPriority: public Priority {

  typedef ListDigraph Graph;
  typedef Graph::Node Node;
  typedef Graph::Arc Arc;
  typedef Graph::OutArcIt OutArcIt;
  typedef Graph::InArcIt InArcIt;
  typedef Graph::ArcMap<int> Cost;
  typedef Graph::NodeMap<int> NodeMap;
  typedef BinHeap<int,NodeMap > Heap;

private:

  Graph *g;
  Cost *c;
  // priority
  NodeMap state;
  Heap prior;
  // cost of queries, search space
  NodeMap sp;
  // deleted neighbours
  NodeMap dn;
  // edgedifference
  NodeMap ed;
  // exp prio
  NodeMap ne;
  // dijkstra
  EdgeDiffDijkstra dijkstra;

  void edgediff(Node& v) {
    vector<Arc> new_arcs;
    int in = 0;
    int out = 0;
    int n = 0;
    int max_out = 0;
    dijkstra.addContractedNode(v);
    for (OutArcIt e(*g, v); e != INVALID; ++e) {
      if (state[g->target(e)] == -2) continue;
      out += 1;
      if ((*c)[e] > max_out) max_out = (*c)[e];
    }
    for (InArcIt e(*g, v); e != INVALID; ++e) {
      Node w = g->source(e);
      if (state[w] == -2) continue;
      if(g->id(w) == g->id(v)) continue;
      in += 1;
      int limit = (*c)[e] + max_out;
      dijkstra.addSource(w);
      while(dijkstra.nextNode() != INVALID && dijkstra.currentDist(dijkstra.nextNode()) < limit) {
        dijkstra.processNextNode();
      }
      for (OutArcIt f(*g, v); f != INVALID; ++f) {
        if (state[g->target(f)] == -2) continue;
        if ((!dijkstra.processed(g->target(f))) || (dijkstra.currentDist(g->target(f)) > (*c)[e] + (*c)[f])) {
          n += 1;
          Arc sa = g->addArc(g->source(e),g->target(f));
          new_arcs.push_back(sa);
          (*c)[sa] = (*c)[e] + (*c)[f];
        }
      }
      dijkstra.clear();
    }
    // delete the new arcs
    int k = new_arcs.size();
    for (int i = 0; i < k; ++i) {
      g->erase(new_arcs[i]);
    }
    dijkstra.removeContractedNode(v);
    ed[v] = n - in - out;
  }

  void updateNeighbour(Node v, Node w) {
    // already contracted
    if (state[w] == -2) return;
    // if it is not a loop
    if (g->id(v) != g->id(w)) {
      edgediff(w);
      dn[w] += 1;
      sp[w] = max(sp[w], sp[v]+1);
      setPriority(w);
    }
  }

  void updateNeighbours(Node v) {
    dijkstra.addContractedNode(v);
    for (OutArcIt e(*g,v); e != INVALID; ++e) {
      updateNeighbour(v, g->target(e));
    }
    for (InArcIt e((*g),v); e != INVALID; ++e) {
      updateNeighbour(v, g->source(e));
    }
  }

  void nodeEdgeProportion(Node v) {
    int maxv = 0;
    int sum = 0;
    int i = 0;
    for (OutArcIt e (*g, v); e != INVALID; ++e) {
      ++i;
      int cost = (*c)[e];
      sum += cost;
      if (cost > maxv) {
        maxv = cost;
      }
    }
    for (InArcIt e (*g, v); e != INVALID; ++e) {
      ++i;
      int cost = (*c)[e];
      sum += cost;
      if (cost > maxv) {
        maxv = cost;
      }
    }
    ne[v] = maxv / (sum / i);
  }

  void setPriority(Node v) {
    prior.set(v, 190*ed[v] + 120*dn[v] + sp[v] + 20*ne[v] );
  }

public:

  ExpPriority(CHData& chdata):
  state(*chdata.graph, -1), prior(state), sp(*chdata.graph ,0), dn(*chdata.graph, 0), ed(*chdata.graph, 0), ne(*chdata.graph, 0),
  dijkstra(*chdata.graph, *chdata.cost) {
    g = chdata.graph;
    c = chdata.cost;
  }

  void init() {
    for (Graph::NodeIt v(*g); v != INVALID; ++v) {
      edgediff(v);
      nodeEdgeProportion(v);
      setPriority(v);
    }
  }

  Node nextNode() {
    Node v = INVALID;
    //cout << prior.size() << "\n";
    while (!prior.empty()) {
      v = prior.top();
      edgediff(v);
      setPriority(v);
      if (v == prior.top()) {
        prior.pop();
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
