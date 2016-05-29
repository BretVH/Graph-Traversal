/****************************************************************************/
/** 																	   **/
/** Graph.cpp - Class representing a directed/undirected graph		       **/
/** 																	   **/
/** Copyright (C) 2014 Michael M. Stark, ALL RIGHTS RESERVED			   **/
/** 																	   **/
/**------------------------------------------------------------------------**/
/** Author:  Michael M. Stark											   **/
/****************************************************************************/

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Graph.h"

#ifdef GRAPHICAL
#include "PDFGraph.h"
#endif

// If 'Verbose' is true, information about the input file
// is written to the standard output as it is read. 
bool Verbose = false;

/* Graph Representation
 *
 * A graph is collection of nodes connected by arcs (called "edges"
 * in the case of an undirected graph).  If there is an arc (edge)
 * joining node 'a' to node 'b', nodes 'b' is said to be "adjacent"
 * to 'a'.
 *
 * In this implementation, the nodes are represented by an array of
 * 'GraphNode' instances; the edges or arcs, as an adjacency matrix.
 * If the graph nodes are numbered 0, 1, ..., n - 1 ('n' is the number
 * of nodes), the adjacency matrix is represented by a two-dimensional
 * 'n'-by-'n' array named 'adjacency'.  The value of 'adjacency[i][j]'
 * is 1 if, and only if, node 'j' is adjacent to node 'i'.
 *
 * Here is an example:
 *
 *                +---+       +---+               0   1   2   3   4
 *         ------>| 1 |------>| 2 |	        +---+---+---+---+---+
 *        |       +---+    __ +---+	      0 | 0 | 1 | 0 | 0 | 0 |
 *        |         ^       /| | ^ 	        +---+---+---+---+---+
 *      +---+       |      /   | | 	      1 | 0 | 0 | 1 | 0 | 0 |
 *      | 0 |       |     /    | | 	        +---+---+---+---+---+
 *      +---+       |    /     | | 	      2 | 0 | 0 | 0 | 0 | 1 |
 *        ^         |   /      v | 	        +---+---+---+---+---+
 *        |       +---+       +---+	      3 | 1 | 1 | 1 | 0 | 0 |
 *         -------| 3 |<------| 4 |	        +---+---+---+---+---+
 *                +---+       +---+	      4 | 0 | 0 | 1 | 1 | 0 |
 *					        +---+---+---+---+---+
 *
 *           (Directed) Graph                     Adjacency Matrix
 *
 * The list of edges adjacent to node 'i' are thus the nodes corresponding
 * to the columns of the adjacency matrix that have unit values.
 *
 * Weighted Arcs
 * -------------
 *
 * For some graph problems, the arcs have an associated weight; for example,
 * if the nodes represented cities, the arc weights could indicate the
 * distance between cities, or travel time, or cost of a bus or airplane
 * ticket.  Weighted arcs are naturally represented by a generalized
 * adjacency matrix in which the value of 'adj[i][j]' is the weight of
 * the arc from node 'i' to node 'j'.  If there is no such arc, the
 * value 'adj[i][j]' remains zero.  If arc weights are added to the
 * example above, the weighted adjancency matrix looks like this:
 *
 *
 *           0.1  +---+  0.7  +---+               0   1   2   3   4
 *         ------>| 1 |------>| 2 |	        +---+---+---+---+---+
 *        |       +---+    __ +---+	      0 | 0 |0.1| 0 | 0 | 0 |
 *        |         ^       /| | ^ 	        +---+---+---+---+---+
 *      +---+       |   0.8/   | | 	      1 | 0 | 0 |0.7| 0 | 0 |
 *      | 0 |    0.9|     /    | |0.2 	        +---+---+---+---+---+
 *      +---+       |    /  0.6| | 	      2 | 0 | 0 | 0 | 0 |0.6|
 *        ^         |   /      v | 	        +---+---+---+---+---+
 *        |  0.1  +---+       +---+	      3 |0.1|0.9|0.8| 0 | 0 |
 *         -------| 3 |<------| 4 |	        +---+---+---+---+---+
 *                +---+  0.5  +---+	      4 | 0 | 0 |0.2|0.5| 0 |
 *					        +---+---+---+---+---+
 *
 * However, this only works if the arc weights are nonzero (otherwise
 * an arc with zero weight is indistinguishable from no arc at all).
 *
 * For this implementation, we assume that the arc weights are all positive,
 * and the default weight is 1.
 */


 /****************************************************************************/
 /***                       Implementation of Graph						   ***/
 /****************************************************************************/

