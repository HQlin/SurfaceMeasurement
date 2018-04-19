
// Build2D_OpenCVDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Build2D_OpenCV.h"
#include "Build2D_OpenCVDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_MESSAGE (WM_USER+200) //线程通知界面更新
#define DELAY_REAL 500					//实时采集图像延迟

//实时采集图像线程
DWORD WINAPI ThreadReal(LPVOID pthread)
{
	CBuild2D_OpenCVDlg * m_pDlg = (CBuild2D_OpenCVDlg*)pthread;
	CString butCStr;
	do{
		m_pDlg->GetDlgItemTextW(IDC_BTN_CONTINUOUS, butCStr);
		m_pDlg->PostMessage(WM_UPDATE_MESSAGE,0,0);
		Sleep(DELAY_REAL);
	}while(L"实时：开" == butCStr);
	return 0;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBuild2D_OpenCVDlg 对话框




CBuild2D_OpenCVDlg::CBuild2D_OpenCVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBuild2D_OpenCVDlg::IDD, pParent)
	, m_comNum(1)
	, m_sendCStr(_T(""))
	, m_ABdis(0)
	, m_ratio(0.1)
	, m_xy(FALSE)
	, m_real(FALSE)
	, m_pix(_T("相机控制"))
	, m_isRectify(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBuild2D_OpenCVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COM, m_comNum);
	DDX_Text(pDX, IDC_EDIT_SEND, m_sendCStr);
	DDX_Text(pDX, IDC_EDIT_AB, m_ABdis);
	DDX_Text(pDX, IDC_EDIT_RATIO, m_ratio);
	DDX_Check(pDX, IDC_CHECK_XY, m_xy);
	DDX_Check(pDX, IDC_CHECK_REAL, m_real);
	DDX_Text(pDX, IDC_STATIC_CAM, m_pix);
	DDX_Check(pDX, IDC_CHECK_RECTIFY, m_isRectify);
}

BEGIN_MESSAGE_MAP(CBuild2D_OpenCVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_COMM, &CBuild2D_OpenCVDlg::OnBnClickedBtnComm)
	ON_BN_CLICKED(IDC_BTN_SEND, &CBuild2D_OpenCVDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_SINGLE, &CBuild2D_OpenCVDlg::OnBnClickedBtnSingle)
	ON_BN_CLICKED(IDC_BTN_PIC, &CBuild2D_OpenCVDlg::OnBnClickedBtnPic)
	ON_BN_CLICKED(IDC_BTN_A, &CBuild2D_OpenCVDlg::OnBnClickedBtnA)
	ON_BN_CLICKED(IDC_BTN_B, &CBuild2D_OpenCVDlg::OnBnClickedBtnB)
	ON_BN_CLICKED(IDC_BTN_RATIO, &CBuild2D_OpenCVDlg::OnBnClickedBtnRatio)
	ON_BN_CLICKED(IDC_CHECK_XY, &CBuild2D_OpenCVDlg::OnBnClickedCheckXy)
	ON_BN_CLICKED(IDC_CHECK_REAL, &CBuild2D_OpenCVDlg::OnBnClickedCheckReal)
	ON_BN_CLICKED(IDC_BTN_CONTINUOUS, &CBuild2D_OpenCVDlg::OnBnClickedBtnContinuous)
	ON_BN_CLICKED(IDC_BTN_CAM, &CBuild2D_OpenCVDlg::OnBnClickedBtnCam)
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_BN_CLICKED(IDC_BTN_DIS, &CBuild2D_OpenCVDlg::OnBnClickedBtnDis)
	ON_BN_CLICKED(IDC_CHECK_RECTIFY, &CBuild2D_OpenCVDlg::OnBnClickedCheckRectify)
END_MESSAGE_MAP()


// CBuild2D_OpenCVDlg 消息处理程序

