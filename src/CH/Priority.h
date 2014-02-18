#ifndef CH_PRIORITY_H
#define CH_PRIORITY_H

#include <lemon/list_graph.h>

/**
 * The iterface used to calculate the priority of the nodes.
 * Every class that calculates the node orders should be subclasses of this class.
 */
class Priority {

	typedef ListDigraph Graph;
	typedef Graph::Node Node;

public:

	virtual ~Priority() {}

	/**
	 * Creates the initial order of the nodes.
	 * This method must be called before running the preprocessing algorithm.
	 */
	virtual void init() {}

	/**
	 * @return The node which should be processed next
	 */
	virtual Node nextNode() = 0;

	/**
	 * Finalize the given node.
	 * @param v The node
	 */
	virtual void finalize(Node v) {}
};

#endif