Graph::Graph(const string& filename)
// Constructs this graph by reading from 'filename'
{
	// Instantiate an 'ifstream' (an 'istream' subclass that reads from a file)
	ifstream in;
	in.open(filename.c_str());
	if (!in) {
		// Just crash if the file can't be opened
		cerr << "Can't read from " << filename << ".  Exiting.\n";
		exit(1);
	}

	// call the 'read' function, which works on a general 'istream'
	read(in, filename);

	// close the 'ifstream'
	in.close();
}



Graph::Graph(const Graph& src)
// Copy constructor
{
	n = src.n;
	init(n);
	for (int k = 0; k < n; k++)
		nodes[k] = src.nodes[k];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			adj[i][j] = src.adj[i][j];
		}
	}

	weighted = src.weighted;
	directed = src.directed;

#ifdef GRAPHICAL
	scale = src.scale;
	for (int k = 0; k < n; k++)
		node_pos[k] = src.node_pos[k];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			arc_pos[i][j] = src.arc_pos[i][j];
		}
	}

	// just copy the pointer to the 'pdf' object
	pdf = src.pdf;
#endif

}


/****************/
/* Stream Input */
/****************/

static string get_string(istream& in)
// Extracts an optionally quoted string from the line that remains in 'in'
// Leading and trailing whitespace is removed, and if what remains is
// contained in single or double quotes, they are removed as well.
// Some examples:
//
//   "  word"   returns "word"
//   "  some words  "   returns "some words"
//   "  'quoted string' "   returns "quoted string"
//
// (Double quotes work too, but they clash with the notation)  
{
	// read up to the end of the line to a string
	string line;
	getline(in, line);

	// this works by finding the "start" and "end" of the string to be returned
	int start = 0;            // marks the start of the content substring
	int end = line.length();  // marks one past the end of the substring

	// skip to the first non-space character in 'line'
	while (start < end && isspace(line.at(start)))
		start++;

	// if the end of 'line' is reached, the extracted string is empty
	if (start >= end)
		return string();  // (returns the empty string)

	 // now find the end of the string by backing over trailing spaces
	// (there must be at least one non-space character between 'start' and 'end')
	while (isspace(line.at(end - 1)))
		end--;

	// check for quotation marks
	if (line.at(start) == '"' || line.at(start) == '\'') {
		char quote = line.at(start);
		// check for a matching closing quote
		if (line.at(end - 1) == quote) {
			start++;
			end--;
		}
		// if there is no matching closing quote, regard the quote character
		// as part of the string
	}

	// return the substring from 'start' through 'end - 1'
	return line.substr(start, end - start);
}

// These functions help check for errors in the input file.
// NOTE: for this assignment you can assume the input is correct,
//       the checking is done mostly to help construct valid files.
bool check_node_index(int index, int n_nodes,
	const string& source_name, int line_num)
	// Checks that 'index' is in the range 1..n_nodes (including 'n_nodes')
	// If it is, true is returned.  Otherwise an error message is written
	// to 'cerr' and false is returned.
{
	if (index <= 0 || index > n_nodes) {
		cerr << source_name << ":" << line_num
			<< " error: invalid node index " << index << endl;
		return false;
	}
	return true;
}

bool check_arc_indices(int start, int end, int n_nodes,
	const string& source_name, int line_num)
	// Checks that both 'start' and 'end' are in the range 1..n_node
	// (including 'n_nodes).  If they are, true is returned.  If either
	// is not, an error message is written to 'cerr' and false is returned.
{
	bool result = true;
	if (start <= 0 || start > n_nodes) {
		cerr << source_name << ":" << line_num
			<< " invalid arc start index " << start << endl;
		result = false;
	}
	if (end <= 0 || end > n_nodes) {
		cerr << source_name << ":" << line_num
			<< " invalid arc end index " << start << endl;
		result = false;
	}
	return result;
}