BOOL CBuild2D_OpenCVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	namedWindow("缩小图",WINDOW_AUTOSIZE);  
	HWND hWnd = (HWND)cvGetWindowHandle("缩小图");  
	HWND hParent = ::GetParent(hWnd);  
	::SetParent(hWnd,GetDlgItem(IDC_PIC)->m_hWnd);  
	::ShowWindow(hParent,SW_HIDE);  

	UpdateDatas();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBuild2D_OpenCVDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBuild2D_OpenCVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBuild2D_OpenCVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBuild2D_OpenCVDlg::OnBnClickedBtnComm()
{
	//串口开关
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_COMM, butCStr);
	if(L"关" == butCStr)
	{
		UpdateData(true);
		CString comCStr;
		comCStr.Format(L"COM%d", m_comNum);
		if ( ! com.Open(comCStr, 19200, 0, 0, 0) )
		{
			USES_CONVERSION; 
			CString str;
			str.Format(L"Error: Can't open communication device!\n%s", A2W(com.GetErrorMessage()));
			MessageBox(str);
			return;
		}
		SetDlgItemText(IDCANCEL,L"开");
	}
	else
	{
		com.Close();
		SetDlgItemText(IDCANCEL,L"关");
	}
	UpdateDatas();
}


void CBuild2D_OpenCVDlg::OnBnClickedBtnSend()
{
	UpdateData(TRUE);
	//发送给串口
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_COMM, butCStr);
	if(L"开" == butCStr)
	{
		USES_CONVERSION; 
		char* str = W2A(m_sendCStr);
		int write_result = com.WriteData(str, strlen(str));

		// -1 ? then we got an error and print it
		if ( write_result < 0 )
			MessageBox(A2W(com.GetErrorMessage()));

		//接收串口回馈信息
		int DATA_LEN = 10;
		char buf[255];
		int read_result = com.ReadData(buf, DATA_LEN);

		// -1 ? then we got an error and print it
		if ( (read_result < 0)  )
			MessageBox(A2W(com.GetErrorMessage()));

		// set end of received data
		buf[DATA_LEN] = '\0';

		CString dataRecv =  A2W(buf);
	}
	else
	{
		MessageBox(L"串口未打开！");
	}
}


void CBuild2D_OpenCVDlg::UpdateDatas()
{
	CString butCStr;
	//判断实时状态
	GetDlgItemTextW(IDC_BTN_CONTINUOUS, butCStr);
	if(L"实时：关" == butCStr)
	{	
		GetDlgItem(IDC_BTN_SINGLE)->EnableWindow(TRUE);	
		GetDlgItem(IDC_BTN_CAM)->EnableWindow(TRUE);		
	}
	else
	{	
		GetDlgItem(IDC_BTN_SINGLE)->EnableWindow(FALSE);	
		GetDlgItem(IDC_BTN_CAM)->EnableWindow(FALSE);	
	}
	//判断相机开关状态
	GetDlgItemTextW(IDC_BTN_CAM, butCStr);
	if(L"关" == butCStr)
	{
		GetDlgItem(IDC_BTN_SINGLE)->EnableWindow(FALSE);	
		GetDlgItem(IDC_BTN_CONTINUOUS)->EnableWindow(FALSE);		
	}
	else
	{	
		GetDlgItem(IDC_BTN_SINGLE)->EnableWindow(TRUE);	
		GetDlgItem(IDC_BTN_CONTINUOUS)->EnableWindow(TRUE);	
	}

	//判断串口开关状态
	GetDlgItemTextW(IDC_BTN_COMM, butCStr);
	if(L"关" == butCStr)
	{
		GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);	
	}
	else
	{
		GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);		
	}
	//更新图像分辨率
	m_pix.Format(L"相机控制：%d * %d", scr.cols,  scr.rows);
	//是否显示原图

	//读取比率值
	CMarkup xml;				//读写xml对象
	if (!xml.Load(CMarkup::GetFilePath(L"Setting.xml")))
		MessageBox(L"找不到该xml文件！");
	//第一层
	xml.ResetPos();
	if(xml.FindChildElem())
	{
		CString para;	
		//第二层
		bool b = xml.FindChildElem(L"Ratio");
		para = xml.GetChildData();
		m_ratio = _ttof(para.GetBuffer());
	}
	GetDlgItem(IDC_EDIT_RATIO)->EnableWindow(false);
	UpdateData(false);	
}

void CBuild2D_OpenCVDlg::OnBnClickedBtnSingle()
{
	//OpenCV采图
	try{
		scr = cam.GetImage();
		imwrite("采图测试.bmp", scr);
	}catch(cv::Exception)
	{
		AfxMessageBox(L"相机采图异常");
		scr = NULL;
	}

	//Halcon采图
	//try{
	//	HObject Hobj = hCam.GetHImage();
	//	scr = HObject2Mat(Hobj);
	//	char a0[256] = "";
	//	CMarkup::GetFilePath(a0, "采图测试.bmp");
	//	imwrite(a0, scr);
	//}catch(cv::Exception)
	//{
	//	AfxMessageBox(L"相机采图异常");
	//	scr = NULL;
	//}

	OnBnClickedCheckRectify();
}

