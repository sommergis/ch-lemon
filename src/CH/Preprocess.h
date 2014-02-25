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

#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <vector>
#include <lemon/core.h>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <lemon/adaptors.h>
#include "Utils/ContractDijkstra.h"
#include "CHData.h"

/*
using std::max;
using std::cout;
using std::vector;
using std::pair;
using std::make_pair;
using lemon::ListDigraph;
using lemon::rnd;
using lemon::StaticDigraph;
using lemon::SubDigraph;
*/

using namespace std;
using namespace lemon;

///The class used to preprocess the graph.
template <class Prior>
class Preprocess {

  typedef ListDigraph Graph;
  typedef Graph::Node Node;
  typedef Graph::Arc Arc;
  typedef Graph::OutArcIt OutArcIt;
  typedef Graph::InArcIt InArcIt;
  typedef Graph::ArcMap<int> Cost;
  typedef Graph::NodeMap<int> NodeMap;

  typedef SubDigraph<Graph, Graph::NodeMap<bool>, Graph::ArcMap<bool> > MySubDigraph;

private:

  CHData *_data;
  Graph *_graph;
  Cost *_cost;
  vector<int> *_order_vector;

  Graph::NodeMap<bool> _finalized;

  Prior _priority;
  NodeMap _searchorder;

  ListDigraph::ArcMap<pair<ListDigraph::Arc,ListDigraph::Arc> > *_pack;

  ContractDijkstra _dijkstra;

  ///Contracts the given node.
  ///\param v The node to be contracted
  void contract(Node& v) {
    // max cost of the out arcs
    int max_out = 0;
    // "delete" node from further searches
    _dijkstra.addContractedNode(v);
    for (OutArcIt e(*_graph, v); e != INVALID; ++e) {
      if (_finalized[_graph->target(e)]) continue;
      if ((*_cost)[e] > max_out) max_out = (*_cost)[e];
    }
    // determining new arcs
    for (InArcIt e(*_graph, v); e != INVALID; ++e) {
      Node w = _graph->source(e);
      if (_finalized[w]) continue;
      if(_graph->id(w) == _graph->id(v)) continue;
      int limit = (*_cost)[e] + max_out;
      _dijkstra.addSource(w);
      while(_dijkstra.nextNode() != INVALID && _dijkstra.currentDist(_dijkstra.nextNode()) <= limit) {
        _dijkstra.processNextNode();
      }
      for (OutArcIt f(*_graph, v); f != INVALID; ++f) {
        if (_finalized[_graph->target(f)]) continue;
        if(_graph->id(_graph->target(f)) == _graph->id(v)) continue;
        int id = _graph->id(_graph->target(f));
        if ((!_dijkstra.processed(_graph->target(f))) || (_dijkstra.currentDist(_graph->target(f)) > (*_cost)[e] + (*_cost)[f])) {
          Arc t = INVALID;
          for (OutArcIt r(*_graph, w); r != INVALID; ++r) {
            if (_graph->id(_graph->target(r)) == id) {
              t = r;
              break;
            }
          }
          if (t == INVALID) {
            Arc sa = _graph->addArc(_graph->source(e),_graph->target(f));
            (*_cost)[sa] = (*_cost)[e] + (*_cost)[f];
            (*_pack)[sa] = make_pair(e,f);
          }
          else if ((*_cost)[t] > (*_cost)[e] + (*_cost)[f]) {
            (*_cost)[t] = (*_cost)[e] + (*_cost)[f];
            (*_pack)[t] = make_pair(e,f);
          }
        }
      }
      _dijkstra.clear();
    }
  }

  ///Creates the search graphs and adds their pointers to the CHData struct given in the contructor.
  void createSearchGraphs() {
    Graph::NodeMap<bool> nf1(*_graph,true);
    Graph::NodeMap<bool> nf2(*_graph,true);
    Graph::ArcMap<bool> af1(*_graph,false);
    Graph::ArcMap<bool> af2(*_graph,false);
    // forward edges
    MySubDigraph sg1(*_graph,nf1,af1);
    // backward edges
    MySubDigraph sg2(*_graph,nf2,af2);

    vector<Arc> to_reverse;
    vector<Arc> not_to_reverse;

    for (Graph::ArcIt e(*_graph); e != INVALID; ++e) {
      Node u = _graph->source(e);
      Node v = _graph->target(e);
      if (_searchorder[u] < _searchorder[v]) not_to_reverse.push_back(e);
      else if (_searchorder[u] > _searchorder[v]) to_reverse.push_back(e);
    }

    for (unsigned int i = 0; i < to_reverse.size(); ++i) {
      _graph->reverseArc(to_reverse[i]);
      af2[to_reverse[i]] = true;
    }

    for (unsigned int i = 0; i < not_to_reverse.size(); ++i) {
      af1[not_to_reverse[i]] = true;
    }

    _data->forward_graph = new StaticDigraph();
    _data->forward_cost = new StaticDigraph::ArcMap<int>(*(_data->forward_graph));
    _data->forward_nodeRef = new Graph::NodeMap<StaticDigraph::Node>(*_graph);
    _data->forward_backArcRef = new StaticDigraph::ArcMap<Arc>(*(_data->forward_graph));

    _data->backward_graph = new StaticDigraph();
    _data->backward_cost = new StaticDigraph::ArcMap<int>(*(_data->backward_graph));
    _data->backward_nodeRef = new Graph::NodeMap<StaticDigraph::Node>(*_graph);
    _data->backward_backArcRef = new StaticDigraph::ArcMap<Arc>(*(_data->backward_graph));

    DigraphCopy<MySubDigraph, StaticDigraph> cg1(sg1, *(_data->forward_graph));
    cg1.nodeRef(*(_data->forward_nodeRef));
    cg1.arcMap(*_cost, *(_data->forward_cost));
    cg1.arcCrossRef(*(_data->forward_backArcRef));
    cg1.run();

    DigraphCopy<MySubDigraph, StaticDigraph> cg2(sg2, *(_data->backward_graph));
    cg2.nodeRef(*(_data->backward_nodeRef));
    cg2.arcMap(*_cost, *(_data->backward_cost));
    cg2.arcCrossRef(*(_data->backward_backArcRef));
    cg2.run();
  }

public:

  ///Initializes the preprocessing algorithm.
  ///\param chdata The CHData struct containing the graph and the arc costs
  Preprocess(CHData& chdata):
  _data(&chdata), _finalized(*_data->graph, false), _priority(*_data), _searchorder(*_data->graph), _dijkstra(*_data->graph, *_data->cost) {
    _graph = _data->graph;
    _cost = _data->cost;
    _order_vector = NULL;

    _pack = new Graph::ArcMap<pair<Arc,Arc> >(*_graph, make_pair(INVALID, INVALID));
    _data->pack = _pack;
  }

  ///Run the preprocessing algorithm.
  void run() {
    _order_vector = new vector<int>(_data->nodes);
    int order = 0;
    _priority.init();
    Node v = _priority.nextNode();
    while (v != INVALID) {
      _searchorder[v] = order;
      (*_order_vector)[order] = _graph->id(v);
      contract(v);
      _priority.finalize(v);
      _finalized[v] = true;
      order++;

      v = _priority.nextNode();
    }
    createSearchGraphs();
    if (_data->local_order) _data->order = _order_vector;
    else delete _order_vector;
  }
};

#endif
