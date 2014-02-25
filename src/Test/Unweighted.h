#include <iostream>
#include <lemon/random.h>
#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include <lemon/core.h>
#include<lemon/time_measure.h>
#include <lemon/dimacs.h>
#include "../CHInterface.h"
#include "../CH/Utils/bfs.h"
#include "../CH/DefaultPriority.h"

using namespace std;
using namespace lemon;

void Unweighted_test(string filename, int tests = 1000) {
  cout << "UNWEIGHTED TEST\n";

  Random rnd;
  Timer t;
  ListDigraph *g = new ListDigraph;
  ListDigraph::ArcMap<int> *c = new ListDigraph::ArcMap<int>(*g);

  cout << "reading graph\n";
  ifstream f(filename.c_str());
  ListDigraph::Node v;
  readDimacsSp(f, *g, *c, v);
  int n = countNodes(*g);
  cout << "nodes: " << n << " arcs: " << countArcs(*g) << "\n";

  for (ListDigraph::ArcIt e(*g); e != INVALID; ++e) {
    (*c)[e] = 1;
  }

  cout << "creating bfs\n";
  StaticDigraph sg;
  StaticDigraph::ArcMap<int> sc(sg);
  ListDigraph::NodeMap<StaticDigraph::Node> noderef(*g);
  DigraphCopy<ListDigraph, StaticDigraph> cg(*g, sg);
      cg.nodeRef(noderef);
      cg.arcMap(*c, sc);
      cg.run();
  Bfs dijkstra(sg);

  cout << "creating ch\n";
  t.restart();
  CHInterface<DefaultPriority> ch(*g, *c);
  ch.createCH();
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

  cout << "running bfs\n";
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
    ch.runSearch(s, t);
    vector<ListDigraph::Arc> path = ch.getPath();
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
      ++wrong;
    }
  }
  if (wrong != 0) {
    cout << "Wrong path: " << wrong << "\n";
  }

  delete c;
  delete g;

  cout << "finished\n";
}
