#include "StdAfx.h"
#include "OpenCVCam.h"


COpenCVCam::COpenCVCam(void)
{
}


COpenCVCam::~COpenCVCam(void)
{
}

bool COpenCVCam::OpenCam(int index)
{
	bool result = capture.open(index);
	if(!result)
	{
		AfxMessageBox(L"打开相机失败");
	}
	return result;
}
	
Mat COpenCVCam::GetImage()
{
	Mat frame; //定义一个Mat变量，用于存储每一帧的图像  
	if(capture.isOpened())                
	{
		capture >> frame;  //读取当前帧   
	} 
	else
	{
		AfxMessageBox(L"未打开相机");
	}
	return frame;
}
	
bool COpenCVCam::CloseCam()
{
	capture.release();
	return true;
}