void on_mouse(int event,int x,int y,int flags,void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{  
	CBuild2D_OpenCVDlg* pThis = (CBuild2D_OpenCVDlg*)ustc;
    static Point cur_pt = (-1,-1);//对应图实时坐标
	char temp[16];
	pThis->UpdateData(true);
	
	//对应图实时坐标
	cur_pt = Point(x,y);  
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下
	{  
		if(pThis->m_real)
		{
			//显示原图
			pThis->DrawX(pThis->xyScr, cur_pt);					
		}
		else
		{
			//显示缩小图
			pThis->DrawX(pThis->xyZoom, cur_pt);
			pThis->ZoomToSrc(pThis->xyZoom, pThis->scr, cur_pt, x, y);		
		}
		pThis->m_sendCStr.Format(L"(%.2f,%.2f)", (x - pThis->scr.cols/2)*pThis->m_ratio, -(y - pThis->scr.rows/2)*pThis->m_ratio);	
		pThis->UpdateData(false);
	}  
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数  
	{  
		if(pThis->m_real)
		{
			//显示原图
			pThis->xyScr.copyTo(pThis->realScr); 			

			sprintf(temp,"(%.2f,%.2f)", (x - pThis->scr.cols/2)*pThis->m_ratio, -(y - pThis->scr.rows/2)*pThis->m_ratio);  
			putText(pThis->realScr,temp,cur_pt,FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,255,255));//只是实时显示鼠标移动的坐标  
			imshow("原图",pThis->realScr); 
		}
		else
		{
			//显示缩小图
			pThis->xyZoom.copyTo(pThis->realZoom);//将xyZoom复制到临时图像realZoom上，用于显示实时坐标     
			pThis->ZoomToSrc(pThis->xyZoom, pThis->scr, cur_pt, x, y);
			sprintf(temp,"(%.2f,%.2f)", (x - pThis->scr.cols/2)*pThis->m_ratio, -(y - pThis->scr.rows/2)*pThis->m_ratio);
			putText(pThis->realZoom,temp,cur_pt,FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,255,255));//只是实时显示鼠标移动的坐标  
			imshow("缩小图",pThis->realZoom);  
		}

	}  
	else if (event == CV_EVENT_LBUTTONUP)//左键松开
	{   
		//使用原图坐标
		if(!pThis->m_real)
		{
			pThis->ZoomToSrc(pThis->xyZoom, pThis->scr, cur_pt, x, y);
		}
		CString butCStr;	
		pThis->GetDlgItemTextW(IDC_BTN_A, butCStr);
		if(L"选择A点" == butCStr)
		{
			pThis->m_A.x = x;
			pThis->m_A.y = y;
			butCStr.Format(L"A(%d, %d)", x, y);
			pThis->SetDlgItemText(IDC_BTN_A, butCStr);
		}

		pThis->GetDlgItemTextW(IDC_BTN_B, butCStr);
		if(L"选择B点" == butCStr)
		{
			pThis->m_B.x = x;
			pThis->m_B.y = y;
			butCStr.Format(L"B(%d, %d)", x, y);
			pThis->SetDlgItemText(IDC_BTN_B, butCStr);
		}
	}  
}  

void CBuild2D_OpenCVDlg::OnBnClickedBtnPic()
{
	CString picPath;   //定义图片路径变量    
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY |   
		OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,   NULL, this);   //选择文件对话框    

	if(dlg.DoModal() == IDOK)    
	{    
		picPath= dlg.GetPathName();  //获取图片路径    
	}    
	//CString to string  
	USES_CONVERSION; 
	string picpath = W2A(picPath);      

	scr = imread(picpath);        

	OnBnClickedCheckRectify();
}