void Graph::read(istream& in, const string& source_name)
// Primary input function: reads the graph file into this object
// from the input stream 'in'.  'source_name' is a description
// of the input (e.g., the filename), used for verbose output
// and error messages.
{
	// In this function, /* comments describe the input format

	/* The very first line is the "magic number"
	 * It has to be "Graph"
	 */
	string magic;

	in >> magic;
	if (magic != "Graph") {
		cerr << "input source '" << source_name << "' is not in Graph format\n";
		exit(1);
	}

	/* Immediately after the "magic number" is a single line containing the
	 * number of nodes. The number of nodes is fixed at this value after it
	 * is read
	 */
	int n_nodes;
	in >> n_nodes;

	// Allocates the array of nodes and the adjacency matrix,
	// assuming there are exactly 'n_nodes' nodes
	init(n_nodes);

	/* The rest of the input is a sequence of lines.  Blank lines and
	 * lines that start with a "#" character (comment lines) are ignored.
	 * Other lines must have the form
	 *
	 *   <key> <values>
	 *
	 * where <key> is a string of non-whitespace characters and <values>
	 * depends on the key.  Whitespace separates <key> and <values>.
	 * Each <key> line either generates or modifies an object (node or arc)
	 * or sets some other state.  The keys are described within the following
	 * loop.
	 *
	 * NOTE: values that index nodes use indexing starting from 1; however,
	 *       the 'nodes' and 'adj' array have indexing starting from 0
	 *       as usual.
	 */

	 // Loop over the rest of the input, reading the <key> <value> lines
	int node_count = 0;
	int line_num = 1;
	string key;
	while (!in.eof() && key != "q") {
		line_num++;

		/* The first word in each line is the "key", which indicates
		 * what kind of object is given
		 */
		in >> key;

		// How to scan and parse the line depends on the key
		if (key.length() == 0) {
			/* Blank lines are skipped
			 */
		}
		else if (key.at(0) == '#') {
			/* Lines that start with "#" are skipped
			 */
			string line;
			getline(in, line); // gets the rest of the input line
		}

		else if (key == "node") {
			/* a "node" has a single string argument, providing a full name
			 * for the node:
			 *
			 *   node <name>
			 */
			 // check for too many nodes
			if (node_count >= n_nodes) {
				cerr << source_name << ":" << line_num
					<< " error: too many nodes!\n";
				exit(1);  // exit immediately on this one
			}
			// get the string argument
			string node_name = get_string(in);
			nodes[node_count].name = node_name;
			nodes[node_count].index = node_count + 1;
			nodes[node_count].value = 0;
			nodes[node_count].state = 0;
			nodes[node_count].flags = 0;
			node_count++;
			if (Verbose)
				cout << "read node '" << node_name << "'\n";
		}

		else if (key == "arc") {
			/* An "arc" has two integer arguments:
			 *
			 *   arc <start-index> <end-index>
			 *
			 * where <start-index> and <end-index> are integers, giving the
			 * indices of the start node and end node, respectively.
			 * (The indexing starts at 1 in the input file)
			 */
			int start, end;
			in >> start >> end;

			// check the indices
			if (!check_arc_indices(start, end, n_nodes, source_name, line_num))
				exit(1);

			adj[start - 1][end - 1] = 1;
			if (Verbose)
				cout << "read arc from " << start << " to " << end << endl;
		}

		else if (key == "weighted_arc") {
			/* A "weighted_arc" has the form
			 *
			 *   weighted_arc <start-index> <end-index> <weight>
			 *
			 * where <start-index> and <end-index> are integers, giving the
			 * indices of the start node and end node, respectively, and
			 * <weight> is a floating-point weight.
			 */
			int start, end;
			double weight;
			in >> start >> end >> weight;

			if (!check_arc_indices(start, end, n_nodes, source_name, line_num))
				exit(1);
			if (weight <= 0) {
				cerr << source_name << ":" << line_num
					<< "arc weight must be positive" << endl;
				exit(1);
			}

			adj[start - 1][end - 1] = weight;
			// set the 'weighted' flag to true
			weighted = true;

			if (Verbose)
				cout << "read weighted arc from " << start << " to " << end
				<< " with weight " << weight << endl;
		}

		else if (key == "node_value") {
			/* A "node_value" has the form
			 *
			 *   node_value <node-index> <value>
			 *
			 * where <node-index> is the integer index of the node (the
			 * indexing starts at 1) and <value> is a floating-point value
			 * assigned to the node.
			 */
			int index;
			double value;
			in >> index >> value;
			if (!check_node_index(index, n_nodes, source_name, line_num))
				exit(1);
			nodes[index - 1].value = value;
			if (Verbose)
				cout << "read node value " << index << " valued at " << value << endl;
		}

#ifdef GRAPHICAL

		// The code here assumes the graphics output is included

		else if (key == "node_pos") {
			/* A "node_pos" specifies the planar (x, y) position of a node
			 *
			 *   node_pos <node-index> <x> <y>
			 *
			 * where <node-index> is the integer index of the node (the
			 * indexing starts at 1) and (<x>, <y>) specifies the position
			 * of the node.
			 */
			int index;
			double x, y;
			in >> index >> x >> y;
			if (!check_node_index(index, n_nodes, source_name, line_num))
				exit(1);
			node_pos[index - 1] = PDFPoint(x, y);
			if (Verbose)
				cout << "read node position " << index
				<< ", ( " << x << ", " << y << ")" << endl;
		}

		else if (key == "arc_point") {
			/* An "arc_pos" specifies a planar (x, y) point of a point on an arc:
			 *
			 *   arc_pos <start> <end> <x> <y>
			 *
			 * where <start> and <end> are the integer indices of the start
			 * and end nodes of the arc (the indexing starts at 1) and
			 * (<x>, <y>) is a point on the arc.
			 */
			int start, end;
			double x, y;
			in >> start >> end >> x >> y;
			if (!check_arc_indices(start, end, n_nodes, source_name, line_num))
				exit(1);
			arc_pos[start - 1][end - 1] = new PDFPoint(x, y);
			if (Verbose)
				cout << "read arc point " << start << ", " << end
				<< ", ( " << x << ", " << y << ")" << endl;
		}

		else if (key == "scale") {
			/* A "scale" specifies a scale for the graph
			 *
			 *   scale <scale-value>
			 */
			double s;
			in >> s;
			if (s <= 0) {
				cerr << source_name << ":" << line_num
					<< " scale must be positive: " << scale << endl;
				exit(1);
			}
			scale = s;
			if (Verbose)
				cout << "read scale " << s << endl;
		}

#endif

		else {
			/* Lines beginning with unknown keys are ignored.
			 * Send message to cerr and close stdIn
			 */
			 cerr << "Unknown object key '" << key << "'\n";
			 fclose(stdin);
		}
	}

	// That's it.
	// Input file errors causes immediate failure and program exit, so there
	// is no need to return anything--if it returns, the input file was okay.
}


