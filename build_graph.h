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
#include <opencv2/core/core.hpp>

struct BuildGraph
{
	BuildGraph(int w, int h);
	~BuildGraph();
	bool read_labels(std::string filename);
	bool read_label_weight(std::string filename);
	bool read_label_center(std::string filename);
	bool read_float_color(std::string filename);
	bool read_globalPb(std::string filename);

	void initial_data(std::string label_file, std::string weight_file, std::string gpb_file, std::string colorf_file, std::string label_center_file, int sa);
	bool build_grid_graph();
	bool build_general_graph();
	void set_neighbors(); //only for general graph

	void solve_grid_graph();
	void solve_general_graph();
	void save_result(std::string filename);
	// regular grid graph
	GCoptimizationGridGraph *gc;
	GCoptimizationGeneralGraph * ggc; //general graph cut
	// general graph
	
	// data
	int width, height, num_pixels, num_labels;
	int symm_axis; // axis for symmetry
	int num_features; // num of features used for clustring
	MatrixXr globalPb, label_weight, color_mat, label_center; 
	MatrixXi init_labels, result_labels;
	double *smooth, *vCosts, *hCosts;
	std::string label_file_name;
	bool bGrid;
};

#endif
