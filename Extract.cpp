#pragma once
#include "stdafx.h"
#include "Headers.h"

////////////////////////////////////////////////////////////////////////////////////////
// 추출
////////////////////////////////////////////////////////////////////////////////////////
void ExtractWatermark(Mat& marked_img)
{
	Extract(marked_img);
}

void Extract(Mat& Marked_Image)
{
	Mat yuv_arr[3];
	cvtColor(Marked_Image, Marked_Image, COLOR_RGB2YCrCb);   // RGB to YCrCb
	split(Marked_Image, yuv_arr);						     // 채널 분리
	Mat Marked_Y_channel = Mat(Marked_Image.cols, Marked_Image.rows, Marked_Image.type());
	int QRcodeSize;

	yuv_arr[0].copyTo(Marked_Y_channel);                // Y 채널 분리
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);    //uchar -> float

	// 추출한 QRcode 계수들을 저장할 행렬
	Mat LH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat HH_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);
	Mat HL_recoverd_QRcode_Pixel = Mat(32, 32, CV_8UC1);

	Mat WT_result;
	yuv_arr[0].convertTo(Marked_Y_channel, CV_32F);      //uchar -> float
	WT(Marked_Y_channel, WT_result, 1);               // 분리한 Y 채널을 대상으로 1단계 DWT 진행
	//imshow("Extracted_Image_WT", WT_result);

	// 부대역의 계수를 저장할 행렬 변수
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

	
	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
	HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2)); // HH
	HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));


	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	// 각 부대역의 블럭들
	vector<Mat> LH_blocks;
	vector<Mat> HH_blocks;
	vector<Mat> HL_blocks;

	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			LH_blocks.push_back(Mat(LH_subband, rect));
		    HH_blocks.push_back(Mat(HH_subband, rect));
			HL_blocks.push_back(Mat(HL_subband, rect));
		}
	}

	int x = 0, y = 0;
	// 1024개의 8*8 블록에 dct 적용 후 값 출력
	for (int i = 0; i < 1024; i++)
	{
		dct(LH_blocks[i], LH_blocks[i]);
		dct(HH_blocks[i], HH_blocks[i]);
		dct(HL_blocks[i], HL_blocks[i]);
	}

	// 각 부대역의 1024개의 블럭들을 대상으로 삽입된 워터마크 추출 진행
	for (int i = 0; i < 1024; i++)
	{
		LH_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x)) = LH_blocks[i].at<float>(0, 0) > 0 ? 255 : 0;
		HH_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x)) = HH_blocks[i].at<float>(0, 0) > 0 ? 255 : 0;
		HL_recoverd_QRcode_Pixel.at<uchar>((int)(y), (int)(x++)) = HL_blocks[i].at<float>(0, 0) > 0 ? 255 : 0;

		if (x == 32)
		{
			y++;
			x = 0;
		}
	}

	// QRcode를 생성할 행렬 변수 설정
	QRcodeSize = LH_recoverd_QRcode_Pixel.rows;
	Mat QR_HH(QRcodeSize + 2, QRcodeSize + 2, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat QR_LH(QRcodeSize + 2, QRcodeSize + 2, LH_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat QR_HL(QRcodeSize + 2, QRcodeSize + 2, HL_recoverd_QRcode_Pixel.type(), Scalar(255));

	// 결정된 QRcode의 픽셀 값을 위치에 맞게 저장
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			QR_HH.at<uchar>(i + 1, j + 1) = HH_recoverd_QRcode_Pixel.at<uchar>(i, j);
			QR_LH.at<uchar>(i + 1, j + 1) = LH_recoverd_QRcode_Pixel.at<uchar>(i, j);
			QR_HL.at<uchar>(i + 1, j + 1) = HL_recoverd_QRcode_Pixel.at<uchar>(i, j);
		}
	}

	// 32x32 크기의 QR의 크기 100x100으로 확장
	Mat BIG_QR_HH(100, 100, HH_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat BIG_QR_LH(100, 100, LH_recoverd_QRcode_Pixel.type(), Scalar(255));
	Mat BIG_QR_HL(100, 100, HL_recoverd_QRcode_Pixel.type(), Scalar(255));
	int nn = 0;
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			BIG_QR_HH.at<uchar>(nn, 3 * j) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn, 3 * j + 1) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn, 3 * j + 2) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn + 1, 3 * j) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn + 1, 3 * j + 1) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn + 1, 3 * j + 2) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn + 2, 3 * j) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn + 2, 3 * j + 1) = QR_HH.at<uchar>(i, j);
			BIG_QR_HH.at<uchar>(nn + 2, 3 * j + 2) = QR_HH.at<uchar>(i, j);

			BIG_QR_LH.at<uchar>(nn, 3 * j) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn, 3 * j + 1) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn, 3 * j + 2) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 1, 3 * j) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 1, 3 * j + 1) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 1, 3 * j + 2) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 2, 3 * j) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 2, 3 * j + 1) = QR_LH.at<uchar>(i, j);
			BIG_QR_LH.at<uchar>(nn + 2, 3 * j + 2) = QR_LH.at<uchar>(i, j);

			BIG_QR_HL.at<uchar>(nn, 3 * j) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn, 3 * j + 1) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn, 3 * j + 2) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn + 1, 3 * j) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn + 1, 3 * j + 1) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn + 1, 3 * j + 2) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn + 2, 3 * j) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn + 2, 3 * j + 1) = QR_HL.at<uchar>(i, j);
			BIG_QR_HL.at<uchar>(nn + 2, 3 * j + 2) = QR_HL.at<uchar>(i, j);
		}
		nn += 3;
	}

	imshow("HH_Extracted_QRcode", BIG_QR_HH);
	imshow("LH_Extracted_QRcode", BIG_QR_LH);
	imshow("HL_Extracted_QRcode", BIG_QR_HL);

	imwrite("HH_Extracted_QRcode.png", BIG_QR_HH);
	imwrite("HL_Extracted_QRcode.png", BIG_QR_HL);
	imwrite("LH_Extracted_QRcode.png", BIG_QR_LH);


	// if want to check psnr, ncc
	// modify getPSNR(), getNCC and test this code
	//cout << "------------- DWT-DCT -------------" << endl;
	//cvtColor(Marked_Image, Marked_Image, COLOR_YCrCb2RGB);   // YCrCb to RGB
	//getPSNR(Marked_Image); // 원본 이미지와 삽입 이미지의 PSNR 값 계산을 위함
	//getNCC();      // 삽입된 워터마크와 추출된 워터마크 간 NCC 값 계산
}