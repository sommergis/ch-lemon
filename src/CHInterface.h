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

#ifndef CHINTERFACE_H_
#define CHINTERFACE_H_

#include <lemon/list_graph.h>
#include "CH/CHData.h"
#include "CH/Preprocess.h"
#include "CH/CHSearch.h"
#include "CH/PathReconstruct.h"

using std::ifstream;
using lemon::ListDigraph;

///This class creates an interface for using the CH search algorithm on a graph.
template <class Prior>
class CHInterface {

  typedef ListDigraph Graph;
  typedef Graph::Node Node;
  typedef Graph::ArcMap<int> Cost;
  typedef Graph::NodeMap<StaticDigraph::Node> refMap;

private:

  CHData _chdata;

  CHSearch *_chsearch;
  PathReconstruct *_pathrec;

  refMap *_forward_noderef;
  refMap *_backward_noderef;

public:

  ///Creates the interface object.
  ///\param graph The ListDigraph in which we want to find shortest paths
  ///\param cost The ArcMap containing the arc costs
  CHInterface(ListDigraph& graph, Cost& cost):
    _chdata(graph, cost) {
    _chdata.nodes = countNodes(graph);

    _chsearch = NULL;
    _pathrec = NULL;

    _forward_noderef = NULL;
    _backward_noderef = NULL;
  }

  ///Destroys the interface object.
  ///The original graph and the arc costs won't be deleted.
  ~CHInterface() {
    delete _pathrec;
    delete _chsearch;
  }

  ///Runs the preprocessing algorithm.
  ///The original graph will be changed.
  void createCH() {
    Preprocess<Prior> preproc(_chdata);
    preproc.run();
    _chsearch = new CHSearch(_chdata);
    _pathrec = new PathReconstruct(_chdata, *_chsearch);
    _forward_noderef = _chdata.forward_nodeRef;
    _backward_noderef = _chdata.backward_nodeRef;
  }

  void addOrder(vector<int>& order) {
    _chdata.setOrder(order);
  }

  ///The order in which the nodes were contracted.
  vector<int> getOrder() {
    return _chdata.getOrder();
  }

  ///Runs the search from Node s to Node t.
  ///\param s The source node
  ///\param t The target node
  void runSearch(Node s, Node t) const {
    _chsearch->run((*_forward_noderef)[s], (*_backward_noderef)[t]);
  }

  ///Runs the search from a new source node using the previous search results.
  ///\param s The new source node
  void runSearch_newSource(Node s) const {
    _chsearch->run_newSource((*_forward_noderef)[s]);
  }

  ///\return The distance between the previous search's source and target
  int dist() const {
    return _chsearch->dist();
  }

  ///\return The path between the previous search's source and target
  vector<ListDigraph::Arc> getPath() const {
    return _pathrec->getPath();
  }

  ///Clears the previous results.
  void clear() const {
    _chsearch->clear();
  }
};

#endif
