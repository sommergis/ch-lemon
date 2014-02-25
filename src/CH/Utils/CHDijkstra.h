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

#ifndef CHDijkstra_H
#define CHDijkstra_H

#include <stack>
#include <lemon/bin_heap.h>

using std::stack;
using lemon::INVALID;
using lemon::BinHeap;

///Template Dijkstra algorithm with support for resetting values instead of using new Map instances for greater speed.
template <typename Graph>
class CHDijkstra {

protected:

  typedef typename Graph::Node Node;
  typedef typename Graph::Arc Arc;
  typedef typename Graph::template ArcMap<int> ArcMap;
  typedef typename Graph::template NodeMap<int> NodeMap;
  typedef typename Graph::OutArcIt OutArcIt;
  typedef BinHeap<int,NodeMap> Heap;
  ///The stack containing nodes to reset
  typedef stack<Node> ResetStack;

  Graph *_graph;
  ArcMap *_cost;
  NodeMap *_state;
  Heap *_heap;
  NodeMap *_distance;
  ResetStack *_reset_stack;

public:

  ///Initializes the algorithm.
  ///\param graph The graph
  ///\param cost The arcMap with the arc costs.
  CHDijkstra(Graph& graph, ArcMap& cost):
  _graph(&graph), _cost(&cost) {
    _state = new NodeMap(*_graph, -1);
    _heap = new Heap(*_state);
    _distance = new NodeMap(*_graph);
    _reset_stack = new ResetStack();
  }

  virtual ~CHDijkstra() {
    delete _state;
    delete _heap;
    delete _distance;
    delete _reset_stack;
  }

  ///Add a new source.
  ///\param s The source node
  ///\param d The initial distance
  void addSource(Node s, int d=0) {
    if(_heap->state(s) != 0) {
      _heap->push(s, d);
      _reset_stack->push(s);
    }
    else if((*_heap)[s] < d) {
      _heap->push(s, d);
      _reset_stack->push(s);
    }
  }

  ///\return The next node to be processed
  Node nextNode() const {
    return _heap->empty() ? INVALID : _heap->top();
  }

  ///\param v The node
  ///\return Whether v is processed.
  bool processed(Node v) const {
    return (_heap->state(v) == -2);
  }

  ///\param v The node
  ///\return Whether v is in the heap.
  bool reached(Node v) const {
    return (_heap->_state(v) == -1);
  }

  ///\param v The node
  ///\return The current distance of v from the source.
  int currentDist(Node v) const {
    return processed(v) ? (*_distance)[v] : (*_heap)[v];
  }

  ///Process the next node.
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
        _reset_stack->push(w);
        break;
      case 0:
        if (d + (*_cost)[e] < (*_heap)[w]) {
          _heap->decrease(w, d + (*_cost)[e]);
        }
        break;
      case -2:
        break;
      }
    }
    return v;
  }

  ///Reset every calculated value.
  void clear() {
    _heap->clear();
    while (!_reset_stack->empty()) {
      Node v = _reset_stack->top();
      clearNode(v);
      _reset_stack->pop();
    }
  }

  ///Resets the calculated values of a node.
  ///\param v The node
  void virtual clearNode(Node v) {
    (*_state)[v] = -1;
  }

  ///Removes a node from further searches
  ///\param v The node
  void addContractedNode(Node v) {
    (*_state)[v] = -2;
  }
};

#endif