/******************/
/* Initialization */
/******************/

void Graph::init(int n_nodes)
// Allocates the array of nodes and the adjacency matrix
// If the graphical output is active, this also initializes the
// extra graphical data
{
	// Set 'n' to the number of nodes
	n = n_nodes;

	// Allocate the 'nodes' array; at this point the individual nodes
	// are not set; they are added as the nodes are input.
	nodes = new GraphNode[n];  // (this calls the default constructor)

	// Allocate the adjacency matrix, and initialize the elements
	// to zero.  The adjacencies are added as the arcs are input.
	adj = new double*[n];
	for (int i = 0; i < n; i++) {
		adj[i] = new double[n];
		for (int j = 0; j < n; j++) {
			adj[i][j] = 0;
		}
	}

	// Assume the arcs as unweighted and it's a directed graph
	weighted = false;
	directed = true;

#ifdef GRAPHICAL

	// Set the default scale to 72 (points)
	scale = 72;

	// Allocate the node positions array
	node_pos = new PDFPoint[n];

	// The arc points, if specified, are stored in a matrix in the
	// same format as the adjacency matrix.  Entries in the arc
	// point matrix are pointers to PDFPoint objects.
	arc_pos = new PDFPoint**[n];
	for (int i = 0; i < n; i++) {
		arc_pos[i] = new PDFPoint*[n];
		for (int j = 0; j < n; j++) {
			arc_pos[i][j] = NULL;
		}
	}

	pdf = NULL; // set the PDF object to null
#endif
}

/**********/
/* Output */
/**********/

