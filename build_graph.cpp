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
#include <vector>
#include <algorithm>
#include "build_graph.h"

BuildGraph::BuildGraph(int w, int h, const cv::Mat& image)
{
	width = w;
	height = h;
	num_pixels = width*height;
	globalPb = MatrixXd::Zero(h, w);
	init_labels = MatrixXi::Constant(h, w, -1); //start from 0 to num_labels-1
	result_labels = MatrixXi::Constant(h, w, -1);
	color_mat = MatrixXd::Zero(h*w, 3);
	vCosts = new double[width*height];
	hCosts = new double[width*height];
	cv::Vec3f color;
	for(int i=0; i<h; ++i)
	{
		for(int j=0; j<w; ++j)
		{
			color = image.at<cv::Vec3b>(i, j);
			color_mat.row(i*w + j) << color.val[0], color.val[1], color.val[2];
		}
	}
	color_mat = color_mat / 255.0;
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

bool BuildGraph::read_label_weight(std::string filename)
{
	std::ifstream inFile(filename.c_str());
	std::string line;
	std::vector<Real> first_line;
	if(inFile.is_open())
	{
		// get the number of clusters
		std::getline(inFile, line);
		std::istringstream iss(line);
		Real temp;
		while(iss >> temp)
		{
			first_line.push_back(temp);
		}
		label_weight = MatrixXr::Zero(num_pixels, first_line.size());
		////////////////////////////
		///fill the first row
		for(int i=0; i<first_line.size(); ++i)
		{
			label_weight(0, i) = first_line.at(i);
		}
		///////////////////////////
		//fill the left data
		for(int i=1; i<num_pixels; ++i)
		{
			std::getline(inFile, line);
			std::istringstream iss2(line);
			for(int j=0; j<first_line.size(); ++j)
			{
				iss2 >> label_weight(i, j);
			}
		}
		inFile.close();
		return true;
	}
	std::cerr <<"open label weight file error\n";
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
bool BuildGraph::build_graph(std::string label_file, std::string weight_file, std::string gpb_file)
{
	if(!read_labels(label_file))
	{
		std::cerr<<"read label file "<<label_file<<" failure\n";
		exit(-1);
	}
	if(!read_label_weight(weight_file))
	{
		std::cerr<<"read label weight file "<<weight_file<<" failure\n";
		exit(-1);
	}
	if(!read_globalPb(gpb_file))
	{
		std::cerr<<"read globalPb file "<<gpb_file<<" failure\n";
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
					gc->setDataCost(width*i+j, l, 0.5*label_weight(width*i+j, l));
					/*  
					if(l == init_labels(i, j))
						gc->setDataCost(width*i+j, l, 0.0);
					else
						gc->setDataCost(width*i+j, l, 0.5);
					*/
				}
			}

		//////////////////////////////////////////
		// file smooth term 
		double sigma2 = 0.2; //squared sigma 
		double weight = 1.0 ; //    / std::sqrt(sigma2);
		Vector3d diff;
		std::cout<<"smooth term weight is "<<weight<<std::endl;
		for(int i=0; i<num_labels; ++i)
			for(int j=0; j<num_labels; ++j)
			{
				/*
				   if(i==j)
				   smooth[i*num_labels+j] = 0.0;
				   else
				   smooth[i*num_labels+j] = 1.0;
				   */
				smooth[i*num_labels+j] = (i==j) ? 0.0 : weight;
			}

		for(int i=0; i<height; ++i)
			for(int j=0; j<width; ++j)
			{
				if(i < height-1)
				{
					diff = color_mat.row(i*width + j) - color_mat.row((i+1)*width + j);
					vCosts[i*width+j] = 0.4 * std::exp(-0.5*std::max(globalPb(i, j), globalPb(i+1, j)) / sigma2) + (0.6) * std::exp(- 0.5*diff.squaredNorm() / sigma2);
					//vCosts[i*width+j] = std::exp(-1*std::max(globalPb(i, j), globalPb(i+1, j)) / sigma2 );
				}
				else
				{
					vCosts[i*width+j] = 0.0;
				}
				if(j < width-1)
				{
					diff = color_mat.row(i*width + j) - color_mat.row(i*width + (j+1));
					hCosts[i*width+j] = 0.4 * std::exp(-1*std::max(globalPb(i, j), globalPb(i, j+1)) / sigma2) + (0.6) * std::exp(- 0.5 * diff.squaredNorm() / sigma2);
					//hCosts[i*width+j] = std::exp(-1*std::max(globalPb(i, j), globalPb(i, j+1)) / sigma2 );
				}
				else
				{
					hCosts[i*width+j] = 0.0;
				}
			}

			gc->setSmoothCostVH(smooth, vCosts, hCosts);

		//////////////////// checking /////////////////////////////
			std::string base_name = label_file.substr(16, label_file.length()-21);
			std::string path = "data/cost/";
			std::string vcost_name = path + base_name + "vcost";
			std::string hcost_name = path + base_name + "hcost";
			std::string weight_name = path + base_name + "wei";
			std::ofstream outVcost(vcost_name.c_str());
			std::ofstream outHcost(hcost_name.c_str());
			std::ofstream outWeight(weight_name.c_str());
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
			for(int i=0; i<num_pixels; ++i)
			{
				for(int j=0; j<num_labels; ++j)
				{
					outWeight << label_weight(i, j)<<" ";
				}
				outWeight<<"\n";
			}
			outVcost.close();
			outHcost.close();
			outWeight.close();
			/*  
			*/
			/////////////////////////////////////////////////////////////
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
	//	std::cout<<"data energy is "<<gc->giveDataEnergy()<<"\nsmooth energy is "<<gc->giveSmoothEnergy()<<std::endl;

		gc->expansion(10);
		//gc->swap(10);

		std::cout<<"After Optimization energy is "<< gc->compute_energy()<<std::endl;
	//	std::cout<<"data energy is "<<gc->giveDataEnergy()<<"\nsmooth energy is "<<gc->giveSmoothEnergy()<<std::endl;
		std::cout<<std::endl;
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
	outFile.close();
}		/* -----  end of function save_result  ----- */

