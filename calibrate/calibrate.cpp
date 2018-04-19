// calibrate.cpp : 定义控制台应用程序的入口点。
//

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

//#include "stdafx.h"
//#include <iostream>
//#include <iomanip>
//#include <vector>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/features2d/features2d.hpp>
//
//#include "CameraCalibrator.h"
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//
//	cv::namedWindow("Image");
//	cv::Mat image;
//	std::vector<std::string> filelist;
//
//	// generate list of chessboard image filename
//	for (int i=1; i<=20; i++) {
//
//		std::stringstream str;
//		str << "chessboards/chessboard" << std::setw(2) << std::setfill('0') << i << ".jpg";
//		std::cout << str.str() << std::endl;
//
//		filelist.push_back(str.str());
//		image= cv::imread(str.str(),0);
//		cv::imshow("Image",image);
//	
//		 cv::waitKey(100);
//	}
//
//	// Create calibrator object
//    CameraCalibrator cameraCalibrator;
//	// add the corners from the chessboard
//	cv::Size boardSize(6,4);
//	cameraCalibrator.addChessboardPoints(
//		filelist,	// filenames of chessboard image
//		boardSize);	// size of chessboard
//		// calibrate the camera
//    //	cameraCalibrator.setCalibrationFlag(true,true);
//	cameraCalibrator.calibrate(image.size());
//
//    // Image Undistortion
//    image = cv::imread(filelist[6]);
//	cv::Mat uImage= cameraCalibrator.remap(image);
//
//	// display camera matrix
//	cv::Mat cameraMatrix= cameraCalibrator.getCameraMatrix();
//	std::cout << " Camera intrinsic: " << cameraMatrix.rows << "x" << cameraMatrix.cols << std::endl;
//	std::cout << cameraMatrix.at<double>(0,0) << " " << cameraMatrix.at<double>(0,1) << " " << cameraMatrix.at<double>(0,2) << std::endl;
//	std::cout << cameraMatrix.at<double>(1,0) << " " << cameraMatrix.at<double>(1,1) << " " << cameraMatrix.at<double>(1,2) << std::endl;
//	std::cout << cameraMatrix.at<double>(2,0) << " " << cameraMatrix.at<double>(2,1) << " " << cameraMatrix.at<double>(2,2) << std::endl;
//
//    imshow("Original Image", image);
//    imshow("Undistorted Image", uImage);
//
//	//打印相机内参数矩阵（3*3矩阵）
//	std::cout << " 相机内参数矩阵：" << cameraMatrix.rows << "x" << cameraMatrix.cols << std::endl;
//	for (int i=0; i<cameraMatrix.rows; i++)
//		for (int j=0; j<cameraMatrix.cols; j++)
//		{
//			std::cout<<std::setw(10)<<cameraMatrix.at<double>(i, j);
//			if (j==2)
//				std::cout<<std::endl;
//		}
//
//	//打印畸变系数矩阵（1*5矩阵）
//   cv::Mat distCoeffs = cameraCalibrator.getDistCoeffs();
//    std::cout << "畸变系数矩阵：" << distCoeffs.rows << "x" << distCoeffs.cols << std::endl;
//    for (int i=0; i<distCoeffs.rows; i++)
//        for (int j=0; j<distCoeffs.cols; j++)
//			std::cout<<distCoeffs.at<double>(i, j)<<"\t";
//
//	cv::waitKey();
//	return 0;
//}
#include "stdafx.h"
#include "opencv2/core/core.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/calib3d/calib3d.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include <iostream>  
#include <fstream>  
#include <windows.h>   
using namespace cv;  
using namespace std;  

