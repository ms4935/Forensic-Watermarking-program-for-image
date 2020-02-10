#pragma once


// LogIn 대화 상자
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

class LogIn : public CDialogEx
{
	DECLARE_DYNAMIC(LogIn)

public:
	LogIn(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LogIn();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LoginDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	Mat QrImg; //Qrcode
	CEdit ID; //Input data
	afx_msg void OnBnClickedLog();

	virtual BOOL PreTranslateMessage(MSG* pMsg); //Enter key
};
