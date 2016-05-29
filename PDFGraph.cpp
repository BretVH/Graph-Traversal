/****************************************************************************/
/** 																       **/
/** PDFGraph.cpp - PDF subclass to display graphs.						   **/
/** 																	   **/
/** Copyright (C) 2014 Michael M. Stark, ALL RIGHTS RESERVED			   **/
/** 																	   **/
/**------------------------------------------------------------------------**/
/** Author:  Michael M. Stark											   **/
/****************************************************************************/

#include <stdlib.h>

#include "PDFGraph.h"
#include "Graph.h"

/****************************************************************************/
/***                       PDFGraph Implementation						  ***/
/****************************************************************************/

/****************/
/* Static Stuff */
/****************/

PDFColor PDFGraph::NodeColor = PDFColor::Black;
PDFColor PDFGraph::ArcColor = PDFColor::Black;
const double PDFGraph::ArrowheadLength = 9;
const double PDFGraph::ArrowheadWidth = 6;
const double PDFGraph::ArcLineWidth = 0.5;
const double PDFGraph::ThickArcLineWidth = 5;
const double PDFGraph::NodeLineWidth = 0.5;
const double PDFGraph::NodeRadius = 12;

const double PDFGraph::NodeFontScale = 12;
const double PDFGraph::ArcFontScale = 10;


/*******************************/
/* Construction/Initialization */
/*******************************/

static void update_bbox(double& x0, double& y0, double& x1, double& y1,
	double x, double y)
{
	x0 = (x < x0 ? x : x0);
	y0 = (y < y0 ? y : y0);

	x1 = (x > x1 ? x : x1);
	y1 = (y > y1 ? y : y1);
}

void PDFGraph::setup()
{
	// for convenience, copy 'n' from the graph
	int n = graph->n;

	// initialize the display flags
	display_flags = 0;

	// compute the bounding box of the nodes
	if (n == 0) {
		x0 = 0; y0 = 0;
		x1 = 0; y1 = 0;
	}
	else {
		x0 = x1 = graph->node_pos[0].x;
		y0 = y1 = graph->node_pos[0].y;
		for (int k = 1; k < n; k++)
			update_bbox(x0, y0, x1, y1, graph->node_pos[k].x, graph->node_pos[k].y);
	}

	// add the arc points (if there are any) to the bounding box
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (graph->arc_pos[i][j]) {
				update_bbox(x0, y0, x1, y1,
					graph->arc_pos[i][j]->x, graph->arc_pos[i][j]->y);
			}
		}
	}

	// add a margin to the bounding box (10%)
	double box_margin = (x1 - x0)*0.1;
	x0 -= box_margin;
	y0 -= box_margin;
	x1 += box_margin;
	y1 += box_margin;

	// setup the transformation
	//double margin = 72;
	//double s = (width - 2*margin)/(x1 - x0);
	double s = graph->scale;

	//b11 = s;  b12 = 0;  b13 = margin - x0;
	b11 = s;  b12 = 0;  b13 = width / 2 - s*(x1 + x0) / 2;
	b21 = 0;  b22 = s;  b23 = height / 2 - s*(y1 + y0) / 2;
}

