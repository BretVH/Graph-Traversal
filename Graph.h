/****************************************************************************/
/** 																	   **/
/** Graph.h - Class representing a directed/undirected graph			   **/
/** 																       **/
/** Copyright (C) 2014 Michael M. Stark, ALL RIGHTS RESERVED			   **/
/** 																	   **/
/**------------------------------------------------------------------------**/
/** Author:  Michael M. Stark											   **/
/****************************************************************************/

#ifndef __GRAPHS_H
#define __GRAPHS_H

#include <cstdlib>
#include <string>
#include <iostream>

// Removing this line will omit all the graphic stuff
#define GRAPHICAL

#ifdef GRAPHICAL
#include "PDF.h"
#endif

using namespace std;

class PDFGraph;

/**************************************************************************** 
 * 
 * STRUCT:  GraphNode
 * 
 ****************************************************************************/

// A lightweight structure for a node in a graph

struct GraphNode {

  string   name;    // name for the node
  int      index;   // index of the node in they order given (starts at 1)
  int      state;   // node state (for graph algorithms)
  double   value;   // value for the node, if there is one
  unsigned flags;   // flags (for highlighting, etc)

  GraphNode() { value = 0; index = 0; state = 0; value = 0; flags = 0; }
  GraphNode( const string& node_name, int node_index,
	     int node_state = 0, double val = 0, unsigned node_flags = 0 )
    : name(node_name), index(node_index), state(node_state), value(val),
      flags(node_flags)
    {}
};

// Constants for use as GraphNode 'state' values 
const int NoState = 0;
const int Active   = 1;
const int Visited  = 2;
const int Finished = 3;

// Constants for the GraphNode 'flag' field
const int HighlightFlag = 1<<0;


/**************************************************************************** 
 * 
 * CLASS:  Graph
 * 
 ****************************************************************************/

// Class representing a general directed or undirected graph

class Graph {
 public:

  /* Constructors */
  Graph() {}
  Graph( const string& filename );
  Graph( istream& in ) { read(in, "input"); }
  Graph( const Graph& source );
  ~Graph();                     /* Implement */

  /* The actual number of nodes, along with their index and names,
   * are fixed in an instance once the graph is initialized.
   * However, arcs can be added and removed, and the weights of arcs,
   * and the 'value' field of each node can be changed.
   */

  bool is_weighted() const { return weighted; } // true if arcs are weighted
  bool is_directed() const { return directed; } // true if this is directed
  
  /* Convenience Accessors (Safe) */
  bool adjacent( int i, int j ) const;         // true if there is an arc i->j
  double get_arc_weight( int i, int j ) const; // returns weight of arc i->j
  GraphNode get_node( int i ) const;           // returns node 'i'
  int get_node_state( int i ) const;           // returns the state of node 'i'
  double get_node_value( int i ) const;        // returns the value of node 'i'
  unsigned get_node_flags( int i ) const;      // returns the flags of node 'i'

  /* Mutators (Safe) */
  void set_arc_weight( int i, int j,
		       double weight = 1 );    // sets the weight of arc i->j
  void set_all_arc_weights( double weight = 1 ); // sets the weight of all arcs
  void unweight_arcs();                 // sets all arc weights to 1
  bool remove_arc( int i, int j );      // removes arc i->j
  void remove_outgoing_arcs( int i );   // removes arcs i->j for all nodes j
  void remove_incoming_arcs( int i );   // removes arcs j->i for all nodes j
  void remove_all_arcs();               // removes all arcs
  bool add_arc( int i, int j );         // adds an arc i->j
  bool add_arc( int i, int j, double weight );// adds an arc i->j with weight
  void set_node_state( int i, int state );    // sets the state of node 'i'
  void set_all_node_states( int state = 0 );  // sets all node states
  void flag_node( int i, unsigned flags );    // adds 'flags' to node 'i'
  void unflag_node( int i, unsigned flags );  // removes 'flags' from node 'i'
  void set_all_node_flags( unsigned flags );  // sets all the node flags
  void set_node_value( int i, double value );   // sets the value of node 'i'
  void set_all_node_values( double value = 0 ); // sets all node values

  void set_weighted()   { weighted = true; }  // makes the arcs weighted
  void set_unweighted() { weighted = false; } // makes the arcs unweighted
  void set_directed()   { directed = true; }  // makes this a direct graph
  void set_undirected() { directed = false; } // makes this an undirected graph
    
  /* General node visiting (mostly for graphical output) */
  void visit_node( int i, const string& annot = "",
		   const Graph* beneath = NULL );

  /* Subgraphs */
  Graph *node_subgraph() const;
  
  /* Graph Algorithms (implemented in "GraphAlg.cpp") */
  Graph* depth_first( int start_i );
  Graph* breadth_first( int start_i );
  Graph *shortest_paths( int source_i );
  void shortest_path( int source_i, int dset_i, ostream *out );
 
  int minDistance();
  /* Text output */
  ostream& write( ostream& out, bool brief = false,
		  const string &prefix = "" );
  
#ifdef GRAPHICAL
  void init_PDF( const string& filename );
  void draw( unsigned flags = 0, const string& annotation = "",
	     Graph *beneath = NULL );
  void finish_PDF();
#endif
  
 private:

  // The nodes are stored in an array of 'GraphNode' objects
  // NOTE: The indexing of the nodes in the 'nodes' array starts at 0,
  //       but the indexing in the input file starts at 1.  So normally
  //       'nodes[k-1]' corresponds to the node of index 'k' in the input
  //       file as well as in the actual 'GraphNode' instance in 'nodes[k-1]'
  //       Put another way, 'nodes[k].index == k + 1'
  int n;     // number of nodes
  GraphNode *nodes; // array of nodes (always size 'n')

  // The arcs are represented in an adjacency matrix
  // (see the Graph.cpp file for more information)
  double **adj;

  // The 'weighted' flag indicates that the arcs are specifically
  // weighted, even if all the values in the adjacency matrix are 1.
  bool weighted;

  // The 'directed' flag indicates this is a directed graph
  // (this is the default)
  bool directed;
  
  // Initialization and file input
  void init( int n_nodes );
  void read( istream& in, const string& sourcename );

  // Traversal "helper" functions
  void depth_first( int i, Graph *spanning_tree );
  
#ifdef GRAPHICAL
  // Graphical stuff
  
  double scale;  
  PDFPoint *node_pos;
  PDFPoint ***arc_pos;

  friend class PDFGraph;
  PDFGraph *pdf;
#endif
};  /* End of class 'Graph' */


#endif
