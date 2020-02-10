#include "stdafx.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "BitBuffer.hpp"
#include "QrCode.hpp"

#include "QRmain.h"

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;
using namespace std;
using namespace cv;

/* Creates a single QR Code, then prints it to the console. */
Mat doBasicDemo(CString id)
{

	int len = id.GetLength();
	char* text = new char[len+1];
	WideCharToMultiByte(CP_ACP, 0, id, -1, text, len + 1, NULL, NULL);
	

	const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;  // Error correction level

	// Make and print the QR Code symbol
	const QrCode qr = QrCode::encodeText(text, errCorLvl);

	Mat QR;
	QR = printQr(qr, 300); // qrcode size를 32로 줌

	return QR;
}

// Prints the given QrCode object to the console.
Mat printQr(const QrCode &qr, int size)
{
	int border = 4;
	Mat QRCODE(size, size, CV_8UC1, Scalar(255));
	int intW = 1; //프린트 가중치 : 선의 두께를 결정한다

	int a = 0;
	int b = (32 - qr.getSize()) / 2;
	int re = 1; // 전체적인 비율로 크기 조절 
	for (int y = (-border); y < (qr.getSize() + border); y++)
	{
		int f = 0;
		a = 0;
		for (int x = (-border); x < (qr.getSize() + border); x++)
		{
			if (qr.getModule(x, y) == true) // black
			{ 
				for (int i = 0; i < re; i++)
				{
					line(QRCODE, Point(a + i, b), Point(a + i, b + re), Scalar(0), 1); 
				}
				f = 1;
			}
			else // white
			{
				for (int i = 0; i < re; i++)
				{
					line(QRCODE, Point(a + i, b), Point(a + i, b + re), Scalar(255), 1); 
				}
			}
			a += re;
		}
		if (f == 1) { b += re; }
	}

	//// 그려진 qrcode를 이미지 파일로 저장
	//const char* qrcodeName = "QRCODE.jpg";
	//imwrite(qrcodeName, QRCODE);

   //// NCC 계산을 위해 추출된 워터마크와 동일한 원본 워터마크 생성
	Mat QR(34, 34, QRCODE.type(), Scalar(255));
	//cout << qr.getSize() << "  " << qr.getSize() + 2 << endl;
	// 결정된 QRcode의 픽셀 값을 위치에 맞게 저장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QR.at<uchar>(i + 1, j + 1) = QRCODE.at<uchar>(i, j);
		}
	}

	Mat QRCODE_100(100, 100, QRCODE.type(), Scalar(255));

	int n = 0;
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QRCODE_100.at<uchar>(n, 3 * j) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n, 3 * j + 1) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n, 3 * j + 2) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n + 1, 3 * j) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n + 1, 3 * j + 1) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n + 1, 3 * j + 2) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n + 2, 3 * j) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n + 2, 3 * j + 1) = QR.at<uchar>(i, j);
			QRCODE_100.at<uchar>(n + 2, 3 * j + 2) = QR.at<uchar>(i, j);
		}
		n += 3;
	}
	//imshow("Original_Qrcode", QRCODE_100);
	imwrite("Original_QRCODE.png", QRCODE_100);

	return QRCODE;
}