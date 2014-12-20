/*
 * =====================================================================================
 *
 *       Filename:  build_graph.cpp  Version:  1.0  Created:  12/20/2014 03:25:31 PM
 *
 *    Description:  build graph for optimation
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2014, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */

#include "build_graph.h"
BuildGraph::BuildGraph(int w, int h)
{
	width = w;
	height = h;
	globalPb = MatrixXf::Zero(h, w);
	labels = MatrixXf::Zero(h, w);
}

BuildGraph::~BuildGraph()
{
	delete gc;
}