void PDFGraph::draw_general(const Graph *src,
	unsigned flags,
	const PDFColor& node_color,
	const PDFColor& arc_color,
	double node_r, double node_line_width,
	double arc_line_width,
	double arrowhead_length, double arrowhead_width,
	int node_font, double node_font_scale,
	int arc_font, double arc_font_scale)
	// General draw function
{
	static char buf[256];  // for the text in the nodes

	// if 'src' is NULL, use the 'graph' of this
	if (src == NULL)
		src = graph;

	// for convenience, copy 'n' from the graph
	int n = src->n;

	// if thick arcs are requested, adjust the size of the arc line width
	// and arrowhead dimensions
	if (flags & ThickArcs) {
		// (actually, don't adjust the line width)
		arrowhead_length *= 1.414;
		arrowhead_width = arrowhead_length;
		arc_line_width = ThickArcLineWidth;
	}

	// start a new page, if so requested
	if (flags & NewPage)
		new_page();

	// draw all the nodes (unless requested not to)
	if (!(flags & NoNodes)) {
		setlinewidth(node_line_width);
		setcolor(node_color);
		for (int i = 0; i < n; i++) {
			PDFPoint p = gtransform(src->node_pos[i].x, src->node_pos[i].y);

			// highlight the node, if the Highlight flag is set
			if (src->nodes[i].flags & HighlightFlag) {
				setcolor(PDFColor(1.0, 1.0, 0.5));
				circle_path(p.x, p.y, 1.618*node_r);
				fill();
				setcolor(node_color);
			}

			// draw the node, as usual
			circle_path(p.x, p.y, node_r);
			// the fill color is set according to the state
			if (src->nodes[i].state == Active)
				setcolor_nonstroke(PDFColor(0.9));
			else if (src->nodes[i].state == Finished)
				setcolor_nonstroke(PDFColor(0.5));
			else if (src->nodes[i].state == Visited)
				setcolor_nonstroke(PDFColor(0.75));
			else
				setcolor_nonstroke(PDFColor(1));
			closepath_fill_stroke();

			// label the node, if so requested
			if (!(flags & NoNodeLabels)) {
				setcolor_nonstroke(node_color);
				if (flags & ShowNodeValues) {
					selectfont(Helvetica | BoldFlag, ArcFontScale);
					sprintf(buf, "%.2g", src->nodes[i].value);
					position_text(buf, p.x, p.y, 0.5, 0.5);
				}
				else {
					selectfont(Helvetica | BoldFlag, NodeFontScale);
					sprintf(buf, "%d", i + 1);
					position_text(buf, p.x, p.y, 0.5, 0.5);
				}
			}
		}
	}

	// draw all the arcs
	if (!(flags & NoArcs)) {
		int heads = (src->directed ? Forward : 0);
		setlinewidth(arc_line_width);
		setcolor(arc_color);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (src->adj[i][j] > 0) {
					PDFPoint p0 = gtransform(src->node_pos[i].x, src->node_pos[i].y);
					PDFPoint p1 = gtransform(src->node_pos[j].x, src->node_pos[j].y);
					// if no arc point is specified, just draw a line
					if (!src->arc_pos[i][j]) {
						arrowed_line(p0.x, p0.y, p1.x, p1.y,
							arrowhead_length, arrowhead_width, heads,
							node_r, node_r);
					}
					else {
						// otherwise construct a circular arc for the arc line
						PDFPoint p2 = gtransform(src->arc_pos[i][j]->x,
							src->arc_pos[i][j]->y);
						const double c1 = (p1.length_sqr() - p0.length_sqr()) / 2;
						const double c2 = (p2.length_sqr() - p1.length_sqr()) / 2;
						const PDFPoint d1 = p1 - p0;
						const PDFPoint d2 = p2 - p1;

						const double D = d1.x*d2.y - d1.y*d2.x;
						if (fabs(D) == 1E-6) {
							// it degenerates to a line
							arrowed_line(p0.x, p0.y, p1.x, p1.y,
								arrowhead_length, arrowhead_width, heads,
								node_r, node_r);
						}
						else {
							PDFPoint c((c1*d2.y - c2*d1.y) / D, -(c1*d2.x - c2*d1.x) / D);
							double r = c.dist(p0);
							double a0 = atan2(p0.y - c.y, p0.x - c.x);
							double a1 = atan2(p1.y - c.y, p1.x - c.x);
							if ((p0 - c).cross_z(p1 - c) < 0) {
								arrowed_arcn(c.x, c.y, r, a0, a1,
									arrowhead_length, arrowhead_width, heads,
									node_r, node_r);
							}
							else {
								arrowed_arc(c.x, c.y, r, a0, a1,
									arrowhead_length, arrowhead_width, heads,
									node_r, node_r);
							}
						}
					}
				}
			}
		}
	}

	// draw the arc weights, if so requested
	if (flags & ArcWeights) {
		selectfont(Helvetica, ArcFontScale);
		double label_offset = 3;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (src->adj[i][j] > 0) {
					PDFPoint mid;
					if (src->arc_pos[i][j])
						// if an arc point is given, start from there	    
						mid = *src->arc_pos[i][j];
					else
						// otherwise use the midpoint
						mid = 0.5*(src->node_pos[i] + src->node_pos[j]);
					mid = gtransform(mid.x, mid.y);

					// find the best placement based on the angle of the perpendicular
					PDFPoint perp =
						(src->node_pos[i] - src->node_pos[j]).perp().unit();
					double angle = atan2(perp.y, perp.x);
					double h_frac = 0;
					double v_frac = 0;
					if (angle < -3 * M_PI / 4) {
						double t = (angle + 5 * M_PI / 4) / (M_PI / 2);  // 0.5 <= t < 1
						h_frac = 1;
						v_frac = t;
					}
					else if (angle < -M_PI / 4) {
						double t = (angle + 3 * M_PI / 4) / (M_PI / 2);
						h_frac = 1 - t;
						v_frac = 1;
					}
					else if (angle < M_PI / 4) {
						double t = (angle + M_PI / 4) / (M_PI / 2);
						h_frac = 0;
						v_frac = 1 - t;
					}
					else if (angle < 3 * M_PI / 4) {
						double t = (angle - M_PI / 4) / (M_PI / 2);
						h_frac = t;
						v_frac = 0;
					}
					else {
						double t = (angle - 5 * M_PI / 4) / (M_PI / 2);
						h_frac = 1;
						v_frac = t;
					}

					// display the text
					mid = mid + label_offset*perp;
					sprintf(buf, "%.2g", graph->adj[i][j]);
					position_text(buf, mid.x, mid.y, h_frac, v_frac);
					//circle_path(mid.x, mid.y, 3); fill();
				}
			}
		}
	}

}

void PDFGraph::draw(unsigned flags, const Graph *src)
// Front-end function to the general 'draw' method
{
	src = (src == NULL ? graph : src);
	unsigned local_flags =
		(src->weighted ? ArcWeights : 0) |
		(src->directed ? 0 : NoArcArrows);

	draw_general(src, display_flags | flags | local_flags, NodeColor, ArcColor);
}

void PDFGraph::draw_beneath(unsigned flags, const Graph *src)
{
	src = (src == NULL ? graph : src);
	unsigned local_flags =
		(src->weighted ? ArcWeights : 0) |
		(src->directed ? 0 : NoArcArrows) |
		ThickArcs | NoNodes;

	draw_general(src, display_flags | flags | local_flags,
		NodeColor, PDFColor(0.5, 0.5, 1.0));
}
