// LogIn.cpp: 구현 파일
//

#include "stdafx.h"
#include "WM.h"
#include "LogIn.h"
#include "afxdialogex.h"

#include "QRmain.h"


// LogIn 대화 상자

IMPLEMENT_DYNAMIC(LogIn, CDialogEx)

LogIn::LogIn(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LoginDlg, pParent)
{

}

LogIn::~LogIn()
{
}

void LogIn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, ID);
}


BEGIN_MESSAGE_MAP(LogIn, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &LogIn::OnBnClickedLog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void LogIn::OnBnClickedLog()
{
	CString data;   // m_Editln에 입력되는 문자열 저장하기 위한 CString변수

	CString tmpStr = _T("");
	int v;

	// edit Control에 입력되는 문자열을 CString에 대입
	ID.GetWindowTextW(data);

	QrImg = doBasicDemo(data); // Qrcode Image

	OnOK();
}


BOOL LogIn::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		OnBnClickedLog();
		return true;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
