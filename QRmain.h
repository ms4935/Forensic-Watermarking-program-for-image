#pragma once
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


using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;
using namespace std;
using namespace cv;

// Function prototypes
Mat doBasicDemo(CString id);
Mat printQr(const QrCode &qr, int size);