void CBuild2D_OpenCVDlg::OnCancel()
{
	CString butCStr;
	//关闭串口
	GetDlgItemTextW(IDCANCEL, butCStr);
	if(L"开" == butCStr)
	{
		OnBnClickedBtnComm();
	}
	//关闭相机
	GetDlgItemTextW(IDC_BTN_CAM, butCStr);
	if(L"开" == butCStr)
	{
		OnBnClickedBtnCam();
	}
	Sleep(DELAY_REAL);
	//退出程序
	exit(0);
	CDialogEx::OnCancel();
}


void CBuild2D_OpenCVDlg::OnBnClickedBtnA()
{
	//A点
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_A, butCStr);
	if(L"选择A点" != butCStr)
	{
		SetDlgItemText(IDC_BTN_A,L"选择A点");
	}
	else
	{
		MessageBox(L"请选择A点！");
	}
}


void CBuild2D_OpenCVDlg::OnBnClickedBtnB()
{
	//B点
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_B, butCStr);
	if(L"选择A点" != butCStr)
	{
		SetDlgItemText(IDC_BTN_B,L"选择B点");
	}
	else
	{
		MessageBox(L"请选择B点！");
	}
}


void CBuild2D_OpenCVDlg::OnBnClickedBtnRatio()
{
	//计算比率
	UpdateData(true);
	double a = (m_A.x-m_B.x)*(m_A.x-m_B.x) + (m_A.y-m_B.y)*(m_A.y-m_B.y);
	m_ratio = m_ABdis/sqrt(a);

	//保存比率值
	CMarkup xml;				//读写xml对象
	if (!xml.Load(CMarkup::GetFilePath(L"Setting.xml")))
		MessageBox(L"找不到该xml文件！");
	//第一层
	xml.ResetPos();
	if(xml.FindChildElem())
	{
		CString para;	
		//第二层
		bool b = xml.FindChildElem(L"Ratio");
		para.Format(L"%f", m_ratio);
		xml.SetChildData(para);
		if(!xml.Save(CMarkup::GetFilePath(L"Setting.xml"))){
			MessageBox(L"保存比率值失败！");
		}
	}
	UpdateDatas();
}

void CBuild2D_OpenCVDlg::OnBnClickedBtnContinuous()
{
	//连采
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_CONTINUOUS, butCStr);
	if(L"实时：关" == butCStr)
	{
		CreateThread(NULL,0,ThreadReal,this,0,NULL);
		SetDlgItemText(IDC_BTN_CONTINUOUS, L"实时：开");
	}
	else
	{
		SetDlgItemText(IDC_BTN_CONTINUOUS, L"实时：关");
	}
}


void CBuild2D_OpenCVDlg::OnBnClickedBtnCam()
{
	//相机开关
	CString butCStr;
	GetDlgItemTextW(IDC_BTN_CAM, butCStr);
	if(L"关" == butCStr)
	{
		//OpenCV相机
		if(cam.OpenCam(0))
		{
			SetDlgItemText(IDC_BTN_CAM, L"开");				
		}
		//Halcon相机
		//if(hCam.OpenCamera(""))
		//{
		//	SetDlgItemText(IDC_BTN_CAM, L"开");	
		//}
	}
	else
	{
		//OpenCV相机
		cam.CloseCam();
		//Halcon相机
		//hCam.CloseCamera();
		SetDlgItemText(IDC_BTN_CAM, L"关");			
	}
	UpdateDatas();
}


LRESULT CBuild2D_OpenCVDlg::OnUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	//线程通知界面更新
	OnBnClickedBtnSingle();
	return 0;
}

void CBuild2D_OpenCVDlg::OnBnClickedBtnDis()
{
	//计算AB距离mm：
	UpdateData(true);
	CString butCStr;
	double a = (m_A.x-m_B.x)*(m_A.x-m_B.x) + (m_A.y-m_B.y)*(m_A.y-m_B.y);
	double dis = m_ratio*sqrt(a);
	butCStr.Format(L"计算AB距离mm：%.2f", dis);
	SetDlgItemText(IDC_BTN_DIS, butCStr);		
}

void CBuild2D_OpenCVDlg::DrawX(Mat &image, Point point)
{
	//绘画某点标志
	int size = 5;
	line(image,Point(point.x - size, point.y - size),cvPoint(point.x + size, point.y + size),CV_RGB(0,255,255),2,CV_AA,0);  
    line(image,Point(point.x + size, point.y - size),cvPoint(point.x - size, point.y + size),CV_RGB(0,255,255),2,CV_AA,0);  
}

