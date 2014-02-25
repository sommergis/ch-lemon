#include <iostream>
#include <lemon/random.h>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <lemon/core.h>
#include<lemon/time_measure.h>
#include <lemon/dimacs.h>
#include "../CHInterface.h"
#include "../CH/Utils/SearchDijkstra.h"
#include "../CH/DefaultPriority.h"
#include "../CH/PredetPriority.h"

using namespace std;
using namespace lemon;

void Sameorder_test(string filename1, string filename2, int tests = 1000) {
  cout << "SAME ORDER TEST\n";

  Random rnd;
  Timer t;
  ListDigraph *g = new ListDigraph;
  ListDigraph::ArcMap<int> *c = new ListDigraph::ArcMap<int>(*g);

  cout << "reading graph\n";
  ifstream f(filename1.c_str());
  ListDigraph::Node v;
  readDimacsSp(f, *g, *c, v);
  int n = countNodes(*g);
  cout << "nodes: " << n << " arcs: " << countArcs(*g) << "\n";

  cout << "creating ch\n";
  t.restart();
  CHInterface<DefaultPriority> *ch = new CHInterface<DefaultPriority>(*g, *c);
  ch->createCH();
  cout << t << "\n";
  cout << "nodes: " << n << " arcs: " << countArcs(*g) << "\n";

  cout << "saving order\n";
  vector<int> order = ch->getOrder();
  delete ch;
  delete c;
  delete g;

  cout << "reading second graph\n";
  g = new ListDigraph;
  c = new ListDigraph::ArcMap<int>(*g);
  ifstream f2(filename2.c_str());
  readDimacsSp(f2, *g, *c, v);

  cout << "creating dijkstra\n";
  StaticDigraph sg;
  StaticDigraph::ArcMap<int> sc(sg);
  ListDigraph::NodeMap<StaticDigraph::Node> noderef(*g);
  DigraphCopy<ListDigraph, StaticDigraph> cg(*g, sg);
      cg.nodeRef(noderef);
      cg.arcMap(*c, sc);
      cg.run();
  SearchDijkstra dijkstra(sg, sc);

  cout << "creating new ch\n";
  CHInterface<PredetPriority> *pch = new CHInterface<PredetPriority>(*g, *c);
  pch->addOrder(order);
  t.restart();
  pch->createCH();
  cout << t << "\n";
  cout << "nodes: " << n << " arcs: " << countArcs(*g) << "\n";

  cout << "creating test cases\n";
  vector<int> source;
  vector<int> target;
  vector<int> ddist;
  vector<int> chdist;
  for (int i = 0; i < tests; ++i) {
    source.push_back(rnd[n]);
    target.push_back(rnd[n]);
  }

  cout << "running dijkstra\n";
  t.restart();
  for (int i = 0; i < tests; ++i) {
    StaticDigraph::Node s = noderef[(*g).nodeFromId(source[i])];
    StaticDigraph::Node t = noderef[(*g).nodeFromId(target[i])];
    dijkstra.addSource(s);
    while (dijkstra.nextNode() != INVALID && !dijkstra.processed(t)) {
      dijkstra.processNextNode();
    }
    StaticDigraph::Arc e = dijkstra.predArc(t);
    int dist = 0;
    while (e != INVALID) {
      dist += sc[e];
      e = dijkstra.predArc(sg.source(e));
    }
    ddist.push_back(dist);
    dijkstra.clear();
  }
  cout << t << "\n";

  cout << "running ch\n";
  t.restart();
  for (int i = 0; i < tests; ++i) {
    ListDigraph::Node s = (*g).nodeFromId(source[i]);
    ListDigraph::Node t = (*g).nodeFromId(target[i]);
    pch->runSearch(s, t);
    vector<ListDigraph::Arc> path = pch->getPath();
    int dist = 0;
    for (unsigned int i = 0; i < path.size(); ++i) {
      dist += (*c)[path[i]];
    }
    chdist.push_back(dist);
  }
  cout << t << "\n";

  cout << "checking results\n";
  int wrong = 0;
  for (int i = 0; i < tests; ++i) {
    if (ddist[i] != chdist[i]) {
      //cout << ddist[i] << " " << chdist[i] << "\n";
      ++wrong;
    }
  }
  if (wrong != 0) {
    cout << "Wrong path: " << wrong << "\n";
  }

  cout << "finished\n";
}
