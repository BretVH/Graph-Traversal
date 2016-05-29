/*
 * File:   GraphAlg.cpp
 * Author: bret and daniel
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Graph.h"
#include "PDF.h"
#include "PDFGraph.h"

using namespace std;

//  DeConstructor
//  insures that no memory is leaked
//
Graph::~Graph()
{
	delete[] nodes;
	nodes = NULL;
	for (int i = 0; i < n; i++)
	{
		delete[] adj[i];
		adj[i] = NULL;
	}
	delete[] adj;
	adj = NULL;
	for (int j = 0; j < n; j++)
	{
		for (int k = 0; k < n; k++)
		{
			if (arc_pos[j][k] != NULL)
				delete arc_pos[j][k];
			arc_pos[j][k] = NULL;
		}
		delete[] arc_pos[j];
		arc_pos[j] = NULL;
	}
	delete[] arc_pos;
	arc_pos = NULL;
	delete[] node_pos;
	node_pos = NULL;
}

//-----------------------------------------------------------------------------
//	Function: Graph* breadth_first( int start_i )
//  
//	Title:	Graph
//
//	Description:
//				a function which performs a breadth-first
//			traversal of a graph, the traversal is sent graphically
//			to a pdf 
//	Argument.
//	
//	Programmer: Bret Van Hof and Daniel Smith
//	
//	Date: 2/20/2014
//
//  Returns: a spanning tree representing the result of a breadth-first 
//		traversal 
//  Parameters: int representing the node from which to start traversing 
//  Version: 1.0
//  Environment: AMD FX 8-core Processor 8350 4.0GHZ
//				 Windows 8.1 Pro 64-bit
//
//-----------------------------------------------------------------------------
Graph* Graph::breadth_first(int start_i)
{
	vector<int> queue;
	// Create a copy of the ndoes of this graph, to serve as a spanning tree
	Graph *spanning_tree = node_subgraph();
	int front, rear, root;
	front = rear = 0;
	visit_node(start_i, " Breadth First Algorithm ", spanning_tree);
	set_node_state(start_i, 2);
	queue.push_back(start_i);
	rear++; 
	while (front != rear)
	{
		root = queue[front];
		for (int k = 0; k < n; k++)
			if (adj[root][k] && !(nodes[k].state == 2))
			{
				set_node_state(k, 2);
				queue.push_back(k);
				rear++;
				spanning_tree->add_arc(k, root);
				visit_node(k, " Breadth First Algorithm ", spanning_tree);
			}
		front++;
	}
	// Draw the completed version
	draw(0, "Completed Breadth-first traversal", spanning_tree);
	return spanning_tree;
}


//-----------------------------------------------------------------------------
//	Function: Graph* depth_first( int start_i )
//  
//	Title:	Graph
//
//	Description:
//			A depth-first traversal of a graph, the traversal is 
//	output graphically to a pdf
//	
//	Programmer: Bret Van Hof and Daniel Smith
//	
//	Date: 2/20/2014
//
//  Returns: a spanning tree representing the result of a depth-first 
//		traversal 
//  Parameters: int representing the node from which to start traversing 
//  Version: 1.0
//  Environment: AMD FX 8-core Processor 8350 4.0GHZ
//				 Windows 8.1 Pro 64-bit
//
//-----------------------------------------------------------------------------
Graph* Graph::depth_first(int start_i)
{
	// To prepare for the search, set all the the node states to 0
	set_all_node_states(0);

	// Draw the graph as it is given
	pdf->new_page("Running Depth-first traversal:");
	pdf->draw();

	// Create a copy of the ndoes of this graph, to serve as a spanning tree
	Graph *spanning_tree = node_subgraph();

	// Perform the algorithm
	depth_first(start_i, spanning_tree);

	// Draw the completed version
	draw(0, "Completed depth-first traversal", spanning_tree);
	return spanning_tree;
}


//-----------------------------------------------------------------------------
//	Function: Graph* depth_first( int i, Graph* spanning_tree )
//  
//	Title:	Graph
//
//	Description:
//				helper method for depth-first traversal
//	
//	Programmer: Bret Van Hof and Daniel Smith
//	
//	Date: 2/20/2014
//
//  Returns: a spanning tree representing the result of a breadth-first 
//		traversal 
//  Parameters: int representing the node from which to start traversing,
//       the spanning tree used in representing the traversal 
//  Version: 1.0
//  Environment: AMD FX 8-core Processor 8350 4.0GHZ
//				 Windows 8.1 Pro 64-bit
//
//-----------------------------------------------------------------------------
void Graph::depth_first(int i, Graph *spanning_tree)
{
	visit_node(i, " Depth First Algorithm ", spanning_tree);
	set_node_state(i, 2);
	for (int l = 0; l < n; l++)
	{
		if (adj[i][l] && !(nodes[l].state == 2))
		{
			spanning_tree->add_arc(l, i);
			depth_first(l, spanning_tree);
		}
	}
}

//-----------------------------------------------------------------------------
//	Function: Graph* shortest_paths( int start_i )
//  
//	Title:	Graph
//
//	Description:
//				Traversal of a graph using Dijkstra's algorithm,
//     the shortest path from any node is displayed graphically in a pdf
//	
//	Programmer: Bret Van Hof and Daniel Smith
//	
//	Date: 2/20/2014
//
//  Returns: a spanning tree representing the shortest path to any node
//
//  Parameters: int representing the node to which the path goes 
//  Version: 1.0
//  Environment: AMD FX 8-core Processor 8350 4.0GHZ
//				 Windows 8.1 Pro 64-bit
//
//-----------------------------------------------------------------------------
Graph* Graph::shortest_paths(int start_i)
{
	Graph *spanning_tree = node_subgraph();
	vector<int> dist;
	// Initialize all distances as INFINITE and stpSet[] as false
	for (int i = 0; i < n; i++)
		dist.push_back(INT_MAX), set_node_state(i, 0);

	// Distance of source vertex from itself is always 0
	dist[start_i] = 0;
	for (int count = 0; count < n; count++)
	{
		int min = INT_MAX, min_index;

		for (int v = 0; v < n; v++)
			if (!(nodes[v].state == 2) && dist[v] <= min)
				min = dist[v], min_index = v;

		int u = min_index;

		if (u == -1)
			break;

		visit_node(u, " Shortest Path Algorithm ", spanning_tree);
		set_node_state(u, 2);


		for (int v = 0; v < n; v++)
			if (!(nodes[v].state == 2) && adj[u][v] && dist[u] != INT_MAX
				&& dist[u] + adj[u][v] < dist[v])
			{
				spanning_tree->remove_outgoing_arcs(v);
				dist[v] = dist[u] + adj[u][v];
				spanning_tree->add_arc(v, u);
			}
	}

	draw(0, "Completed Shortest path traversal", spanning_tree);
	return spanning_tree;
}


//-----------------------------------------------------------------------------
//	Function: Graph* shortest_path(  int source_i, int dset_i, ostream *out )
//  
//	Title:	Graph
//
//	Description:
//				Traversal of a graph using Dijkstra's algorithm,
//     the length of the shortest path to each node is output to the console
//	
//	Programmer: Bret Van Hof and Daniel Smith
//	
//	Date: 2/20/2014
//
//  Returns: N/A
//
//  Parameters: int representing the node to which the path goes, an unused int,
//	and a reference to an ostream which is used to print the info 
//  Version: 1.0
//  Environment: AMD FX 8-core Processor 8350 4.0GHZ
//				 Windows 8.1 Pro 64-bit
//
//-----------------------------------------------------------------------------
void Graph::shortest_path(int source_i, int dset_i, ostream *out)
{
	vector<int> dist;

	for (int i = 0; i < n; i++)
		dist.push_back(INT_MAX), set_node_state(i, 0);

	dist[source_i] = 0;

	for (int count = 0; count < n - 1; count++)
	{

		int min = INT_MAX, min_index;

		for (int v = 0; v < n; v++)
			if (!(nodes[v].state == 2) && dist[v] <= min)
				min = dist[v], min_index = v;
		int u = min_index;

		// Mark the picked vertex as processed
		set_node_state(u, 2);

		for (int v = 0; v < n; v++)

			if (!(nodes[v].state == 2) && adj[u][v] && dist[u] != INT_MAX
				&& dist[u] + adj[u][v] < dist[v])
				dist[v] = dist[u] + adj[u][v];
	}
	// print the constructed distance array
	(*out) << "Vertex   Distance from Source" << endl;
	for (int mi = 0; mi < n; mi++)
		(*out) << nodes[mi].name << "  " << dist[mi] << endl;
}