ostream& Graph::write(ostream& out, bool brief, const string &prefix)
// Writes a text representation of this graph, in the format
// described in the 'read' function above.  
{
	// write the "magic number"
	// NOTE: This has to use "\n" (not 'endl')
	out << prefix << "Graph\n";

	// write the number of nodes
	out << prefix << n << "\n";

	// write the nodes
	if (!brief) {
		for (int i = 0; i < n; i++)
			out << prefix << "node \"" << nodes[i].name << "\"\n";
	}

	// write the node values (if there are any)
	for (int i = 0; i < n; i++)
		if (nodes[i].value != 0)
			out << prefix << "node_value " << (i + 1) << " "
			<< nodes[i].value << "\n";

	// write the node states (if there are any)
	for (int i = 0; i < n; i++)
		if (nodes[i].state != 0)
			out << prefix << "node_state " << (i + 1) << " "
			<< nodes[i].value << "\n";

	// write the arcs
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (adj[i][j] > 0) {
				if (weighted)
					out << prefix << "weighted_arc " << (i + 1) << " " << (j + 1)
					<< " " << adj[i][j] << "\n";
				else
					out << prefix << "arc " << i << " " << (j + 1) << "\n";
			}
		}
	}

#ifdef GRAPHICAL
	if (!brief) {
		// write the node positions
		for (int i = 0; i < n; i++)
			out << prefix << "node_pos " << (i + 1) << " "
			<< node_pos[i].x << " " << node_pos[i].y << "\n";
		// write the arc positions
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				if (arc_pos[i][j])
					out << prefix << "arc_point " << (i + 1) << " " << (j + 1) << " "
					<< arc_pos[i][j]->x << " " << arc_pos[i][j]->y << "\n";
	}
#endif

	return out;
}

/********************/
/* "Safe" Accessors */
/********************/

// These are accessor methods that check the node index arguments
// to be sure they are in the domain of the graph.  The behavior
// if they are not depends on the function.
//
// NOTE: The internal indexing of graph nodes starts at zero

bool check_index(int i, int n, const char *msg)
// Basic index domain check function: returns true
// if 'i' is the range 0..n-1, and false otherwise.
{
	if (i < 0) {
		cerr << msg << ": negative index " << i << endl;
		return false;
	}
	if (i >= n) {
		cerr << msg << ": index " << i
			<< " exceeds the number of nodes (" << n << ")" << endl;
		return false;
	}
	return true;
}


bool Graph::adjacent(int i, int j) const
// Returns true if there is an arc from node 'i' to node 'j'
// (The indexing starts at 0)
{
	// check the indices
	if (!check_index(i, n, "adjacent() (start index)"))
		return false;
	if (!check_index(j, n, "adjacent() (end index)"))
		return false;

	// check the adjacency matrix
	return (adj[i][j] > 0);
}

double Graph::get_arc_weight(int i, int j) const
// Returns the weight of the arc from node 'i' to node 'j'
// or 0 if there is no arc, or the indices are out of range
// The result is the actual weight stored in the 'adj' matrix
// so this is independent of the state of the 'weighted' field.
// (The indexing starts at 0)
{
	// check the indices
	if (!check_index(i, n, "get_arc_weight() (start index)"))
		return 0;
	if (!check_index(j, n, "get_arc_weight() (end index)"))
		return 0;

	// in this implementation, the adjacency matrix elements store the
	// weight of the arcs directly
	return adj[i][j];
}

GraphNode Graph::get_node(int i) const
// Returns (a copy of) node 'i', or node 0 if 'i' is out of range
// (The indexing starts at 0)
{
	// check the index
	if (!check_index(i, n, "get_node()"))
		return nodes[0]; // wrong, of course

	return nodes[i];
}

int Graph::get_node_state(int i) const
// Returns the 'state' value of node 'i',
// (The indexing starts at 0)
{
	// check the index
	if (!check_index(i, n, "get_node_state()"))
		return 0;

	return nodes[i].state;
}

double Graph::get_node_value(int i) const
// Returns the value of the node at index 'i'
// (The indexing starts at 0)
{
	// check the index
	if (!check_index(i, n, "get_node_value()"))
		return 0;

	return nodes[i].value;
}

unsigned Graph::get_node_flags(int i) const
// Returns the 'flags' field of the node at index 'i'
// (The indexing starts at 0)
{
	// check the index
	if (!check_index(i, n, "get_node_flags()"))
		return 0;

	return nodes[i].flags;
}


/*******************/
/* "Safe" Mutators */
/*******************/

