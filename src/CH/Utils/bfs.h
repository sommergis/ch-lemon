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

#ifndef BFS_H_INCLUDED
#define BFS_H_INCLUDED

#include <queue>
#include <lemon/static_graph.h>

using namespace std;
using namespace lemon;

class Bfs {

  typedef StaticDigraph Graph;
  typedef Graph::Node Node;
  typedef Graph::Arc Arc;
  typedef Graph::ArcMap<int> ArcMap;
  typedef Graph::NodeMap<bool> CheckedMap;
  typedef Graph::NodeMap<int> DistMap;
  typedef Graph::OutArcIt OutArcIt;
  typedef Graph::NodeMap<Arc> PredMap;

  private:

  Graph *_graph;
  CheckedMap _checked;
  DistMap _dist;
  PredMap _pred;
  queue<Node> _queue;
  queue<Node> _reset_stack;

  public:

  Bfs(Graph& old_g):
  _graph(&old_g), _checked(*_graph, false), _dist(*_graph), _pred(*_graph, INVALID) {}

  void addSource(Node s) {
    _queue.push(s);
    _checked[s] = true;
    _reset_stack.push(s);
  }

  bool hasNext() const {
    return _queue.empty();
  }

  Node nextNode() const {
    return _queue.front();
  }

  bool processed(Node v) const {
    return _checked[v];
  }

  Node processNextNode() {
    Node v = _queue.front();
    _queue.pop();
    for (OutArcIt e(*_graph,v); e!=INVALID; ++e) {
      Node w = _graph->target(e);
      if (!_checked[w]) {
        _queue.push(w);
        _dist[w] = _dist[v] + 1;

        _pred[w] = e;
        _checked[w] = true;

        _reset_stack.push(w);
      }
    }
    return v;
  }

  Arc predArc(Node v) const {
    return _pred[v];
  }

  void clear() {
    _queue = queue<Node>();
    while (!_reset_stack.empty()) {
      Node v = _reset_stack.front();
      _reset_stack.pop();
      _dist[v] = 0;
      _pred[v] = INVALID;
      _checked[v] = false;
    }
  }
};

#endif // BFS_H_INCLUDED
