#include <cstdlib>
#include <cstdio>

#include "PDF.h"
#include "Graph.h"
#include "PDFGraph.h"

int main(int argc, char *argv[])
{
	// Read the graph from the standard input
	Graph g(cin);
	g.init_PDF("graph.pdf");
	g.draw();
	Graph* a = g.breadth_first(0);
	Graph* b = g.depth_first(0);
	g.shortest_path(0, 0, &cout);
	Graph* c = g.shortest_paths(0);
	c = NULL;
	g.finish_PDF();
}