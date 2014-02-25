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

#ifndef PATHRECONSTRUCT_H
#define PATHRECONSTRUCT_H

#include <vector>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

using std::vector;
using std::pair;
using lemon::ListDigraph;
using lemon::StaticDigraph;
using lemon::INVALID;

///A class used to reconstruct the paths from a CHSearch.
class PathReconstruct {

private:

  CHSearch *_chsearch;
  ListDigraph::ArcMap<pair<ListDigraph::Arc, ListDigraph::Arc> > *_pack;

  StaticDigraph::ArcMap<ListDigraph::Arc> *_forward_back_arc_ref;
  StaticDigraph::ArcMap<ListDigraph::Arc> *_backward_back_arc_ref;

  ///Recursively unpack the new arcs in the path.
  ///\param path The partially completed path of original arcs
  ///\param e The current arc
  void unpack(vector<ListDigraph::Arc>& path, ListDigraph::Arc e) const {
    if ((*_pack)[e].first == INVALID) {
      path.push_back(e);
    } else {
      unpack(path, (*_pack)[e].first);
      unpack(path, (*_pack)[e].second);
    }
  }

public:

  ///Initializes the references.
  ///\param chdata The CHData containing every necessary pointer
  ///\param _chsearch The CHSearch class used to find the shortest path
  PathReconstruct(CHData& chdata, CHSearch& chsearch):
    _chsearch(&chsearch) {
    _pack = chdata.pack;
    _forward_back_arc_ref = chdata.forward_backArcRef;
    _backward_back_arc_ref = chdata.backward_backArcRef;
  }

  ///\return The sortest path containing the original arcs
  vector<ListDigraph::Arc> getPath() const {
    vector<ListDigraph::Arc> path;
    vector<StaticDigraph::Arc> forward_path = _chsearch->buildForwardPath();
    vector<StaticDigraph::Arc> backward_path = _chsearch->buildBackwardPath();
    for (unsigned int i = 0; i < forward_path.size(); ++i) {
      unpack(path, (*_forward_back_arc_ref)[forward_path[i]]);
    }
    for (unsigned int i = 0; i < backward_path.size(); ++i) {
      unpack(path, (*_backward_back_arc_ref)[backward_path[i]]);
    }
    return path;
  }

};

#endif
