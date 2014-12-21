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
#include "build_graph.h"

int main(int argc, char** argv)
{
	if(argc != 4)
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

	std::cout<<"width is "<<width<<", height is "<<height<<std::endl;
	BuildGraph label_graph(width, height);
	label_graph.build_graph(argv[2], argv[3]);
	label_graph.solve();
	label_graph.save_result("label.txt");

	/////////////  checking  /////////////////
	/*
	std::ofstream outFile("init_labels.txt");
	if(outFile.is_open())
	{
		outFile << label_graph.init_labels;
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