void CBuild2D_OpenCVDlg::Rectify(Mat &scr, Mat &dst, CString xmlPath)
{
	//图像畸变矫正
	//内参矩阵, 就算复制代码，也不要用我的参数。摄像头都不一样...
	Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
	//畸变参数，不要用我的参数~
    Mat distCoeffs = Mat::zeros(5, 1, CV_64F);

	CMarkup xml;				//读写xml对象
	if (!xml.Load(CMarkup::GetFilePath(xmlPath)))
		MessageBox(L"找不到该xml文件！");
	
	//第一层
	xml.ResetPos();
	if(xml.FindChildElem())
	{
		CString para;	
		//第二层
		bool b = xml.FindChildElem(L"CameraMatrix");
		xml.IntoElem();

		{				
			xml.FindChildElem(L"Mat00");
			para = xml.GetChildData();
			cameraMatrix.at<double>(0, 0) = _ttof(para.GetBuffer());
			xml.FindChildElem(L"Mat01");
			para = xml.GetChildData();
			cameraMatrix.at<double>(0, 1) = _ttof(para.GetBuffer());
			xml.FindChildElem(L"Mat02");
			para = xml.GetChildData();
			cameraMatrix.at<double>(0, 2) = _ttof(para.GetBuffer());

			xml.FindChildElem(L"Mat10");
			para = xml.GetChildData();
			cameraMatrix.at<double>(1, 0) = _ttof(para.GetBuffer());
			xml.FindChildElem(L"Mat11");
			para = xml.GetChildData();
			cameraMatrix.at<double>(1, 1) = _ttof(para.GetBuffer());
			xml.FindChildElem(L"Mat12");
			para = xml.GetChildData();
			cameraMatrix.at<double>(1, 2) = _ttof(para.GetBuffer());

			xml.FindChildElem(L"Mat20");
			para = xml.GetChildData();
			cameraMatrix.at<double>(2, 0) = _ttof(para.GetBuffer());
			xml.FindChildElem(L"Mat21");
			para = xml.GetChildData();
			cameraMatrix.at<double>(2, 1) = _ttof(para.GetBuffer());
			xml.FindChildElem(L"Mat22");
			para = xml.GetChildData();
			cameraMatrix.at<double>(2, 2) = _ttof(para.GetBuffer());
		}

		//第二层
		xml.OutOfElem();
		//第三层
		bool bb = xml.FindChildElem(L"DistCoeffs");
		xml.IntoElem();

		{
			xml.FindChildElem(L"Mat0");
			para = xml.GetChildData();
			distCoeffs.at<double>(0, 0) = _ttof(para.GetBuffer());

			xml.FindChildElem(L"Mat1");
			para = xml.GetChildData();
			distCoeffs.at<double>(1, 0) = _ttof(para.GetBuffer());


			xml.FindChildElem(L"Mat2");
			para = xml.GetChildData();
			distCoeffs.at<double>(2, 0) = _ttof(para.GetBuffer());

			xml.FindChildElem(L"Mat3");
			para = xml.GetChildData();
			distCoeffs.at<double>(3, 0) = _ttof(para.GetBuffer());

			xml.FindChildElem(L"Mat4");
			para = xml.GetChildData();
			distCoeffs.at<double>(4, 0) = _ttof(para.GetBuffer());
		}
	}

    Mat view, rview, map1, map2;
	Mat R = Mat::eye(3,3,CV_32F);  
    Size imageSize;
    imageSize = scr.size();
    initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imageSize, CV_16SC2, map1, map2);
    remap(scr, dst, map1, map2, INTER_LINEAR);
}


void CBuild2D_OpenCVDlg::OnBnClickedCheckRectify()
{
	//1、是否畸变矫正
	UpdateData(true);
	if(m_isRectify)
	{
		Rectify(scr, rectifyScr, L"Setting.xml");
		char a0[256] = "";
		CMarkup::GetFilePath(a0, "矫正后.bmp");
		imwrite(a0, rectifyScr);
	}
	else
	{
		scr.copyTo(rectifyScr);
	}
	OnBnClickedCheckReal();
}

