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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "build_graph.h"

BuildGraph::BuildGraph(int w, int h)
{
	width = w;
	height = h;
	globalPb = MatrixXf::Zero(h, w);
	init_labels = MatrixXi::Constant(h, w, -1); //start from 0 to num_labels-1
	result_labels = MatrixXi::Constant(h, w, -1);
}

BuildGraph::~BuildGraph()
{
	delete gc;
}

bool BuildGraph::read_labels(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	if(inFile.is_open())
	{
		for(int i=0; i<height; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss(line);
			for(int j=0; j<width; ++j)
			{
				iss >> init_labels(i, j);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr<<"open label file error\n";
	return false;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  read_globalPb
 *  Description:  the smooth term
 * =====================================================================================
 */
bool BuildGraph::read_globalPb(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	if(inFile.is_open())
	{
		for(int i=0; i<height; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss(line);
			for(int j=0; j<width; ++j)
			{
				iss >> globalPb(i, j);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr<<"open globalPb file error\n";
	return false;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  build_graph
 *  Description:  build graph and solve
 * =====================================================================================
 */
bool BuildGraph::build_graph(std::string label_file, std::string gpb_file)
{
	read_labels(label_file);
	read_globalPb(gpb_file);
	num_labels = init_labels.maxCoeff() + 1;
	gc = new GCoptimizationGridGraph(width, height, num_labels);
	////////////////////////////////////////////
	// fill data term
	for(int i=0; i<height; ++i)
		for(int j=0; j<width; ++j)
			for(int l=0; l<num_labels; ++l)
			{
				if(l == init_labels(i, j))
					gc->setDataCost(width*i+j, l, 0);
				else
					gc->setDataCost(width*i+j, l, 5);
			}
		
	//////////////////////////////////////////
	// file smooth term 
	int * v = new int[num_labels*num_labels];
	for(int i=0; i<num_labels; ++i)
		for(int j=0; j<num_labels; ++j)
		{
			if(i==j)
				v[i*num_labels+j] = 0;
			else
				v[i*num_labels+j] = 1;
		}

	int *vCosts = new int[width*height];
	int *hCosts = new int[width*height];
	for(int i=0; i<height; ++i)
		for(int j=0; j<width; ++j)
		{
			if(i < height-1)
			{
				vCosts[i+j*height] = int( 10 * std::exp(-1*std::max(globalPb(i, j), globalPb(i+1, j))) );
			}
			else
			{
				vCosts[i+j*height] = 0;
			}
			if(j < width-1)
			{
				hCosts[i*width+j] = int( 10 * std::exp(-1*std::max(globalPb(i, j), globalPb(i, j+1))) );
			}
			else
			{
				hCosts[i*width+j] = 0;
			}
		}

	delete v;
	delete vCosts;
	delete hCosts;
	return true;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  solve
 *  Description:  solve graph problem max flow/min cut
 * =====================================================================================
 */
void BuildGraph::solve()
{
	gc->expansion(2);
	for(int i=0; i<height; ++i)
		for(int j=0; j<width; ++j)
		{
			result_labels(i, j) = gc->whatLabel(width*i + j);
		}
}		/* -----  end of function solve  ----- */

