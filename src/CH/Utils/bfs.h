#ifndef BFS_H_INCLUDED
#define BFS_H_INCLUDED

#include <queue>
#include <lemon/static_graph.h>

using namespace std;
using namespace lemon;

class bfs {

	typedef StaticDigraph Graph;
	typedef Graph::Node Node;
	typedef Graph::Arc Arc;
	typedef Graph::ArcMap<int> ArcMap;
	typedef Graph::NodeMap<bool> CheckedMap;
	typedef Graph::NodeMap<int> DistMap;
	typedef Graph::OutArcIt OutArcIt;
	typedef Graph::NodeMap<Arc> PredMap;

	private:

	Graph *g;
	CheckedMap checked;
	DistMap dist;
	PredMap pred;
	queue<Node> q;
	queue<Node> reset;

	public:

	bfs(Graph& old_g):
	g(&old_g), checked(*g, false), dist(*g), pred(*g, INVALID) {}

	void addSource(Node s) {
		q.push(s);
		checked[s] = true;
		reset.push(s);
	}

	bool hasNext() const {
		return q.empty();
	}

	Node nextNode() const {
		return q.front();
	}

	bool processed(Node v) const {
		return checked[v];
	}

	Node processNextNode() {
		Node v = q.front();
		q.pop();
		for (OutArcIt e(*g,v); e!=INVALID; ++e) {
			Node w = g->target(e);
			if (!checked[w]) {
				q.push(w);
				dist[w] = dist[v] + 1;

				pred[w] = e;
				checked[w] = true;

				reset.push(w);
			}
		}
		return v;
	}

	Arc predArc(Node v) const {
		return pred[v];
	}

	void clear() {
		q = queue<Node>();
		while (!reset.empty()) {
			Node v = reset.front();
			reset.pop();
			dist[v] = 0;
			pred[v] = INVALID;
			checked[v] = false;
		}
	}
};

#endif // BFS_H_INCLUDED
