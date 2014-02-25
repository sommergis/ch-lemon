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

#ifndef EdgeDiffDijkstra_H
#define EdgeDiffDijkstra_H

#include <lemon/list_graph.h>
#include "CHDijkstra.h"

using lemon::ListDigraph;

class EdgeDiffDijkstra: public CHDijkstra<ListDigraph> {

  typedef CHDijkstra<ListDigraph> super;
  typedef ListDigraph Graph;

private:

  int _hoplimit;
  NodeMap *_level;

public:

  EdgeDiffDijkstra(Graph& graph, ArcMap& cost):
  super(graph, cost) {
    _hoplimit = 5;
    _level = new NodeMap(*_graph, 0);
  }

  ~EdgeDiffDijkstra() {
    delete _level;
  }

  void removeContractedNode(Node v) {
    (*_state)[v] = -1;
  }

  void setLimit(int i) {
    _hoplimit = i;
  }

  void clearNode(Node v) {
    super::clearNode(v);
    (*_level)[v] = 0;
  }

  Node processNextNode() {
    Node v = _heap->top();
    int d = _heap->prio();
    _heap->pop();
    (*_distance)[v] = d;
    if ((*_level)[v] >= _hoplimit) return v;
    for (OutArcIt e(*_graph,v); e!=INVALID; ++e) {
      Node w = _graph->target(e);
      switch(_heap->state(w)) {
      case -1:
        _heap->push(w, d + (*_cost)[e]);
        _reset_stack->push(w);
        (*_level)[w] = (*_level)[v] + 1;
        break;
      case 0:
        if (d + (*_cost)[e] < (*_heap)[w]) {
          _heap->decrease(w, d + (*_cost)[e]);
          (*_level)[w] = (*_level)[v] + 1;
        }
        break;
      case -2:
        break;
      }
    }
    return v;
  }
};

#endif
