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
	globalPb = MatrixXd::Zero(h, w);
	init_labels = MatrixXi::Constant(h, w, -1); //start from 0 to num_labels-1
	result_labels = MatrixXi::Constant(h, w, -1);
	vCosts = new double[width*height];
	hCosts = new double[width*height];
}

BuildGraph::~BuildGraph()
{
	delete gc;
	delete []smooth;
	delete []vCosts;
	delete []hCosts;
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
 *  Description:  build graph for solve
 * =====================================================================================
 */
bool BuildGraph::build_graph(std::string label_file, std::string gpb_file)
{
	if(!read_labels(label_file))
	{
		std::cerr<<"read label file failure\n";
		exit(-1);
	}
	if(!read_globalPb(gpb_file))
	{
		std::cerr<<"read globalPb file failure\n";
		exit(-1);
	}

	num_labels = init_labels.maxCoeff() + 1;
	smooth = new double[num_labels * num_labels];
	try{
	gc = new GCoptimizationGridGraph(width, height, num_labels);
	////////////////////////////////////////////
	// fill data term
	for(int i=0; i<height; ++i)
		for(int j=0; j<width; ++j)
		{
			for(int l=0; l<num_labels; ++l)
			{
				if(init_labels(i, j) == -1) //handle uncertain labels
				{
					gc->setDataCost(width*i+j, l, 0.2);
				}
				else if(l == init_labels(i, j))
					gc->setDataCost(width*i+j, l, 0.0);
				else
					gc->setDataCost(width*i+j, l, 0.5);
			}
		}
		
	//////////////////////////////////////////
	// file smooth term 
	for(int i=0; i<num_labels; ++i)
		for(int j=0; j<num_labels; ++j)
		{
			/*
			if(i==j)
				smooth[i*num_labels+j] = 0.0;
			else
				smooth[i*num_labels+j] = 1.0;
			*/
			smooth[i*num_labels+j] = (i==j) ? 0.0 : 1.0;
		}

	for(int i=0; i<height; ++i)
		for(int j=0; j<width; ++j)
		{
			if(i < height-1)
			{
				vCosts[i+j*height] = std::exp(-1*std::max(globalPb(i, j), globalPb(i+1, j)));
			}
			else
			{
				vCosts[i+j*height] = 0.0;
			}
			if(j < width-1)
			{
				hCosts[i*width+j] = std::exp(-1*std::max(globalPb(i, j), globalPb(i, j+1)));
			}
			else
			{
				hCosts[i*width+j] = 0.0;
			}
		}
//////////////////// checking /////////////////////////////
/*  
	std::ofstream outVcost("vcost.txt");
	std::ofstream outHcost("hcost.txt");
	for(int i=0; i<height; ++i)
	{
		for(int j=0; j<width; ++j)
		{
			outVcost << vCosts[i*width + j]<<" ";
			outHcost << hCosts[i*width + j]<<" ";
		}
		outVcost<<"\n";
		outHcost<<"\n";
	}
	outVcost.close();
	outHcost.close();
*/
/////////////////////////////////////////////////////////////
	gc->setSmoothCostVH(smooth, vCosts, hCosts);
	}
	catch(GCException e)
	{
		e.Report();
	}

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
	try{
	std::cout<<"Before Optimization energy is "<< gc->compute_energy()<<std::endl;
	std::cout<<"data energy is "<<gc->giveDataEnergy()<<"\nsmooth energy is "<<gc->giveSmoothEnergy()<<std::endl;
	gc->expansion(10);
	std::cout<<"After Optimization energy is "<< gc->compute_energy()<<std::endl;
	std::cout<<"data energy is "<<gc->giveDataEnergy()<<"\nsmooth energy is "<<gc->giveSmoothEnergy()<<std::endl;
	for(int i=0; i<height; ++i)
		for(int j=0; j<width; ++j)
		{
			result_labels(i, j) = gc->whatLabel(width*i + j);
		}
	}
	catch(GCException e)
	{
		e.Report();
	}
}		/* -----  end of function solve  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  save_result
 *  Description: save the final labels 
 * =====================================================================================
 */
void BuildGraph::save_result(std::string filename)
{
	std::ofstream outFile(filename.c_str());
	if(outFile.is_open())
	{
		outFile << result_labels;
	}
	else
	{
		std::cerr<<"open file for writting result error\n";
		exit(-1);
	}
}		/* -----  end of function save_result  ----- */

