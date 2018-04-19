
// Build2D_OpenCVDlg.h : 头文件
//

#pragma once
#include "comaccess.h"
#include <iostream>  
#include<opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  
#include "OpenCVCam.h"
#include "Markup.h"
  
#include<string>  
  
using namespace std;  
using namespace cv;  

// CBuild2D_OpenCVDlg 对话框
class CBuild2D_OpenCVDlg : public CDialogEx
{
// 构造
public:
	CBuild2D_OpenCVDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BUILD2D_OPENCV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnCancel();							//关闭程序
	void UpdateDatas();									//更新界面数据
	
	//串口控制
	int m_comNum;										// COM
	ComAccess com;										// 串口对象
	CString m_sendCStr;									// 串口发送的信息
	afx_msg void OnBnClickedBtnComm();
	afx_msg void OnBnClickedBtnSend();

	//相机控制
	COpenCVCam cam;										//OpenCV相机对象
	afx_msg void OnBnClickedBtnContinuous();			//实时
	afx_msg void OnBnClickedBtnCam();					//相机开关
	afx_msg void OnBnClickedBtnSingle();				//单采
	afx_msg void OnBnClickedBtnPic();					//跑图

	//初始化
	afx_msg void OnBnClickedBtnA();						//选择A点
	afx_msg void OnBnClickedBtnB();						//选择B点
	Point m_A,m_B;										//AB点	
	double m_ABdis;										//AB距离mm
	double m_ratio;										//比率mm/pix
	afx_msg void OnBnClickedBtnRatio();					//计算比率

	
	afx_msg void OnBnClickedCheckXy();
	afx_msg void OnBnClickedCheckReal();	
	BOOL m_xy;											// 是否显示坐标系
	BOOL m_real;										// 是否显示原图

	//zoom：窗口大小原图或矫正图
	//xyZoom：窗口大小原图或矫正图+坐标系
	//realZoom：窗口大小原图或矫正图实时显示物理坐标信息
	Mat zoom,xyZoom,realZoom;  

	//xyScr：原图或矫正图+坐标系
	//realScr：原图或矫正图实时显示物理坐标信息
	//rectifyScr;原图或矫正图
	Mat xyScr,realScr,rectifyScr; 

	//scr：原图
	Mat scr;

	afx_msg LRESULT OnUpdateMessage(WPARAM wParam, LPARAM lParam); //线程通知界面更新
	afx_msg void OnBnClickedBtnDis();
	// 图像分辨率
	CString m_pix;

	//绘画某点标志
	void DrawX(Mat &image, Point point);

	//图像畸变矫正
	void Rectify(Mat &scr, Mat &dst, CString xmlPath);
	// 是否畸变矫正
	BOOL m_isRectify;
	afx_msg void OnBnClickedCheckRectify();

	//缩小图上坐标转原图上坐标
	void ZoomToSrc(Mat &zoom, Mat &scr, Point zoomP, int &x, int &y);

	//HObject2Mat
	//Mat HObject2Mat(HObject Hobj);
};
