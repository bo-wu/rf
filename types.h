/*
 * =====================================================================================
 *
 *       Filename:  types.h  Version:  1.0  Created:  12/20/2014 03:44:12 PM
 *
 *    Description:  define some types for common use
 *
 *         Author:  Bo Wu (Robert), wubo.gfkd@gmail.com
 *	    Copyright:  Copyright (c) 2014, Bo Wu
 *   Organization:  National University of Defense Technology
 *
 * =====================================================================================
 */
#ifndef TYPES_H_
#define TYPES_H_
#include "Eigen/Dense"
typedef double Real;
typedef Eigen::MatrixXf MatrixXf;
typedef Eigen::MatrixXd MatrixXd;
typedef Eigen::MatrixXi MatrixXi;
typedef Eigen::VectorXd VectorXd;
typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> MatrixXr;
#endif
