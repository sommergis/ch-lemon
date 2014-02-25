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

#ifndef SearchDijkstra_H
#define SearchDijkstra_H

#include <lemon/static_graph.h>
#include "CHDijkstra.h"

using lemon::StaticDigraph;

///The dijkstra algorithm used for searching in the preprocessed CH graphs.
class SearchDijkstra: public CHDijkstra<StaticDigraph> {

  typedef CHDijkstra<StaticDigraph> super;
  typedef StaticDigraph Graph;
  typedef Graph::NodeMap<Arc> PredMap;

private:

  PredMap *_pred;

public:

  SearchDijkstra(Graph& graph, ArcMap& cost):
  super(graph, cost) {
    _pred = new PredMap(*_graph, INVALID);
  }

  ~SearchDijkstra() {
    delete _pred;
  }

  ///Process the next node without relaxing the arcs.
  Node processWithoutRelax() {
    Node v = _heap->top();
    int d = _heap->prio();
    _heap->pop();
    (*_distance)[v] = d;
    return v;
  }

  ///Reset every calculated value.
  void clear() {
    super::clear();
  }

  ///Resets the calculated values of a node.
  ///\param v The node
  void clearNode(Node v) {
    super::clearNode(v);
    (*_pred)[v] = INVALID;
  }

  ///Precess the next node.
  Node processNextNode() {
    Node v = _heap->top();
    int d = _heap->prio();
    _heap->pop();
    (*_distance)[v] = d;
    for (OutArcIt e(*_graph,v); e!=INVALID; ++e) {
      Node w = _graph->target(e);
      switch(_heap->state(w)) {
      case -1:
        _heap->push(w, d + (*_cost)[e]);
        (*_pred)[w] = e;
        _reset_stack->push(w);
        break;
      case 0:
        if (d + (*_cost)[e] < (*_heap)[w]) {
          _heap->decrease(w, d + (*_cost)[e]);
          (*_pred)[w] = e;
        }
        break;
      case -2:
        break;
      }
    }
    return v;
  }

  ///\param v The node
  ///\return The arc of the shortest path tree whose target is v.
  Arc predArc(Node v) const {
    return (*_pred)[v];
  }
};

#endif
