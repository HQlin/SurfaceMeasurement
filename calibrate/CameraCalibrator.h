/*------------------------------------------------------------------------------------------*\
   This file contains material supporting chapter 9 of the cookbook:  
   Computer Vision Programming using the OpenCV Library. 
   by Robert Laganiere, Packt Publishing, 2011.

   This program is free software; permission is hereby granted to use, copy, modify, 
   and distribute this source code, or portions thereof, for any purpose, without fee, 
   subject to the restriction that the copyright notice may not be removed 
   or altered from any source or altered source distribution. 
   The software is released on an as-is basis and without any warranties of any kind. 
   In particular, the software is not guaranteed to be fault-tolerant or free from failure. 
   The author disclaims all warranties with regard to this software, any use, 
   and any consequent failure, is purely the responsibility of the user.
 
   Copyright (C) 2010-2011 Robert Laganiere, www.laganiere.name
\*------------------------------------------------------------------------------------------*/

#ifndef CAMERACALIBRATOR_H
#define CAMERACALIBRATOR_H

#include <vector>
#include <iostream>

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class CameraCalibrator {

	// input points输入点
    std::vector<std::vector<cv::Point3f>> objectPoints;			//世界坐标系下的点
    std::vector<std::vector<cv::Point2f>> imagePoints;			//像素坐标系下的点
    // output Matrices输出矩阵
    cv::Mat cameraMatrix;										//摄像机内参数矩阵
    cv::Mat distCoeffs;											//透镜畸变系数矩阵
	// flag to specify how calibration is done标定方式
	int flag;
	// used in image undistortion 用于图像去畸变
    cv::Mat map1,map2; 
	bool mustInitUndistort;

  public:
	//Output rotations and translations
    std::vector<cv::Mat> rvecs, tvecs;
	CameraCalibrator() : flag(0), mustInitUndistort(true) {};

	// Open the chessboard images and extract corner points导入标定图片提取角点
	int addChessboardPoints(const std::vector<std::string>& filelist, cv::Size & boardSize);
	// Add scene points and corresponding image points添加场景点与对应的图像点
    void addPoints(const std::vector<cv::Point2f>& imageCorners, const std::vector<cv::Point3f>& objectCorners);
	// Calibrate the camera标定相机
	double calibrate(cv::Size &imageSize);
    // Set the calibration flag设置标定方式
    void setCalibrationFlag(bool radial8CoeffEnabled=false, bool tangentialParamEnabled=false);
	// Remove distortion in an image (after calibration)消除透镜畸变(标定之后调用有效)
	cv::Mat CameraCalibrator::remap(const cv::Mat &image);

    // Getters获取矩阵
    cv::Mat getCameraMatrix() { return cameraMatrix; }
    cv::Mat getDistCoeffs()   { return distCoeffs; }
};

#endif // CAMERACALIBRATOR_H