void Graph::set_arc_weight(int i, int j, double weight)
// Sets the weight of the arc from node 'i' to node 'j' to 'weight'
// (The indexing starts at 0)
{
	// check the indices
	if (!check_index(i, n, "set_arc_weight() (start index)"))
		return;
	if (!check_index(j, n, "set_arc_weight() (end index)"))
		return;

	// check the weight
	if (weight <= 0) {
		cerr << "set_arc_weight(): negative weight " << weight << endl;
		weight = 1;
	}

	// in this implementation, the adjacency matrix elements store the
	// weight of the arcs directly
	adj[i][j] = weight;
}

void Graph::set_all_arc_weights(double weight)
// Sets the weight of each arc in this graph to 'weight' (which
// defaults to 1) 
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (adj[i][j] > 0)
				adj[i][j] = weight;
}


bool Graph::remove_arc(int i, int j)
// Removes the arc joining node 'i' with node 'j'
// Returns true if this arc existed before the call, and false otherwise
{
	// check the indices
	if (!check_index(i, n, "remove_arc() (start index)"))
		return false;
	if (!check_index(j, n, "remove_arc() (end index)"))
		return false;

	if (adj[i][j] > 0) {
		adj[i][j] = 0;
		return true;
	}
	else {
		adj[i][j] = 0;
		return false;
	}
}

void Graph::remove_all_arcs()
// Removes all the arcs in this graph
{
	// Setting all the 'adj' cells to 0 sufficies
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			adj[i][j] = 0;
}

void Graph::remove_outgoing_arcs(int i)
// Removes all the outgoing arcs for node 'i'
{
	if (!check_index(i, n, "remove_outgoing_arcs()"))
		return;
	// this amounts to setting row 'i' in the adjacency matrix to all zeros
	for (int j = 0; j < n; j++)
		adj[i][j] = 0;
}

void Graph::remove_incoming_arcs(int j)
// Removes all the incoming arcs for node 'j'
{
	if (!check_index(j, n, "remove_incoming_arcs()"))
		return;
	// this amounts to setting column 'j' in the adjacency matrix to all zeros
	for (int i = 0; i < n; i++)
		adj[i][j] = 0;
}

void Graph::unweight_arcs()
// Sets the weight of each arc in this graph to 'weight' (which
// defaults to 1) and sets the 'weighted' flag to false
{
	set_all_arc_weights(1);
	weighted = false;
}


bool Graph::add_arc(int i, int j)
// Adds the arc joining node 'i' with node 'j' having weight 1
// Returns true if this arc existed before the call, and false otherwise
{
	// check the indices
	if (!check_index(i, n, "add_arc() (start index)"))
		return false;
	if (!check_index(j, n, "add_arc() (end index)"))
		return false;

	if (adj[i][j] > 0) {
		adj[i][j] = 1;
		return true;
	}
	else {
		adj[i][j] = 1;
		return false;
	}
}

bool Graph::add_arc(int i, int j, double weight)
// Adds the arc joining node 'i' with node 'j' having weight 'weight'
// Returns true if this arc existed before the call, and false otherwise
{
	// check the indices
	if (!check_index(i, n, "add_arc() (start index)"))
		return false;
	if (!check_index(j, n, "add_arc() (end index)"))
		return false;

	// check the weight
	if (weight <= 0) {
		cerr << "set_arc_weight(): negative weight " << weight << endl;
		weight = 1;
	}

	if (adj[i][j] > 0) {
		adj[i][j] = weight;
		return true;
	}
	else {
		adj[i][j] = weight;
		return false;
	}
}


/* State Manipulation */

void Graph::set_node_state(int i, int state)
// Sets the state of node 'i' to 'state'
{
	// check the index
	if (!check_index(i, n, "set_node_state()"))
		return;

	// there are no restrictions on the state
	nodes[i].state = state;
}

void Graph::set_all_node_states(int state)
// Sets all the node states to 'state'
{
	for (int i = 0; i < n; i++)
		nodes[i].state = state;
}

void Graph::flag_node(int i, unsigned flags)
// Adds the bitfield flags in 'flags' to the flags of node 'i'
// That is, performs a bitwise "or" of the 'flags' field of this
// node with the value in 'flags'
{
	// check the index
	if (!check_index(i, n, "flag_node()"))
		return;

	nodes[i].flags |= flags;
}

