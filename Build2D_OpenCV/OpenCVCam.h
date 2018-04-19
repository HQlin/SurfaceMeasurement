#pragma once
#include<opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  

using namespace cv;

class COpenCVCam
{
public:
	COpenCVCam(void);
	~COpenCVCam(void);

	bool OpenCam(int index);
	Mat GetImage();
	bool CloseCam();

	VideoCapture capture;
};

