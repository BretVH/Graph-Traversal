/****************************************************************************/
/** 																	   **/
/** PDFGraph.h - PDF subclass to display graphs.						   **/
/** 																	   **/
/** Copyright (C) 2014 Michael M. Stark, ALL RIGHTS RESERVED			   **/
/** 																	   **/
/**------------------------------------------------------------------------**/
/** Author:  Michael M. Stark											   **/
/****************************************************************************/

#ifndef __PDFGRAPH_H
#define __PDFGRAPH_H

#include "PDF.h"

class Graph;

/****************************************************************************
 *
 * CLASS:  PDFGraph
 *
 ****************************************************************************/

class PDFGraph : public PDF {
public:

	// basic drawing parameters
	static const double ArrowheadLength; //= 9;
	static const double ArrowheadWidth; //= 6;
	static const double ArcLineWidth; //= 0.5;
	static const double ThickArcLineWidth; //= 5;
	static const double NodeLineWidth; //= 0.5;
	static const double NodeRadius; //= 12;

	static const int    NodeFont = Helvetica | BoldFlag;
	static const double NodeFontScale; //= 12;
	static const int    ArcFont = Helvetica;
	static const double ArcFontScale; //= 10;

	static PDFColor NodeColor;
	static PDFColor ArcColor;

	// flags for the general drawing function
	static const unsigned NewPage = 1 << 0;
	static const unsigned NoNodes = 1 << 1;
	static const unsigned NoNodeLabels = 1 << 2;
	static const unsigned ShowNodeValues = 1 << 3;
	static const unsigned ShowNodeNames = 1 << 4;
	static const unsigned NoArcs = 1 << 5;
	static const unsigned ArcWeights = 1 << 6;
	static const unsigned ThickArcs = 1 << 7;
	static const unsigned NoArcArrows = 1 << 8;

	/* Constructor */
	PDFGraph(const char* filename, const Graph* g,
		double width = LetterWidth, double height = LetterHeight)
		: PDF(filename, width, height) {
		graph = g;
		setup();	
	}

	// Graph to device (PDF) coordinate transformation
	PDFPoint gtransform(double x, double y) {
		return PDFPoint(b11*x + b12*y + b13, b21*x + b22*y + b23);
	}
	PDFPoint gtransform_vector(double x, double y) {
		return PDFPoint(b11*x + b12*y, b21*x + b22*y);
	}

	// Drawing
	void set_display_flags(unsigned flags) {
		display_flags = flags;
	}
	void merge_display_flags(unsigned flags) {
		display_flags |= flags;
	}
	void reset_display_flags() {
		display_flags = 0;
	}

	void draw_general(const Graph *src,
		unsigned flags,
		const PDFColor& node_color,
		const PDFColor& arc_color,
		double node_r = NodeRadius,
		double node_line_width = NodeLineWidth,
		double arc_line_width = ArcLineWidth,
		double arrowhead_length = ArrowheadLength,
		double arrowhead_width = ArrowheadWidth,
		int node_font = NodeFont,
		double node_font_scale = NodeFontScale,
		int arc_font = ArcFont,
		double arc_font_scale = ArcFontScale);
	void draw(unsigned flags = 0, const Graph* src = NULL);
	void draw_beneath(unsigned flags, const Graph *src = NULL);

	// Convenience functions for setting the drawing state
	void show_node_values() { display_flags |= ShowNodeValues; }
	void show_node_names() { display_flags |= ShowNodeNames; }
	void show_node_indices() {
		display_flags &= ~(ShowNodeValues | ShowNodeNames);
	}

private:

	// pointer to the graph object 
	const Graph* graph;

	// display flags
	unsigned display_flags;

	// bounding box, in graph canvas coordinates
	double x0, y0;
	double x1, y1;

	// transformation matrix
	// (this is separate from the transformation matrix in 'PDF')
	double b11, b12, b13;
	double b21, b22, b23;

	// Initialization stuff
	void setup();

};


#endif