//LPWSTR 2 char
char* ConvertLPWSTRToLPSTR (LPWSTR lpwszStrIn)  
{  
    LPSTR pszOut = NULL;  
    if (lpwszStrIn != NULL)  
    {  
        int nInputStrLen = wcslen (lpwszStrIn);  
  
        // Double NULL Termination  
        int nOutputStrLen = WideCharToMultiByte (CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;  
        pszOut = new char [nOutputStrLen];  
  
        if (pszOut)  
        {  
            memset (pszOut, 0x00, nOutputStrLen);  
            WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);  
        }  
    }  
    return pszOut;  
}  

//程序执行绝对路径
void GetFilePath(char* dst, const char* name)
{
	TCHAR exeFullPath[256];
	GetModuleFileName(NULL,exeFullPath,256);
	char *a = ConvertLPWSTRToLPSTR(exeFullPath);
	char *p= strtok(a,"\\");
	       
	int i = 0;
	while(p!=NULL)
	{
		p = strtok(NULL,"\\");
		i++;	
    }

	a = ConvertLPWSTRToLPSTR(exeFullPath);
	p= strtok(a,"\\");
	for(int j=0;j<i-1;j++)
	{
		strcat(dst, p);
		strcat(dst, "\\");	
		p = strtok(NULL,"\\");
	}
	strcat(dst, name);	
}


//仿射变换
Mat AffineTrans(Mat src, Point2f* scrPoints, Point2f* dstPoints)
{
	Mat dst;
	Mat Trans = getAffineTransform(scrPoints, dstPoints);
	warpAffine(src, dst, Trans, Size(src.cols, src.rows), CV_INTER_CUBIC);
	return dst;
}