void CBuild2D_OpenCVDlg::OnBnClickedCheckReal()
{
	//2、是否显示原图
	UpdateData(true);
	if(m_real)
	{			
		namedWindow("原图");  		
		rectifyScr.copyTo(xyScr); 
		imshow("原图",xyScr); 
		setMouseCallback("原图",on_mouse,this);  
	}
	else
	{
		destroyWindow("原图");
		//以下操作获取图形控件尺寸并以此改变图片尺寸    
		CRect rect;    
		GetDlgItem(IDC_PIC)->GetClientRect(&rect);    
		Rect dst(rect.left,rect.top,rect.right,rect.bottom);    
		resize(rectifyScr,zoom,cv::Size(rect.Width(),rect.Height()));     
		setMouseCallback("缩小图",on_mouse,this);  
		zoom.copyTo(xyZoom);  
	}
	OnBnClickedCheckXy();
}

void CBuild2D_OpenCVDlg::OnBnClickedCheckXy()
{
	//3、是否显示坐标系
	UpdateData(true);
	if(m_real)
	{
		if(m_xy)
		{
			line(xyScr,Point(0,xyScr.rows/2),Point(scr.cols,xyScr.rows/2),Scalar(0,255,0),1,CV_AA);  
			line(xyScr,Point(xyScr.cols/2,0),Point(scr.cols/2,xyScr.rows),Scalar(0,255,0),1,CV_AA);	
			xyScr.copyTo(realScr); 
		}
		else
		{
			rectifyScr.copyTo(xyScr);  
			xyScr.copyTo(realScr);		
		}
		imshow("原图",xyScr);  
	}
	else
	{
		if(m_xy)
		{
			line(xyZoom,Point(0,xyZoom.rows/2),Point(xyZoom.cols,xyZoom.rows/2),Scalar(0,255,0),1,CV_AA);  
			line(xyZoom,Point(xyZoom.cols/2,0),Point(xyZoom.cols/2,xyZoom.rows),Scalar(0,255,0),1,CV_AA);
			xyZoom.copyTo(realZoom); 
		}
		else
		{
			zoom.copyTo(xyZoom);  
			xyZoom.copyTo(realZoom);		
		}
		imshow("缩小图",xyZoom);  
	}
	UpdateDatas();
}

void CBuild2D_OpenCVDlg::ZoomToSrc(Mat &zoom, Mat &scr, Point zoomP, int &x, int &y)
{
	//缩小图上坐标转原图上坐标
	double cols = (double)zoom.cols/scr.cols;
	double rows = (double)zoom.rows/scr.rows;

	x = zoomP.x/cols;
	y = zoomP.y/rows;
}

//Mat CBuild2D_OpenCVDlg::HObject2Mat(HObject Hobj)  
//{  
//    HTuple htCh=HTuple();  
//    HTuple cType;  
//    Mat Image;  
//    ConvertImageType(Hobj,&Hobj,"byte");  
//    CountChannels(Hobj,&htCh);  
//    HTuple wid;  
//    HTuple hgt;  
//    int W,H;  
//    if(htCh[0].I()==1)  
//    {  
//        HTuple ptr;  
//        GetImagePointer1(Hobj,&ptr,&cType,&wid,&hgt);  
//        W=(Hlong)wid;  
//        H=(Hlong)hgt;  
//        Image.create(H,W,CV_8UC1);  
//        uchar* pdata=(uchar*)ptr[0].I();  
//        memcpy(Image.data,pdata,W*H);  
//    }  
//    else if(htCh[0].I()==3)  
//    {  
//        HTuple ptrR, ptrG, ptrB;  
//        GetImagePointer3(Hobj,&ptrR,&ptrG,&ptrB,&cType,&wid,&hgt);  
//        W=(Hlong)wid;  
//        H=(Hlong)hgt;  
//        Image.create(H,W,CV_8UC3);  
//        vector<Mat> vecM(3);  
//        vecM[2].create(H,W,CV_8UC1);  
//        vecM[1].create(H,W,CV_8UC1);  
//        vecM[0].create(H,W,CV_8UC1);  
//        uchar* pr=(uchar*)ptrR[0].I();  
//        uchar* pg=(uchar*)ptrG[0].I();  
//        uchar* pb=(uchar*)ptrB[0].I();  
//        memcpy(vecM[2].data,pr,W*H);  
//        memcpy(vecM[1].data,pg,W*H);  
//        memcpy(vecM[0].data,pb,W*H);  
//        merge(vecM,Image);  
//    }  
//    return Image;  
//}  
