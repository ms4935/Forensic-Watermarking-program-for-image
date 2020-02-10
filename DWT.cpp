
#include "stdafx.h"
#include "DWT.h"

//-------------------------------- 
// Garrot 수축 함수
//-------------------------------- 
float Garrot_shrink(float d, float T)
{
	float res;
	if (fabs(d) > T)
	{
		res = d - ((T*T) / d);
	}
	else
	{
		res = 0;
	}

	return res;
}

//-------------------------------- 
// Wavelet transform 
//-------------------------------- 
static void cvHaarWavelet(Mat &src, Mat &dst, int NIter)
{
	float c, dh, dv, dd;
	//assert(src.type() == CV_32FC1);
	//assert(dst.type() == CV_32FC1);
	int width = src.cols;         //cout << "width : " << src.cols << endl;
	int height = src.rows;        //cout << "height : " << src.rows << endl;

	//-------------------------------- 
	// NIter - 웨이블릿 변환 단계
	//-------------------------------- 
	for (int k = 0; k < NIter; k++)
	{
		///cout << "height >> (k+1) : " << (height >> (k + 1)) << endl;
		///cout << "width >> (k+1) : " << (width >> (k + 1)) << endl;
		for (int y = 0; y < (height >> (k + 1)); y++)   // 2^(k+1)로 height를 나눈다.
		{
			for (int x = 0; x < (width >> (k + 1)); x++)
			{
				c = (src.at<float>(2 * y, 2 * x) + src.at<float>(2 * y, 2 * x + 1) + src.at<float>(2 * y + 1, 2 * x) + src.at<float>(2 * y + 1, 2 * x + 1))*0.5;
				dst.at<float>(y, x) = c;

				dh = (src.at<float>(2 * y, 2 * x) + src.at<float>(2 * y + 1, 2 * x) - src.at<float>(2 * y, 2 * x + 1) - src.at<float>(2 * y + 1, 2 * x + 1))*0.5;
				dst.at<float>(y, x + (width >> (k + 1))) = dh;

				dv = (src.at<float>(2 * y, 2 * x) + src.at<float>(2 * y, 2 * x + 1) - src.at<float>(2 * y + 1, 2 * x) - src.at<float>(2 * y + 1, 2 * x + 1))*0.5;
				dst.at<float>(y + (height >> (k + 1)), x) = dv;

				dd = (src.at<float>(2 * y, 2 * x) - src.at<float>(2 * y, 2 * x + 1) - src.at<float>(2 * y + 1, 2 * x) + src.at<float>(2 * y + 1, 2 * x + 1))*0.5;
				dst.at<float>(y + (height >> (k + 1)), x + (width >> (k + 1))) = dd;
			}
		}
		dst.copyTo(src);      // dst -> src
	}
}

//-------------------------------- 
// Inverse wavelet transform 
//-------------------------------- 
static void cvInvHaarWavelet(Mat &src, Mat &dst, int NIter, float SHRINKAGE_T)
{
	float c, dh, dv, dd;
	//assert(src.type() == CV_32FC1);
	//assert(dst.type() == CV_32FC1);
	int width = src.cols;       //  cout << "width : " << src.cols << endl;
	int height = src.rows;      //   cout << "height : " << src.rows << endl;
	//-------------------------------- 
	// NIter - 웨이블릿 변환 단계
	//-------------------------------- 
	for (int k = NIter; k > 0; k--)
	{
		for (int y = 0; y < (height >> k); y++)
		{
			for (int x = 0; x < (width >> k); x++)
			{
				c = src.at<float>(y, x);
				dh = src.at<float>(y, x + (width >> k));
				dv = src.at<float>(y + (height >> k), x);
				dd = src.at<float>(y + (height >> k), x + (width >> k));

				// (shrinkage) 
				dh = Garrot_shrink(dh, SHRINKAGE_T);
				dv = Garrot_shrink(dv, SHRINKAGE_T);
				dd = Garrot_shrink(dd, SHRINKAGE_T);

				//------------------- 
				dst.at<float>(y * 2, x * 2) = 0.5*(c + dh + dv + dd);
				dst.at<float>(y * 2, x * 2 + 1) = 0.5*(c - dh + dv - dd);
				dst.at<float>(y * 2 + 1, x * 2) = 0.5*(c + dh - dv - dd);
				dst.at<float>(y * 2 + 1, x * 2 + 1) = 0.5*(c - dh - dv + dd);
			}
		}
		// 주석처리하면 src에 dst가 저장 안됨
		Mat C = src(Rect(0, 0, width >> (k - 1), height >> (k - 1)));
		Mat D = dst(Rect(0, 0, width >> (k - 1), height >> (k - 1)));
		D.copyTo(C);      // C 행렬에 D 행렬의 데이터 복사
	}
}
//-------------------------------- 
// Wavelte worker
//-------------------------------- 
void WT(Mat& img, Mat& dst, int NIter)
{
	Mat Ori = Mat(img.rows, img.cols, CV_32FC1);
	Mat Src = Mat(img.rows, img.cols, CV_32FC1);
	Mat Dst = Mat(img.rows, img.cols, CV_32FC1);
	Dst = 0;
	img.convertTo(Ori, CV_32FC1);
	Ori.copyTo(Src);

	cvHaarWavelet(Src, Dst, NIter);         // Src를 W-변환하여 Dst에 저장 (NIter : 반복횟수 = 해상도 개수 지정)
	Dst.copyTo(dst);						// dst -> src
}
void IWT(Mat& dst, Mat& idst, int NIter)
{
	Mat IDst_temp = Mat(dst.rows, dst.cols, CV_32FC1);
	cvInvHaarWavelet(dst, IDst_temp, NIter, 5);   // W-변환의 결과인 Temp를 대상으로 역W-변환하여 Filtered에 저장
	IDst_temp.copyTo(idst);
}
