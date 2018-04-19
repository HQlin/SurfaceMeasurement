
// Build2D_OpenCV.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CBuild2D_OpenCVApp:
// 有关此类的实现，请参阅 Build2D_OpenCV.cpp
//

class CBuild2D_OpenCVApp : public CWinApp
{
public:
	CBuild2D_OpenCVApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CBuild2D_OpenCVApp theApp;