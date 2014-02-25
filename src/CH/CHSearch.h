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

#ifndef CHSearch_H
#define CHSearch_H

#include <lemon/static_graph.h>
#include <vector>
#include "Utils/SearchDijkstra.h"

using std::cout;
using std::min;
using std::vector;
using lemon::StaticDigraph;

///The class responsible for searching in the preprocessed graphs.
class CHSearch {

  typedef StaticDigraph Graph;
  typedef Graph::Node Node;
  typedef Graph::Arc Arc;
  typedef Graph::ArcMap<int> Cost;
  typedef Graph::OutArcIt OutArcIt;
  typedef SearchDijkstra Dijkstra;

private:

  Dijkstra *_forward_dijkstra;
  Dijkstra *_backward_dijkstra;
  Graph *_forward_graph;
  Graph *_backward_graph;
  Cost *_forward_cost;
  Cost *_backward_cost;
  int _dist_s;
  int _dist_t;
  int _dmin;
  Node _nodemin;
  bool _source;
  bool _target;

  ///Small initialization.
  void init() {
    _dmin = -1;
    _nodemin = INVALID;
  }

  ///Runs the search between the previously given source and target.
  void runSearch() {
    Node v;
    // while no node was found which was finalized in both directions
    while (_dmin == -1) {
      if (_forward_dijkstra->nextNode() != INVALID) {
        v = _forward_dijkstra->processNextNode();
        _dist_s = _forward_dijkstra->currentDist(v);
        check(v);
      }
      else {
        runBackward();
        return;
      }
      if (_backward_dijkstra->nextNode() != INVALID) {
        v = _backward_dijkstra->processNextNode();
        _dist_t = _backward_dijkstra->currentDist(v);
        check(v);
      }
      else {
        runForward();
        return;
      }
    }
    // no more nodes to finalize and no common found
    if (_dmin == -1) return;
    // found a common node than we can limit the search
    while (min(_dist_s,_dist_t) < _dmin) {
      if (_dist_s >= _dmin || _forward_dijkstra->nextNode() == INVALID) {
        runBackward();
        return;
      }
      else {
        v = _forward_dijkstra->nextNode();
        _forward_dijkstra->processNextNode();
        _dist_s = _forward_dijkstra->currentDist(v);
        check(v);
      }
      if (_backward_dijkstra->nextNode() == INVALID || _dist_t >= _dmin) {
        runForward();
        return;
      }
      else {
        v = _backward_dijkstra->nextNode();
        _backward_dijkstra->processNextNode();
        _dist_t = _backward_dijkstra->currentDist(v);
        check(v);
      }
    }
  }

  ///Only runs the forward search.
  void runForward() {
    Node v;
    while (_dmin == -1 && _forward_dijkstra->nextNode() != INVALID) {
      v = _forward_dijkstra->processNextNode();
      _dist_s = _forward_dijkstra->currentDist(v);
      check(v);
    }
    if (_dmin == -1) return;
    while (_dist_s < _dmin && _forward_dijkstra->nextNode() != INVALID) {
      v = _forward_dijkstra->processNextNode();
      _dist_s = _forward_dijkstra->currentDist(v);
      check(v);
    }
  }

  ///Only runs the backward search.
  void runBackward() {
    Node v;
    while (_dmin == -1 && _backward_dijkstra->nextNode() != INVALID) {
      v = _backward_dijkstra->processNextNode();
      _dist_t = _backward_dijkstra->currentDist(v);
      check(v);
    }
    if (_dmin == -1) return;
    while (_dist_t < _dmin && _backward_dijkstra->nextNode() != INVALID) {
      v = _backward_dijkstra->processNextNode();
      _dist_t = _backward_dijkstra->currentDist(v);
      check(v);
    }
  }

  ///Check if a node is processed in both directions.
  void check(Node& v) {
    if (_forward_dijkstra->processed(v) && _backward_dijkstra->processed(v)) {
      if ((_forward_dijkstra->currentDist(v) + _backward_dijkstra->currentDist(v) < _dmin) || (_dmin == -1)) {
        _dmin = _forward_dijkstra->currentDist(v) + _backward_dijkstra->currentDist(v);
        _nodemin = v;
      }
    }
  }

  ///Add source.
  ///\param v The source
  void addSource(Node& v) {
    _forward_dijkstra->clear();
    _forward_dijkstra->addSource(v);
    _dist_s = 0;
    _source = true;
  }

  ///Add target.
  ///\param v The target
  void addTarget(Node& v) {
    _backward_dijkstra->clear();
    _backward_dijkstra->addSource(v);
    _dist_t = 0;
    _target = true;
  }

  ///Recursively build the forward path.
  ///\param p The current path
  ///\param v The current node
  void fpath(vector<Arc>& p, Node v) const {
    if (_forward_dijkstra->predArc(v) == INVALID) return;
    else {
      fpath(p, _forward_graph->source(_forward_dijkstra->predArc(v)));
      p.push_back(_forward_dijkstra->predArc(v));
    }
  }

  ///Recursively build the backward path.
  ///\param p The current path
  ///\param v The current node
  void bpath(vector<Arc>& p, Node v) const {
    if (_backward_dijkstra->predArc(v) == INVALID) return;
    else {
      p.push_back(_backward_dijkstra->predArc(v));
      bpath(p, _backward_graph->source(_backward_dijkstra->predArc(v)));
    }
  }

public:

  ///Initializes the search algorithm.
  ///\param chdata The CHData struct containing every necessary pointer.
  CHSearch(CHData& chdata) {
    _forward_graph = chdata.forward_graph;
    _forward_cost = chdata.forward_cost;
    _backward_graph = chdata.backward_graph;
    _backward_cost = chdata.backward_cost;

    _forward_dijkstra = new Dijkstra(*_forward_graph, *_forward_cost);
    _backward_dijkstra = new Dijkstra(*_backward_graph, *_backward_cost);

    _dist_s = 0;
    _dist_t = 0;
    _dmin = -1;
    _nodemin = INVALID;

    _source = false;
    _target = false;
  }

  ~CHSearch() {
    delete _forward_dijkstra;
    delete _backward_dijkstra;
  }

  ///Resets all calculated data.
  void clear() {
    _forward_dijkstra->clear();
    _backward_dijkstra->clear();
    _source = false;
    _target = false;
  }

  ///Run the algorithm between the given source and target.
  ///\param s The _source
  ///\param t The _target
  void run(Node& s, Node& t) {
    addSource(s);
    addTarget(t);
    init();
    runSearch();
  }

  ///Run the algorithm between the new source and the previous target.
  ///\param s The new _source
  void run_newSource(Node& s) {
    addSource(s);
    init();
    if (_source && _target) runSearch();
  }

  ///Run the algorithm between the given target and the previous source.
  ///\param t The new _target
  void run_newTarget(Node& t) {
    addTarget(t);
    init();
    if (_source && _target) runSearch();
  }

  ///\return The previously calculated distance.
  int dist() const {
    return _dmin;
  }

  ///\return The forward path
  vector<Arc> buildForwardPath() const {
    vector<Arc> path;
    if (_nodemin != INVALID) {
      fpath(path, _nodemin);
    }
    return path;
  }

  ///\return The backward path
  vector<Arc> buildBackwardPath() const {
    vector<Arc> path;
    if (_nodemin != INVALID) {
      bpath(path, _nodemin);
    }
    return path;
  }

};

#endif
