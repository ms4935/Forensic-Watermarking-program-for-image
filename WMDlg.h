
// WMDlg.h: 헤더 파일
//

#pragma once

//#include "LogIn.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "LogIn.h"

using namespace std;
using namespace cv;

// CWMDlg 대화 상자
class CWMDlg : public CDialogEx
{
// 생성입니다.
public:
	CWMDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	/*이미지 변수*/
	Mat QrImg; //Qrcode
	Mat HostImg; //Host image
	Mat MarkedImg; //QR Marked image

	/*Login*/
	LogIn LogInDialog; //LogIn dialog
	afx_msg void OnBnClickedLoginbutton();

	/*이미지 선택*/
	afx_msg void OnBnClickedImagebutton();

	/*워터마크 삽입*/
	afx_msg void OnBnClickedInsertbutton();

	/*워터마크 추출*/
	afx_msg void OnBnClickedExtrabutton();
};
