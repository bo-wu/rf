/*
 * =====================================================================================
 *
 *       Filename:  build_graph.h  Version:  1.0  Created:  12/20/2014 03:25:09 PM
 *
 *    Description:  build graph for optimation
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2014, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */
#ifndef BUILD_GRAPH_H_
#define BUILD_GRAPH_H_
#include "gco/GCoptimization.h"
#include "types.h"

struct BuildGraph
{
	BuildGraph(int w, int h);
	~BuildGraph();
	bool read_labels();
	bool read_globalPb();

	GCoptimizationGridGraph *gc;
	int width, height, num_pixels, num_labels;
	MatrixXf globalPb, labels;
};

#endif