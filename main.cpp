/*
 * =====================================================================================
 *
 *       Filename:  main.cpp  Version:  1.0  Created:  12/20/2014 03:23:18 PM
 *
 *    Description:  main 
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2014, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "build_graph.h"

int main(int argc, char** argv)
{
	if(argc != 8)
	{
		std::cerr<<"Not right parameter\n";
		return -1;
	}
	cv::Mat image;
	image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
	if(!image.data)
	{
		std::cout<<"Could not open or find the image\n";
		return -1;
	}
	int height = image.rows;
	int	width = image.cols;
	std::string path = "result/label/";
	std::string suffix = "label";
	std::string full_name = argv[1];
	std::string name = path + full_name.substr(12, full_name.length()-15) + suffix;
	std::cout <<"Image "<<argv[1]<<std::endl;
	std::cout<<"width is "<<width<<", height is "<<height<<std::endl;
	BuildGraph label_graph(width, height);

	//// param1@ init label
	//   param2@ label(data) weight
	//   param3@ smooth term weight
	//   param4@ float color value
	//   param5@ cluster centers
	label_graph.initial_data(argv[2], argv[3], argv[4], argv[5], argv[6], atoi(argv[7]));

/*
	label_graph.build_grid_graph();
	label_graph.solve_grid_graph();
*/	
	label_graph.build_general_graph();
	label_graph.solve_general_graph();
	/* 
	 */
	label_graph.save_result(name);

	/////////////  checking  /////////////////
	/*
	std::ofstream outFile("label_weight.txt");
	if(outFile.is_open())
	{
		outFile <<std::left<<std::setw(9)<<std::setfill('0')<<std::setprecision(8) << label_graph.label_weight;
	}
	outFile.close();
	outFile.open("gpb.txt");
	if(outFile.is_open())
	{
		outFile<< label_graph.globalPb;
	}
	outFile.close();
	*/
	return 0;
}

