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

#ifndef CH_PRIORITY_H
#define CH_PRIORITY_H

#include <lemon/list_graph.h>

///The interface used to calculate the priority of the nodes.
///Every class that calculates the node orders should be subclasses of this class.
class Priority {

  typedef ListDigraph Graph;
  typedef Graph::Node Node;

public:

  virtual ~Priority() {}

  ///Creates the initial order of the nodes.
  ///This method must be called before running the preprocessing algorithm.
  virtual void init() {}

  ///\return The node which should be processed next
  virtual Node nextNode() = 0;

  ///Finalize the given node.
  ///\param v The node
  virtual void finalize(Node v) {}
};

#endif
