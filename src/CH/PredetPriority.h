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

#ifndef CH_PREDET_PRIORITY_H
#define CH_PREDET_PRIORITY_H

#include <vector>
#include <lemon/list_graph.h>
#include "CHData.h"
#include "Priority.h"

using namespace std;
using namespace lemon;

///This class uses a given order which should be available in the CHData object given to the constructor.
class PredetPriority: public Priority {

  typedef ListDigraph Graph;
  typedef Graph::Node Node;

private:

  int _index;
  int _nodes;
  Graph *_graph;
  vector<int> *_order;

public:

  PredetPriority(CHData& chdata) {
    _graph = chdata.graph;
    _order = chdata.order;
    _index = 0;
    _nodes = chdata.nodes;
  }

  Node nextNode() {
    if (_index < _nodes) {
      return _graph->nodeFromId((*_order)[_index]);
    } else {
      return INVALID;
    }
  }

  void finalize(Node v) {
    ++_index;
  }
};

#endif