void Graph::unflag_node(int i, unsigned flags)
// Removes the bitfield flags in 'flags' to the flags of node 'i'
// That is, performs a bitwise "and" of the 'flags' field of this
// node with the bitwise complement of 'flags'
{
	// check the index
	if (!check_index(i, n, "unflag_node()"))
		return;

	nodes[i].flags &= ~flags;
}

void Graph::set_all_node_flags(unsigned flags)
// Sets the flags of all the nodes to 'flags'
{
	for (int i = 0; i < n; i++)
		nodes[i].flags = flags;
}


/* Node Values */

void Graph::set_node_value(int i, double value)
// Sets the value of node 'i' to 'value'
{
	// check the index
	if (!check_index(i, n, "set_node_value()"))
		return;

	// there are no restrictions on the value
	nodes[i].value = value;
}

void Graph::set_all_node_values(double value)
// Sets all the node values to 'value'
{
	for (int i = 0; i < n; i++)
		nodes[i].value = value;
}

/******************/
/* Subgraph Stuff */
/******************/

Graph *Graph::node_subgraph() const
// Returns a newly created subgraph of this graph that contains
// only the nodes (i.e., with all arcs removed).  
{
	// Start by making a copy of 'this'
	Graph *subgraph = new Graph(*this);

	// remove all the arcs 
	subgraph->remove_all_arcs();

	// initially make this unweighted
	subgraph->weighted = false;

	return subgraph;
}


/*************/
/* PDF Stuff */
/*************/

#ifdef GRAPHICAL

void Graph::init_PDF(const string& filename)
// Initializes the associated PDF (actually, 'PDFGraph') object
// preparing to write to the 'filename'
{
	pdf = new PDFGraph(filename.c_str(), this);
}

void Graph::draw(unsigned flags, const string& annotation,
	Graph *beneath)
	// PRE: the PDF object of this graph is active
	// Draws this graph to the PDF object, starting on a new page
{
	if (pdf) {
		pdf->new_page(annotation.c_str());
		if (beneath)
			pdf->draw_beneath(flags, beneath);
		pdf->draw(flags);
	}
}

void Graph::finish_PDF()
// PRE: the PDF object of this graph is active
// Completes the PDF object and closes the output file
// NOTE: This function MUST be called for proper output.
{
	if (pdf) {
		pdf->finish();
		// delete 'pdf'
		delete pdf;
		pdf = NULL;
	}
}

#endif


/****************************************/
/* Node Visiting, with Graphical Output */
/****************************************/

void Graph::visit_node(int i, const string& annot, const Graph *beneath)
// Graphically displays the act "Visiting" node 'i'.  This amounts to
// displaying the graph on a new page beginning with 'annot' and 
//
//   - if 'beneath' is not null, the arcs (edges) are drawn in thicker
//     blue without any nodes.
//   - nodes are shaded according to their 'state' value
//     ('Finished' nodes are shaded in dark gray, 'Active' nodes in
//     light gray, and 'Visited' nodes in a medium gray.)
//   - node 'i' is highlighted
//
// The idea of 'beneath' is that it can contain a partial spanning
// tree or other subgraph.
{
	// check the index
	if (!check_index(i, n, "visit_node()"))
		return;

	// Display the graph on a new page,
	// starting by adding the node index to the annotation
	// (There is probably a more C++ way to do this.  Apparently C++-11
	// has a 'std::to_string' method, but it's not available at the moment.)
	char buf[256];
	sprintf(buf, "%d", i + 1);
	string annot_full = annot + ": visiting node " + string(buf);

	pdf->new_page(annot_full.c_str());

	// add the annotation as a comment
	sprintf(buf, "! Visiting node %d '%s'", i, annot.c_str());
	pdf->comment(buf);

	// highlight the node, temporarily
	unsigned flags0 = nodes[i].flags;
	nodes[i].flags |= HighlightFlag;

	// Draw the "beneath" graph, if it is specified
	if (beneath) {
		// Write the "beneath" graph in text output as a comment
		ostringstream output;
		output << "! Beneath graph:" << endl;
		write(output, true);
		pdf->comment(output.str().c_str());

		// draw the "beneath" graph
		pdf->draw_beneath(0, beneath);
	}

	// Write this graph in text output as a comment
	ostringstream output;
	output << "! Graph:" << endl;
	write(output, true);
	pdf->comment(output.str().c_str());

	// Draw this graph
	pdf->draw();

	// revert the flag of node 'i'
	nodes[i].flags = flags0;

}