//透视变换
Mat PerspectiveTrans(Mat src, Point2f* scrPoints, Point2f* dstPoints)
{
	Mat dst;
	Mat Trans = getPerspectiveTransform(scrPoints, dstPoints);
	warpPerspective(src, dst, Trans, Size(src.cols, src.rows), CV_INTER_CUBIC);
	return dst;
}

  
int _tmain(int argc, _TCHAR* argv[])
{  
	char a0[256] = "";
	GetFilePath(a0, "calibdata.txt");
    ifstream fin(a0); /* 标定所用图像文件的路径 320*240*/  
	char a1[256] = "";
	GetFilePath(a1, "caliberation_result.txt");
    ofstream fout(a1);  /* 保存标定结果的文件 */    
    //读取每一幅图像，从中提取出角点，然后对角点进行亚像素精确化   
    cout<<"开始提取角点………………";  
    int image_count=0;  /* 图像数量 */  
    Size image_size;  /* 图像的尺寸 */  
    Size board_size = Size(6,4);    /* 标定板上每行、列的角点数 */  
    vector<Point2f> image_points_buf;  /* 缓存每幅图像上检测到的角点 */  
    vector<vector<Point2f>> image_points_seq; /* 保存检测到的所有角点 */  
    string filename;  
    int count= -1 ;//用于存储角点个数。  
    while (getline(fin,filename))  
    {  
        image_count++;        
        // 用于观察检验输出  
        cout<<"image_count = "<<image_count<<endl;          
        /* 输出检验*/  
        cout<<"-->count = "<<count;   
		char a2[256] = "";
		GetFilePath(a2, filename.c_str());
        Mat imageInput=imread(a2);  
        if (image_count == 1)  //读入第一张图片时获取图像宽高信息  
        {  
            image_size.width = imageInput.cols;  
            image_size.height =imageInput.rows;           
            cout<<"image_size.width = "<<image_size.width<<endl;  
            cout<<"image_size.height = "<<image_size.height<<endl;  
        }  
  
        /* 提取角点 */  
        if (0 == findChessboardCorners(imageInput,board_size,image_points_buf))  
        {             
            cout<<"can not find chessboard corners!\n"; //找不到角点  
            exit(1);  
        }   
        else   
        {  
            Mat view_gray;  
            cvtColor(imageInput,view_gray,CV_RGB2GRAY);  
            /* 亚像素精确化 */  
            //find4QuadCornerSubpix(view_gray,image_points_buf,Size(5,5)); //对粗提取的角点进行精确化  
            cornerSubPix(view_gray,image_points_buf,Size(5,5),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));  
            image_points_seq.push_back(image_points_buf);  //保存亚像素角点  
            /* 在图像上显示角点位置 */  
            drawChessboardCorners(view_gray,board_size,image_points_buf,false); //用于在图片中标记角点  
            imshow("Camera Calibration",view_gray);//显示图片   

            waitKey(500);//暂停0.5S         
        }  
    }  
    //int total = image_points_seq.size();  
    //cout<<"total = "<<total<<endl;  
    //int CornerNum=board_size.width*board_size.height;  //每张图片上总的角点数  
    //for (int ii=0 ; ii<total ;ii++)  
    //{  
    //    if (0 == ii%CornerNum)// 24 是每幅图片的角点个数。此判断语句是为了输出 图片号，便于控制台观看   
    //    {     
    //        int i = -1;  
    //        i = ii/CornerNum;  
    //        int j=i+1;  
    //        cout<<"--> 第 "<<j <<"图片的数据 --> : "<<endl;  
    //    }  
    //    if (0 == ii%3)  // 此判断语句，格式化输出，便于控制台查看  
    //    {  
    //        cout<<endl;  
    //    }  
    //    else  
    //    {  
    //        cout.width(10);  
    //    }  
    //    //输出所有的角点  
    //    cout<<" -->"<<image_points_seq[ii][0].x;  
    //    cout<<" -->"<<image_points_seq[ii][0].y;  
    //}     
    cout<<"角点提取完成！\n";  
  
    //以下是摄像机标定  
    cout<<"开始标定………………";  
    /*棋盘三维信息*/  
    Size square_size = Size(10,10);  /* 实际测量得到的标定板上每个棋盘格的大小 */  
    vector<vector<Point3f>> object_points; /* 保存标定板上角点的三维坐标 */  
    /*内外参数*/  
    Mat cameraMatrix=Mat(3,3,CV_32FC1,Scalar::all(0)); /* 摄像机内参数矩阵 */  
    vector<int> point_counts;  // 每幅图像中角点的数量  
    Mat distCoeffs=Mat(1,5,CV_32FC1,Scalar::all(0)); /* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */  
    vector<Mat> tvecsMat;  /* 每幅图像的旋转向量 */  
    vector<Mat> rvecsMat; /* 每幅图像的平移向量 */  
    /* 初始化标定板上角点的三维坐标 */  
    int i,j,t;  
    for (t=0;t<image_count;t++)   
    {  
		int w = board_size.height;
        vector<Point3f> tempPointSet;  
        for (i=0;i<board_size.height;i++)   
        {  
            for (j=0;j<board_size.width;j++)   
            {  
                Point3f realPoint;  
                /* 假设标定板放在世界坐标系中z=0的平面上 */  
                realPoint.x = j*square_size.width;  
                realPoint.y = i*square_size.height;  
                realPoint.z = 0;  
                tempPointSet.push_back(realPoint);  
            }  
        }  
        object_points.push_back(tempPointSet);  
    }  
    /* 初始化每幅图像中的角点数量，假定每幅图像中都可以看到完整的标定板 */  
    for (i=0;i<image_count;i++)  
    {  
        point_counts.push_back(board_size.width*board_size.height);  
    }     
    /* 开始标定 */  
    calibrateCamera(object_points,image_points_seq,image_size,cameraMatrix,distCoeffs,rvecsMat,tvecsMat,0);  
    cout<<"标定完成！\n";  
    //对标定结果进行评价  
    cout<<"开始评价标定结果………………\n";  
    double total_err = 0.0; /* 所有图像的平均误差的总和 */  
    double err = 0.0; /* 每幅图像的平均误差 */  
    vector<Point2f> image_points2; /* 保存重新计算得到的投影点 */  
    cout<<"\t每幅图像的标定误差：\n";  
    fout<<"每幅图像的标定误差：\n";  
    for (i=0;i<image_count;i++)  
    {  
        vector<Point3f> tempPointSet=object_points[i];  
        /* 通过得到的摄像机内外参数，对空间的三维点进行重新投影计算，得到新的投影点 */  
        projectPoints(tempPointSet,rvecsMat[i],tvecsMat[i],cameraMatrix,distCoeffs,image_points2);  
        /* 计算新的投影点和旧的投影点之间的误差*/  
        vector<Point2f> tempImagePoint = image_points_seq[i];  
        Mat tempImagePointMat = Mat(1,tempImagePoint.size(),CV_32FC2);  
        Mat image_points2Mat = Mat(1,image_points2.size(), CV_32FC2);  
        for (int j = 0 ; j < tempImagePoint.size(); j++)  
        {  
            image_points2Mat.at<Vec2f>(0,j) = Vec2f(image_points2[j].x, image_points2[j].y);  
            tempImagePointMat.at<Vec2f>(0,j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);  
        }  
        err = norm(image_points2Mat, tempImagePointMat, NORM_L2);  
        total_err += err/=  point_counts[i];     
        std::cout<<"第"<<i+1<<"幅图像的平均误差："<<err<<"像素"<<endl;     
        fout<<"第"<<i+1<<"幅图像的平均误差："<<err<<"像素"<<endl;
    }     
    std::cout<<"总体平均误差："<<total_err/image_count<<"像素"<<endl;     
    fout<<"总体平均误差："<<total_err/image_count<<"像素"<<endl<<endl;     
    std::cout<<"评价完成！"<<endl;    
    //保存定标结果      
    std::cout<<"开始保存定标结果………………"<<endl;         
    Mat rotation_matrix = Mat(3,3,CV_32FC1, Scalar::all(0)); /* 保存每幅图像的旋转矩阵 */  
    fout<<"相机内参数矩阵："<<endl;     
    fout<<cameraMatrix<<endl<<endl;     
    fout<<"畸变系数：\n";     
    fout<<distCoeffs<<endl<<endl<<endl;     
    for (int i=0; i<image_count; i++)   
    {   
        fout<<"第"<<i+1<<"幅图像的旋转向量："<<endl;     
        fout<<tvecsMat[i]<<endl;      
        /* 将旋转向量转换为相对应的旋转矩阵 */     
        Rodrigues(tvecsMat[i],rotation_matrix);     
        fout<<"第"<<i+1<<"幅图像的旋转矩阵："<<endl;     
        fout<<rotation_matrix<<endl;     
        fout<<"第"<<i+1<<"幅图像的平移向量："<<endl;     
        fout<<rvecsMat[i]<<endl<<endl;     
    }     
    std::cout<<"完成保存"<<endl;   
    fout<<endl;  
    /************************************************************************   
    显示定标结果   
    *************************************************************************/  
    Mat mapx = Mat(image_size,CV_32FC1);  
    Mat mapy = Mat(image_size,CV_32FC1);  
    Mat R = Mat::eye(3,3,CV_32F);  
    //std::cout<<"保存矫正图像"<<endl;  
    string imageFileName;  
    std::stringstream StrStm;  

	ifstream fin1(a0);
	i = 0;
	initUndistortRectifyMap(cameraMatrix,distCoeffs,R,cameraMatrix,image_size,CV_32FC1,mapx,mapy);  
    while (getline(fin1,filename))  
    {  
        Mat imageSource = imread(filename);  
        Mat newimage = imageSource.clone();  
        //另一种不需要转换矩阵的方式  
        //undistort(imageSource,newimage,cameraMatrix,distCoeffs);  
        remap(imageSource,newimage,mapx, mapy, INTER_LINEAR);  
		imshow("PerspectiveTrans",newimage);//显示图片 			
    }  
   // std::cout<<"保存结束"<<endl;    

	waitKey(0);

    return 0;  
}  