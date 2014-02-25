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

#ifndef CH_CHDATA_H
#define CH_CHDATA_H

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <vector>

using lemon::ListDigraph;
using lemon::StaticDigraph;
using std::pair;
using std::vector;

///A struct containing every object that is required for CH searches.
struct CHData {

  ///The original graph
  ListDigraph *graph;
  ///The original cost function on arcs
  ListDigraph::ArcMap<int> *cost;
  ///The pair of arcs that are bypassed by a new arc
  ListDigraph::ArcMap<pair<ListDigraph::Arc,ListDigraph::Arc> > *pack;
  ///The number of nodes
  int nodes;
  ///Vector containing the node ids in the order of contraction
  vector<int> *order;
  ///True if the order vector is not used outside the package
  bool local_order;
  ///The forward search graph
  StaticDigraph *forward_graph;
  ///The cost of the arcs in the forward search graph
  StaticDigraph::ArcMap<int> *forward_cost;
  ///Node reference from the original graph to the forward search graph
  ListDigraph::NodeMap<StaticDigraph::Node> *forward_nodeRef;
  ///Node reference from the forward search graph to the original graph
  StaticDigraph::ArcMap<ListDigraph::Arc> *forward_backArcRef;
  ///The backward search graph
  StaticDigraph *backward_graph;
  ///The cost of the arcs in the backward search graph
  StaticDigraph::ArcMap<int> *backward_cost;
  ///Node reference from the original graph to the backward search graph
  ListDigraph::NodeMap<StaticDigraph::Node> *backward_nodeRef;
  ///Node reference from the backward search graph to the original graph
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
    if (local_order) {
      delete order;
    }
  }

  void setOrder(vector<int>& ordervector) {
    local_order = false;
    order = &ordervector;
  }

  vector<int> getOrder() {
    local_order = false;
    return *order;
  }
};

#endif
