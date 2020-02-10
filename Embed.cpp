#pragma once
#include "stdafx.h"
#include "Headers.h"

////////////////////////////////////////////////////////////////////////////////////////
// 삽입
////////////////////////////////////////////////////////////////////////////////////////
Mat EmbedWatermark(Mat& HostImg, Mat& QrImg)
{
	return Embed(HostImg, QrImg);
}

Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	vector<Mat> yuv_arr(3);
	Mat WT_result;
	Mat IWT_result;
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode 각 픽셀 값을 255, 0으로 저장할 행렬 변수 생성

	// QR의 데이터를 0과 255로 설정
	for (int y = 0; y < QrImg.rows; y++)
		for (int x = 0; x < QrImg.cols; x++)
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr); // 채널 분리

	WT(yuv_arr[0], WT_result, 1); // Y채-널을 대상으로 1단계 DWT 진행

	// 부대역의 계수를 저장할 행렬 변수    
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

	LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
    HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
	HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	vector<Mat> LH_blocks;
	vector<Mat> HH_blocks;
	vector<Mat> HL_blocks;
	int value[1024]; // QR의 삽입 값을 저장할 배열
	int i = 0;
	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			LH_blocks.push_back(Mat(LH_subband, rect));
			HH_blocks.push_back(Mat(HH_subband, rect));
			HL_blocks.push_back(Mat(HL_subband, rect));
			// 삽입 값을 위한 파라미터 지정
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}
	// 1024개의 8*8 블록에 dct 적용과 삽입 후 역dct 적용
	for (int i = 0; i < 1024; i++)
	{
		dct(LH_blocks[i], LH_blocks[i]);
		dct(HH_blocks[i], HH_blocks[i]);
		dct(HL_blocks[i], HL_blocks[i]);

		LH_blocks[i].at<float>(0, 0) += value[i] * 25;	// 이전에 설정한 삽입 파라미터를 이용하여 원본 이미지의 DC 값에 추가
		HH_blocks[i].at<float>(0, 0) += value[i] * 25;	// 이전에 설정한 삽입 파라미터를 이용하여 원본 이미지의 DC 값에 추가
		HL_blocks[i].at<float>(0, 0) += value[i] * 25;	// 이전에 설정한 삽입 파라미터를 이용하여 원본 이미지의 DC 값에 추가
		
		dct(LH_blocks[i], LH_blocks[i], DCT_INVERSE);
		dct(HH_blocks[i], HH_blocks[i], DCT_INVERSE);
		dct(HL_blocks[i], HL_blocks[i], DCT_INVERSE);
	}

	// IWT 수행
	IWT(WT_result, IWT_result, 1);
	IWT_result.convertTo(yuv_arr[0], CV_8U);
	merge(yuv_arr, yuv);

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // YCrCb to RGB

	// 압축률 지정을 위한 부분
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;		// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;	// default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;
	param95[1] = 95;

	imwrite("Marked_image.png", yuv);
	imwrite("Marked_image_75.jpg", yuv, param75);
	imwrite("Marked_image_95.jpg", yuv, param95);

	////////////////////// 압축 공격 이외의 공격 ///////////////////////////
/*	Mat attack = imread("[blackgreygradient]DWT_DCT_CRT_LH.png");
	Mat GblurredImg1, GblurredImg2, Ablur, Mblur;
	Mat SPimg;

	GaussianBlur(attack, GblurredImg1, Size(3, 3), 0);
	GaussianBlur(attack, GblurredImg2, Size(5, 5), 0);
	blur(attack, Ablur, Size(3, 3));                  //Average blurring
	medianBlur(attack, Mblur, 3);

	SPimg = attack.clone();
	SaltandPepper(SPimg, 0.03);

	imwrite("2Gaussian Blurring 3x3.png", GblurredImg1);
	imwrite("2Gaussian Blurring 5x5.png", GblurredImg2);
	imwrite("2Average Blurring 3x3.png", Ablur);
	imwrite("2Median Blurring 3x3.png", Mblur);
	imwrite("2Salt and Pepper 3%.png", SPimg);
*/
	return yuv;
}