//this is a class of opencv
//add #include "Opencvprocess.h" to where you want
//LoadMatImg(char* filename) to load a piture from computer
//monoinvert()  for invert picture if it is upside down
//MatToBmp can make you display a piture by bmp format
//BmpToMat can make you load a bmp to Mat format so that you can use it for a camera
//MatImgProcess can be edited to you want to deal with image
//m_RawMatImg stores the origin piture
#include "stdafx.h"
#include "Opencvprocess.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <math.h>
#include "include\\Preferences.h"
//#include "SetDlg.h"
using namespace std;

//COpencvprocess *cvproc = NULL;



int COpencvprocess::MatImgProcess(const cv::Mat& mat, cv::Mat& dst)
{
	//#pragma omp parallel
	{
		//多核心执行这个
		if (mat.empty())
			return INVALID;
		try

		{
			Mat gray, binnaryimg, color, grayforcont, graybilate;
			Point pend, pstart;
			vector<vector<Point>> contours;
			if (CV_8UC1 != mat.type())
			{
				color = mat.clone();
				cvtColor(mat, gray, CV_BGR2GRAY);
			}
			else
			{
				gray = mat.clone();
				cvtColor(mat, color, CV_GRAY2BGR);
			}
			image_mono_cesus(gray);

			//模糊去除噪点，降低误检率
			

			//GaussianBlur(gray, gray, Size(7, 7), 1, 1);
			//二值化
			//int threshold = ThresHold(graybilate, binnaryimg);

			cv::threshold(gray, grayforcont, g.ini.m_threshold2, 255, THRESH_BINARY);	//获取筷子的二值化

			cv::findContours(grayforcont, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

			int MaxID = getMaxIDandDeleteMin(contours, 1000);
			//if (0 != contours.size()) cout << contours[0].size() << endl;
			//用于找到白色最大连通区域
			if (-1 == MaxID)
			{
				dst = mat.clone();
				show_error(dst, "No object found!");
				return NOOBJECT;
			}


			//得到筷子的轮廓
			Mat white_mask(gray.size(), CV_8U, Scalar(0));
			cv::drawContours(white_mask, contours, MaxID, Scalar(255), CV_FILLED/*2*/);   // -1 表示所有轮廓
			cv::erode(white_mask, white_mask, Mat(), cv::Point(-1, -1), 1);

			Mat black_mask(gray.size(), CV_8U, Scalar(0));
			//adaptiveThreshold(gray, black_mask, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 105, 64);
			getAllBlackBlockinGray(black_mask, gray, white_mask);
		
			cv::bilateralFilter(black_mask, graybilate, g.ini.d, g.ini.sigmaColor, g.ini.sigmaSpace);

			black_mask = graybilate.clone();

		
			int threshold;
#ifdef TEST
			threshold = ThresHold(graybilate, binnaryimg);
			cout << "old version " << threshold << endl;
#endif
			if (g.ini.m_imageDealMode == 0)//OTSU
			{
				threshold = getOtsuByMask(graybilate, white_mask) + g.ini.m_idFixThreshold;
#ifdef TEST
				cout << "new version " << threshold << endl;
#endif
				if (threshold > 254) threshold = 254;
				else if (threshold < 0) threshold = 1;
			}
			else
			{
				threshold = g.ini.m_idFixThreshold;
				if (threshold > 254) threshold = 254;
				else if (threshold < 0) threshold = 1;
			}

			cv::adaptiveThreshold(black_mask, black_mask, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, g.ini.m_blocksize, (double)threshold / 7);
			dst = black_mask.clone();
			//m_ResultMatImg = black_mask.clone();
			//分别找到两张图的重心
			cv::Point pt;
			getWidthAndHeight(white_mask, pt);
			float width = pt.x;
			float height = pt.y;
			float ratio = width / height;
			if (2.0 < ratio || ratio < 0.5)
			{
				show_error(dst, "Obj Ratio Outrange");
				return NODOTS;
			}
			find_gravity_center(white_mask, pstart);
			find_gravity_center(black_mask, pend);

			if (pend.x == 0 && pend.y == 0)
			{
				show_error(dst, "There are no dots!");
				return NODOTS;
			}

			int xlen = iabs(pend.x - pstart.x);
			int ylen = iabs(pend.y - pstart.y);

			if (xlen > ylen)
			{
				pend.y = pstart.y;
				if (pend.x > pstart.x)
				{
					pend.x = pstart.x + 150;
				}
				else
				{
					pend.x = pstart.x - 150;
				}
			}
			else
			{
				pend.x = pstart.x;
				if (pend.y > pstart.y)
				{
					pend.y = pstart.y + 150;
				}
				else
				{
					pend.y = pstart.y - 150;
				}
			}

			cvtColor(dst, dst, CV_GRAY2BGR);
			drawArrow(dst, pstart, pend, 30, 10, Scalar(250, 250, 0), 2, CV_AA);

			switch (g.ini.m_direction)
			{
			case UP:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return UP;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return DOWN;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return LEFT;
				}
				else
				{
					return RIGHT;
				}
				break;
			case DOWN:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return DOWN;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return UP;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return RIGHT;
				}
				else
				{
					return LEFT;
				}
				break;
			case LEFT:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return RIGHT;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return LEFT;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return UP;
				}
				else
				{
					return DOWN;
				}
				break;
			case RIGHT:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return LEFT;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return RIGHT;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return DOWN;
				}
				else
				{
					return UP;
				}
				break;
			default:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return UP;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return DOWN;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return LEFT;
				}
				else
				{
					return RIGHT;
				}
				break;
			}
			return INVALID;

			return true;
		}
		catch (...)
		{
			AfxMessageBox(L"图像处理错误");
			return false;
		}
		return false;
	}
}









void COpencvprocess::getWidthAndHeight(cv::Mat mat, cv::Point &pt)
{
	cv::Rect rt;
	pt.x = pt.y = 0;
	rt.x = mat.cols;
	rt.y = mat.rows;
	rt.width = 0;
	rt.height = 0;
	size_t height = mat.rows;
	size_t width = mat.cols;
	size_t widthStep = mat.step;
	uchar* mdata = mat.data;
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (mdata[w])
			{
				if (w<rt.x) rt.x = w;
				if (h<rt.y) rt.y = h;
				if (h >rt.height) rt.height = h;
				if (w >rt.width) rt.width = w;
			}
		}
		mdata += widthStep;
	}
	pt.y = rt.height - rt.y;
	pt.x = rt.width - rt.x;
}

int COpencvprocess::getOtsuByMask(const Mat& src, const Mat& mask)
{

	try {
		const int GrayScale = 256;
		const int width = src.cols;
		const int height = src.rows;
		int pixelCount[GrayScale] = { 0 };
		float pixelPro[GrayScale] = { 0 };
		int i, j, pixelSum = width * height, threshold = 0;
		pixelSum = 1;//去除bug,所以不等于0
		uchar* idata = src.data;  //指向像素数据的指针
		uchar* mdata = mask.data;

		//统计灰度级中每个像素在整幅图像中的个数  
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (mask.data[i * width + j])
				{
					pixelCount[(int)idata[i * width + j]]++;  //将像素值作为计数数组的下标
					pixelSum++;
				}
			}
		}


		//计算每个像素在整幅图像中的比例  
		float maxPro = 0.0;
		int kk = 0;
		for (i = 0; i < GrayScale; i++)
		{
			pixelPro[i] = (float)pixelCount[i] / pixelSum;
			if (pixelPro[i] > maxPro)
			{
				maxPro = pixelPro[i];
				kk = i;
			}
		}


		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
		for (i = 0; i < GrayScale; i++)     // i作为阈值
		{
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
			for (j = 0; j < GrayScale; j++)
			{
				if (j <= i)   //背景部分  
				{
					w0 += pixelPro[j];
					u0tmp += j * pixelPro[j];
				}
				else   //前景部分  
				{
					w1 += pixelPro[j];
					u1tmp += j * pixelPro[j];
				}
			}
			u0 = u0tmp / w0;
			u1 = u1tmp / w1;
			u = u0tmp + u1tmp;
			deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
			if (deltaTmp > deltaMax)
			{
				deltaMax = deltaTmp;
				threshold = i;
			}
		}

		//delete[] idata;
		return threshold;
	}
	catch (...) {
		AfxMessageBox(L"未知错误");
	}

}

COpencvprocess::COpencvprocess()
: ImageData(NULL)
{
	float n = 0.0, delta_ang = (float)CV_PI / 180;
	for (int i = 0; i < 180; i++,n += delta_ang)
	{
		m_default_ang_cos_sin[i * 2] =		(float)cos(delta_ang);
		m_default_ang_cos_sin[i * 2 + 1] =	(float)sin(delta_ang);
	}

	_RED.R = 255;
	_RED.G = 0;
	_RED.B = 0;
	_GREEN.R = 0;
	_GREEN.G = 255;
	_GREEN.B = 0;
	_BLUE.R = 0;
	_BLUE.G = 0;
	_BLUE.B = 255;
	_YELLOW.R = 255;
	_YELLOW.G = 255;
	_YELLOW.B = 0;

	MotionInfoClear.tpye1.move_wise = 0;
	MotionInfoClear.tpye1.move_angle = 0;
	MotionInfoClear.tpye2.arrayTo = 0;
	MotionInfoClear.Valid = 0;
	motion_info = MotionInfoClear;

	for (int i = 0; i < 256; i++)
		m_ColorCesus[0][i] = i;
}


COpencvprocess::~COpencvprocess()
{

}



void COpencvprocess::GetROI(tagImgROI &ROI)
{
	if (0 == ROI.height || 0 == ROI.height) return;
	ImgROI = ROI;
}


void COpencvprocess::SwapValue(char &a, char &b)
{
	a = a^b;
	b = a^b;
	a = a^b;
}


// 获取映射感兴趣区域
BOOL COpencvprocess::GetIplROI(IplImage* Ipl)
{
	if (0 == MFCROI.width || 0 == MFCROI.height || NULL == Ipl){
		ImgROI.xpos = ImgROI.ypos = 0;
		ImgROI.width = Ipl->width; ImgROI.height = Ipl->height;
		return 0;
	}
	ImgROI.xpos = (int)((1.0 * MFCROI.xpos *  Ipl->width) / SHOWIMGPOS.width);
	ImgROI.ypos = (int)((1.0 * MFCROI.ypos *  Ipl->height) / SHOWIMGPOS.height);
	ImgROI.width = (int)(((1.0 * (MFCROI.xpos + MFCROI.width)  *  (Ipl->width)) / SHOWIMGPOS.width) - ImgROI.xpos);
	ImgROI.height = (int)(((1.0 * (MFCROI.ypos + MFCROI.height) * (Ipl->height)) / SHOWIMGPOS.height) - ImgROI.ypos);

	return 1;
}

BOOL COpencvprocess::GetMatROI(const Mat& mat)
{
	if(mat.empty())
	{
		ImgROI.xpos = ImgROI.ypos = ImgROI.width = ImgROI.height = 0;
		return FALSE;
	}
	else if (0 == MFCROI.width || 0 == MFCROI.height) {
		ImgROI.xpos = ImgROI.ypos = 0;
		ImgROI.width = mat.cols; ImgROI.height = mat.rows;
		return FALSE;
	}

	ImgROI.xpos = (int)((1.0 * MFCROI.xpos *  mat.cols) / SHOWIMGPOS.width);
	ImgROI.ypos = (int)((1.0 * MFCROI.ypos *  mat.rows) / SHOWIMGPOS.height);
	ImgROI.width = (int)(((1.0 * (MFCROI.xpos + MFCROI.width)  *  (mat.cols)) / SHOWIMGPOS.width) - ImgROI.xpos);
	ImgROI.height = (int)(((1.0 * (MFCROI.ypos + MFCROI.height) * (mat.rows)) / SHOWIMGPOS.height) - ImgROI.ypos);

	return TRUE;
}


inline void COpencvprocess::swapValue(uchar& a, uchar& b)
{	
	a = a^b;
	b = a^b;
	a = a^b;
}

//如果是windows下面读取图片，图片会颠倒，这个函数就是为了防止颠倒
void COpencvprocess::MonoInvert(const Mat & mat)
{
	int widthStep = mat.step;
	int rows = mat.rows - 1;
	for (int r = 0; r < rows; r++,rows--)
		for (int step = 0; step<widthStep; step++)
		{
			swapValue(mat.data[r*widthStep + step], mat.data[rows*widthStep + step]);
		}
}


// 统计图片直方图
int COpencvprocess::image_color_cesus(const Mat& mat, int channel)
{
	if (mat.empty())
		return -1;

	int MaxStatic = 0, widthStep = mat.step, rows = mat.rows, cols = mat.cols, tmp;// nChannel, depth;
	//初始化
	for (int n = 0; n < 256; m_ColorCesus[channel][n] = 0, n++);
	//统计数据
	for (int r = 0; r < rows; r++, rows--)
		for (int step = 0; step<cols; step++)
		{
		tmp = m_ColorCesus[channel][mat.data[r*widthStep + step*channel + channel]] ++;
			if (tmp > MaxStatic) MaxStatic = tmp;
		}
	
	return MaxStatic;
}


//获取大津二值化的数值
int COpencvprocess::otsuThreshold(const Mat& mat)
{
	try {
		const int GrayScale = 256;
		const int width = mat.cols;
		const int height = mat.rows;
		int pixelCount[GrayScale] = { 0 };
		float pixelPro[GrayScale] = { 0 };
		int i, j, pixelSum = width * height, threshold = 0;
		uchar* idata = mat.data;  //指向像素数据的指针

												//统计灰度级中每个像素在整幅图像中的个数  
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				pixelCount[(int)idata[i * width + j]]++;  //将像素值作为计数数组的下标
			}
		}

		//计算每个像素在整幅图像中的比例  
		float maxPro = 0.0;
		int kk = 0;
		for (i = 0; i < GrayScale; i++)
		{
			pixelPro[i] = (float)pixelCount[i] / pixelSum;
			if (pixelPro[i] > maxPro)
			{
				maxPro = pixelPro[i];
				kk = i;
			}
		}


		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
		for (i = 0; i < GrayScale; i++)     // i作为阈值
		{
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
			for (j = 0; j < GrayScale; j++)
			{
				if (j <= i)   //背景部分  
				{
					w0 += pixelPro[j];
					u0tmp += j * pixelPro[j];
				}
				else   //前景部分  
				{
					w1 += pixelPro[j];
					u1tmp += j * pixelPro[j];
				}
			}
			u0 = u0tmp / w0;
			u1 = u1tmp / w1;
			u = u0tmp + u1tmp;
			deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
			if (deltaTmp > deltaMax)
			{
				deltaMax = deltaTmp;
				threshold = i;
			}
		}

		//delete[] idata;
		return threshold;
	}
	catch (...) {
		AfxMessageBox(L"未知错误");
	}
}


int COpencvprocess::otsubyROI(const Mat& mat)
{
	try{
		const int	GrayScale	= 256;
		const int	width		= ImgROI.width;
		const int	height		= ImgROI.height;
		int			pixelCount[GrayScale]	= { 0 };
		float		pixelPro[GrayScale]		= { 0 };
		int			i, j, pixelSum = width * height, threshold = 0;
		uchar* idata = mat.data;  //指向像素数据的指针

		//统计灰度级中每个像素在整幅图像中的个数  
		for (i = ImgROI.ypos; i < (height + ImgROI.ypos); i++)
		for (j = ImgROI.xpos; j < (width + ImgROI.xpos); j++)
			//pixelCount[(int)idata[(mat.rows - i - 1)* width + j]]++;  //将像素值作为计数数组的下标
			pixelCount[(int)idata[i* width + j]]++;
		//计算每个像素在整幅图像中的比例  
		float maxPro = 0.0;
		int kk = 0;
		for (i = 0; i < GrayScale; i++)
		{
			pixelPro[i] = (float)pixelCount[i] / pixelSum;
			if (pixelPro[i] > maxPro)
			{
				maxPro = pixelPro[i];
				kk = i;
			}
		}


		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
		for (i = 0; i < GrayScale; i++)     // i作为阈值
		{
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
			for (j = 0; j < GrayScale; j++)
			{
				if (j <= i)   //背景部分  
				{
					w0 += pixelPro[j];
					u0tmp += j * pixelPro[j];
				}
				else   //前景部分  
				{
					w1 += pixelPro[j];
					u1tmp += j * pixelPro[j];
				}
			}
			u0 = u0tmp / w0;
			u1 = u1tmp / w1;
			u = u0tmp + u1tmp;
			deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
			if (deltaTmp > deltaMax)
			{
				deltaMax = deltaTmp;
				threshold = i;
			}
		}

		//delete[] idata;
		return threshold;
	}
	catch (...){
		AfxMessageBox(L"未知错误");
	}
	
}


void COpencvprocess::Threshold(const Mat& mat, int threshold)
{
	if (0 == ImgROI.height || 0 == ImgROI.width)
	{
		ImgROI.ypos = ImgROI.xpos = 0; ImgROI.height = mat.rows; ImgROI.width = mat.cols;
	}

	if (threshold > 255) threshold = 255;
	else if (threshold < 0) threshold = 1;

	int ypos = ImgROI.ypos;
	int height = ImgROI.ypos + ImgROI.height;
	int xpos = ImgROI.xpos;
	int width = xpos + ImgROI.width;

	try{
	
	for (int h = ypos; h < height; ++h)
	for (int w = xpos; w < width; ++w)

		if (threshold < mat.data[h*mat.step + w])
			mat.data[h*mat.step + w] = 255;
		else
			mat.data[h*mat.step + w] = 0;
	}
	catch (...){
		AfxMessageBox(L"error location is Threshold(IplImage* src, unsigned char threshold)");

	}
}


// 为了将图片从mat格式转化成bmp格式，需要创建的格式头信息在这里
LPBITMAPINFO COpencvprocess::CreateMapInfo(Mat mat)
{
	BITMAPINFOHEADER BIH = { 40, 1, 1, 1, 8, 0, 0, 0, 0, 0, 0 };
	LPBITMAPINFO lpBmi;
	int          wid, hei, bits, colors, i, depth,  channels;
	RGBQUAD  ColorTab[256];
	wid = mat.cols;     hei = mat.rows ;
	get_Mat_depth_and_channels(mat, depth, channels);
	bits = depth * channels;
	if (bits>8) colors = 0;
	else colors = 1 << bits;
	lpBmi = (LPBITMAPINFO)malloc(40 + 4 * colors);
	BIH.biWidth = wid;     BIH.biHeight = hei;
	BIH.biBitCount = (BYTE)bits;
	memcpy(lpBmi, &BIH, 40);                   //  复制位图信息头
	if (bits == 8) {                           //  256 色位图
		for (i = 0; i<256; i++)  {                //  设置灰阶调色板
			ColorTab[i].rgbRed = ColorTab[i].rgbGreen = ColorTab[i].rgbBlue = (BYTE)i;
		}
		memcpy(lpBmi->bmiColors, ColorTab, 1024);
	}
	return(lpBmi);
}


// 将bmp格式转成Mat格式 
cv::Mat COpencvprocess::BmpToMat(BITMAPINFO* bmpinfo, BYTE* imageData)
{
	int size = bmpinfo->bmiHeader.biWidth*bmpinfo->bmiHeader.biHeight;
	cv::Mat m(Size(bmpinfo->bmiHeader.biWidth, bmpinfo->bmiHeader.biHeight), CV_8UC1);
	memcpy(m.data, (uchar*)imageData, size);
	return m;
	//if (!m_RawMatImg.empty()) m_RawMatImg.release();
	//m_RawMatImg.create(Size(bmpinfo->bmiHeader.biWidth, bmpinfo->bmiHeader.biHeight), CV_8UC1);
	//memcpy(m_RawMatImg.data, (uchar*)imageData, size);
	return Mat();
}

//载入图片到Mat格式里
Mat COpencvprocess::LoadMatImg(char* filename)
{
	m_RawMatImg = imread(filename, -1);

	if (m_RawMatImg.empty()) 
	{
		AfxMessageBox(L"找不到你所选择的图片哦！");
		return m_RawMatImg;
	}
	ImgROI.height = 0;
	ImgROI.width = 0;
	//get_Mat_depth_and_channels(m_RawMatImg, ImgProperty.depth,ImgProperty.nChannels);
	return m_RawMatImg;
}

//为了获取图像深度和通道数
bool COpencvprocess::get_Mat_depth_and_channels(const Mat& mat, int& depth, int& channels)
{
	switch (mat.type())
	{
	case CV_8UC1:	depth = 8; channels = 1; return true;
	case CV_8UC2:	depth = 8; channels = 2; return true;
	case CV_8UC3:	depth = 8; channels = 3; return true;
	case CV_8UC4:	depth = 8; channels = 4; return true;
	case CV_8SC1:	depth = 8; channels = 1; return true;
	case CV_8SC2:	depth = 8; channels = 2; return true;
	case CV_8SC3:	depth = 8; channels = 3; return true;
	case CV_8SC4:	depth = 8; channels = 4; return true;
	case CV_16UC1:	depth = 16; channels = 1; return true;
	case CV_16UC2:	depth = 16; channels = 2; return true;
	case CV_16UC3:	depth = 16; channels = 3; return true;
	case CV_16UC4:	depth = 16; channels = 4; return true;
	case CV_16SC1:	depth = 16; channels = 1; return true;
	case CV_16SC2:	depth = 16; channels = 2; return true;
	case CV_16SC3:	depth = 16; channels = 3; return true;
	case CV_16SC4:	depth = 16; channels = 4; return true;
	case CV_32SC1:	depth = 32; channels = 1; return true;
	case CV_32SC2:	depth = 32; channels = 2; return true;
	case CV_32SC3:	depth = 32; channels = 3; return true;
	case CV_32SC4:	depth = 32; channels = 4; return true;
	case CV_32FC1:	depth = -32; channels = 1; return true;
	case CV_32FC2:	depth = -32; channels = 2; return true;
	case CV_32FC3:	depth = -32; channels = 3; return true;
	case CV_32FC4:	depth = -32; channels = 4; return true;
	case CV_64FC1:	depth = -64; channels = 1; return true;
	case CV_64FC2:	depth = -64; channels = 2; return true;
	case CV_64FC3:	depth = -64; channels = 3; return true;
	case CV_64FC4:	depth = -64; channels = 4; return true;
	default:;
		return false;
	}
}


float COpencvprocess::IvtSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating VALUE 
	i = 0x5f375a86 - (i >> 1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x); // Newton step, repeating increases accuracy
	return x;
}

float COpencvprocess::Sqrt(float x)
{
	return 1 / IvtSqrt(x);
}

void COpencvprocess::InitMat(Mat& mat, int t)
{
	for (int i = 0; i<mat.rows; i++)
		for (int j = 0; j<mat.cols; j++)
			mat.at<int>(i, j) = t;
}

int COpencvprocess::hough_lines(const Mat& mat, vector<Vec4i>& lines, double rho, double theta, int threshold, double  minLineLength, double  maxLineGap)
{
	if (1.0 != rho || 1 != mat.channels()) return ERROR;
	/*
		概率霍夫变换的步骤：
		1。从点集中随机选取一个像素点，对应的累加器加1
		2。从点集中删除该点
		3。更新累加器
		4。若更新之后的累加器值大于阈值（检测到直线），则删除集合中位于该直线上的所有点
	*/
	int widthStep = mat.step;										//	图像步长
	int h, w;														//	局部变量，分别是图像高度和宽度
	int xlen, ylen;													//	用于计算图像上的欧氏距离,x方向长度和y方向上的长度
	double lineLength;												//	存储计算的欧氏距离

	const int width = mat.cols;
	const int height = mat.rows;
	const int numangle = 180;										//  定义角度数
	const int numrho = ((width + height) * 2 + 1);					//  定义hough图像中的长度;

	double ang;
	int r, n, count;
	int pt[2];														//  表示一个点,其中pt[0]表示x坐标,pt[1]表示y坐标
	
	double deltatheta = theta;// / 180;
	float trigtab[numangle * 2];									//  用来存储 cos＆sin 0-180
	Vec4i point = { 0,0,0,0 };													
	const float* ttab;

	//important to free seq
	int *seq;														//	存取点集，所有非0点的坐标都在这里
	Mat Matmask(mat.rows, mat.cols, CV_8UC1);						//	映射mat中非零的点到这里
	Mat Mataccum(numangle, numrho, CV_32SC1);						//	用来存储计算hough的图像
	InitMat(Mataccum, 0);											//	将图像里所有的数值置零

	int line_end[2][2] = { { 0,0 },{ 0,0 } };						//  表示直线两端点的坐标,如line_end[0][1]表示第一个点的y坐标
	int* accum_data = new int[numangle*numrho];
	int* adata = accum_data;
		//(int*)malloc(numangle*numrho*sizeof(int));			//	蛋疼，应该指向	Mataccum的	用来存储计算hough的图像										//	半圆矩阵的指针
	//保存半个单位圆
	for (ang = 0, n = 0; n < numangle; ang += deltatheta, n++)		//	遍历180角度,deltatheta是精度
	{
		trigtab[n * 2] = (float)(cos(ang) * rho);					//	x = r*cos(theta) //就是存了半个单位圆
		trigtab[n * 2 + 1] = (float)(sin(ang) * rho);				//	y = r*sin(theta)
	}
	ttab = trigtab;

	//CString str;
	//str.Format(_T("the cos is %f and sin is %f "),*ttab, *(ttab+1));
	//AfxMessageBox(str);

	lines.clear();													//清除里面所存的点，不过stl并非真正的清除
												
	// stage 1. 收集所有的非零点，将它们存入数组中，并在mask中标记为1
	for (h = 0, count = 0; h < mat.rows; h++)
		for ( w = 0; w < widthStep; w++)
		{
			if (mat.data[h*widthStep + w])
			{
				Matmask.data[h*widthStep + w] = (uchar)1;
				count++;
			}
			else
				Matmask.data[h*widthStep + w] &= 0;
		}

	seq = new int[count * 2];// if count is equal to 100, then the max is 100 and array from 0 - 99

	//以下循环保存了所有非0点坐标的位置
	for (h = 0, count = 0; h < mat.rows; h++)
		for (w = 0; w < widthStep; w++)
		{
			if (mat.data[h*widthStep + w])
			{
				seq[count * 2]		=	w;
				seq[count * 2 + 1]	=	h;
				count++;											//	累加器更新
			}
		}
		//CString str;
		//str.Format(_T("width step %d"), widthStep);
		//AfxMessageBox(str);
		//Sleep(100);
	//count is 100

	for (; count > 0; count--)
	{
		// 选择随机点
		int idx = rand() % count;									//	从seq里选取一个点
		////////////////////////////////////////

		pt[0] = seq[idx * 2];
		pt[1] = seq[idx * 2 + 1];
		//int line_end[2][2] = { { 0,0 },{ 0,0 } };   //  表示直线两端点的坐标,如line_end[0][1]表示第一个点的y坐标

		//push back(point(1, 2, 3, 4))

		int* matdata = accum_data; //替代int* adata = accum->data.i;
		float a, b;


		int i, j, k, x0, y0, dx0, dy0, xflag;
		int good_line;
		const int shift = 16;
		

		int max_val = threshold - 1,max_n = 0;

		pt[0] = seq[idx * 2];
		pt[1] = seq[idx * 2 + 1];

		//将随机获得的坐标点赋值给i和j
		i = pt[1];
		j = pt[0];


		//sk add 排出掉已经检查的点
		//swapValuei(seq[idx * 2], seq[count * 2]);
		//swapValuei(seq[idx * 2 + 1], seq[count * 2 + 1]);

		//  通过重新赋值删除此点 
		pt[0] = seq[count * 2 - 2];
		pt[1] = seq[count * 2 - 1];

		// 检查此点是否已经被排除 (例如属于别的直线)
		if(!Matmask.data[i*width + j]) //替代if (!mdata0[i*width + j])
			continue;

		// 对此点逐行更新累加器, 看能否超过阈值
		for (n = 0; n < numangle; n++, matdata += numrho)
		//for (n = 0; n < numangle; n++, adata += numrho)
		{
			r = (int)floor(j * ttab[n * 2] + i * ttab[n * 2 + 1] + 0.5);//ttab为半单位圆	r = cos y sin x
			r += (numrho - 1) / 2;   //所有r都加上图像的行数和列数，让r的结果永远为正

			int val = ++matdata[r];
			if (max_val < val)
			{
				max_val = val;
				max_n = n;
			}
		}

		// 没有超过阈值，就舍弃，进行下一个点的运算
		if (max_val < threshold)
			continue;

		// 如果超过阈值，就从当前点沿各个方向寻找并提取线段
		a = -ttab[max_n * 2 + 1];									//  -sin(max_n)
		b = ttab[max_n * 2];										//  cos(max_n)
		x0 = j;
		y0 = i;
		if (fabs(a) > fabs(b))										//  在方向上要摒除斜率不近似垂直的线  
		{        
			xflag = 0;
			dy0 = b > 0 ? 1 : -1;
			dx0 = (int)floor(a*(1 << shift) / fabs(b) + 0.5);
			x0 = (x0 << shift) + (1 << (shift - 1));
			continue;
		}
		else
		{
			xflag = 0;
			dy0 = b > 0 ? 1 : -1;
			dx0 = (int)floor(a*(1 << shift) / fabs(b) + 0.5);
			x0 = (x0 << shift) + (1 << (shift - 1));
		}

		for (k = 0; k < 2; k++)
		{
			int gap = 0, x = x0, y = y0, dx = dx0, dy = dy0;

			if (k > 0)
				dx = -dx, dy = -dy;

			// 用固定步长沿此点的两个方向寻找直线上的点
			// 直至图像边届或出现过大的跳跃
			for (;; x += dx, y += dy)
			{
				uchar* mdata;
				int i1, j1;

				if (xflag)
				{
					j1 = x;
					i1 = y >> shift;
				}
				else
				{
					j1 = x >> shift;
					i1 = y;
				}

				if (j1 < 0 || j1 >= width || i1 < 0 || i1 >= height)
					break;

				mdata = Matmask.data;
				int base = i1*width + j1;

				// 对于非零点：
				//    升级线的端点,
				//    重令gap为0
				if (mdata[base])
				{
					gap = 0;
					line_end[k][1] = i1;					
					line_end[k][0] = j1;
				}
				else if (mdata[base + 1] || mdata[base - 1])
				{
					gap = 0;
					line_end[k][1] = i1;
					line_end[k][0] = j1;
				}
				else if (++gap > maxLineGap)								// 对于零点，要考虑双线或三线的共有点情况以得到更准确的线  
					break;	
			}
		}

		//  判断此线是否是满足长度lineLength的直线
			xlen = line_end[1][0] - line_end[0][0];
			ylen = line_end[1][1] - line_end[0][1];
			lineLength = xlen*xlen + ylen*ylen;
			good_line = lineLength > (minLineLength*minLineLength);

		//  重新遍历此线来:
		//      置线上的点mask为0
		//      对good_line来减去此线在累加器上的作用
		for (k = 0; k < 2; k++)
		{
			int x = x0, y = y0, dx = dx0, dy = dy0;

			if (k > 0)
				dx = -dx, dy = -dy;


			for (;; x += dx, y += dy)
			{
				uchar* mdata;
				int i1, j1;

				if (xflag)
				{
					j1 = x;
					i1 = y >> shift;
				}
				else
				{
					j1 = x >> shift;
					i1 = y;
				}

				mdata = Matmask.data + i1*width + j1;

				if (*mdata)
				{
					if (good_line)
					{
						adata = accum_data;
						for (n = 0; n < numangle; n++, adata += numrho)
						{
							r = (int)floor(j1 * ttab[n * 2] + i1 * ttab[n * 2 + 1] + 0.5);
							r += (numrho - 1) / 2;
							adata[r]--;
						}
						//	*mdata = 0;
					}
					*mdata = 0;
				}


				if (*(mdata - 1))
				{
					if (good_line)
					{
						*(mdata - 1) = 0;
					}
				}

				if (*(mdata + 1))
				{
					if (good_line)
					{
						*(mdata + 1) = 0;
					}
				}

				if (i1 == line_end[k][1] && j1 == line_end[k][0])
					break;
			}
		}



		//  把此线端点存入lines结构
		if (good_line /*&& /* fabs(line_end[0][1] - line_end[1][1]) / fabs(line_end[0][0] - line_end[1][0] + 0.5) >= 3*/)
		{
			point[0] = line_end[0][0];//x1
			point[1] = line_end[0][1];//y1
			point[2] = line_end[1][0];//x2
			point[3] = line_end[1][1];//y2
			lines.push_back(point);
		}

	}//for (; count > 0; count--)

	delete[] accum_data;
	delete[] seq;
	return TRUE;
}





float COpencvprocess::m_fabs(float &a)
{
	int i = *(int*)&a & 0x7FFFFFFF;
	return *(float*)&i;
}


void COpencvprocess::swapValuei(int& a, int& b)
{
	a = a^b;
	b = a^b;
	a = a^b;
}


// 从背景图片里提取掩码图片
bool COpencvprocess::get_mask_from_background(const Mat& mat)
{
	if (mat.empty() /*|| 0!=mat.type()*/)
		return false;

	int width = mat.cols;
	int height = mat.rows, depth, channels;
	const uchar threshold = 240;
	
	get_Mat_depth_and_channels(mat, depth, channels);

	if (1 != channels)
	{
		cvtColor(mat, m_BackGroundMatImg, CV_BGR2GRAY);
		cvtColor(mat, m_BackGroundMaskMatImg, CV_BGR2GRAY);
	}
	else
	{
		m_BackGroundMatImg = mat.clone();
		m_BackGroundMaskMatImg = mat.clone();
	}
	const int widthStep = m_BackGroundMatImg.step;
	//m_BackGroundMatImg = mat.clone();
	//m_BackGroundMaskMatImg = mat.clone();//尽管冗余了，但是可以取得图像尺寸
	for (int h = 0; h < height;h++)
	for (int w = 0; w < width; w++)
	{
		if (threshold > m_BackGroundMatImg.data[h*widthStep + w])
			m_BackGroundMaskMatImg.data[h*widthStep + w] &= 0x0;
		else
			m_BackGroundMaskMatImg.data[h*widthStep + w] |= 0xFF;
	}
	return true;
}


void COpencvprocess::delete_Mat_with_Mask(Mat& dst)
{
	int width = dst.cols;
	int height = dst.rows;
	int widthStep = dst.step;

	if (width != m_BackGroundMaskMatImg.cols || height != m_BackGroundMaskMatImg.rows || dst.empty() || m_BackGroundMaskMatImg.empty())
		return;

	for (int h = 0; h < height; h++)
	for (int w = 0; w < width; w++)
	{
		if (!m_BackGroundMaskMatImg.data[h*widthStep + w])
			dst.data[h*widthStep + w] |= 0xFF;
	}
}


void COpencvprocess::get_BackGroud_Mat_img(Mat& mat)
{
	m_BackGroundMatImg = mat.clone();
	if (m_BackGroundMatImg.empty())
	{
		AfxMessageBox(L"获取背景图片失败");
		return;
	}
}





void COpencvprocess::find_gravity_center(const Mat& mat, vector<V2i>& p)
{
	if (mat.empty()) return;
	p.clear();
	int width = mat.cols;
	const int height = mat.rows;
	const int widthStep = mat.step;
	uchar* idata = mat.data;
	long x = 0, y = 0, sum = 0;
	V2i tmp;
	tmp.x = 2;
	tmp.y = 2;
	for (int h = 0; h < height; h++)
	{ 
		for (int w = 0; w < widthStep; w++)
		{
			if (idata[w])
			{
				x += w;
				y += h;
				sum += 1;
			}
		}
		idata += widthStep;
	}
	p.push_back(tmp);
}

void COpencvprocess::find_gravity_center(const Mat& mat, V2i& p)
{
	if (mat.empty()) return;

	int width = mat.cols;
	const int height = mat.rows;
	const int widthStep = mat.step;
	uchar* idata = mat.data;
	long x = 0, y = 0, sum = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (255 == idata[w])
			{
				x += w;
				y += h;
				sum += 1;
			}
		}
		idata += widthStep;
	}
	cout << y << endl;
	p.x = x / sum;
	p.y = y / sum;
}

void COpencvprocess::find_gravity_center(const Mat& mat, Point &p)
{
	if (mat.empty()) return;
	int width = mat.cols;
	const int height = mat.rows;
	const int widthStep = mat.step;
	uchar* idata = mat.data;
	long x = 0, y = 0, sum = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (255 == idata[w])
			{
				x += w;
				y += h;
				sum += 1;
			}
		}
		idata += widthStep;
	}
	if (0 == sum){ p.x = 0; p.y = 0; return; }
	p.x =(int) (x / sum);
	p.y =(int) (y / sum);
}


void COpencvprocess::DrawLatticeOnMat(const Mat& mat, vector<V2i>& p)
{
	V2i tmp = p[0];
	int width = mat.cols;
	int height = mat.rows;
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
		{
			if (h == tmp.x|| w == tmp.y)
			{
				mat.data[h*(mat.step) + 3 * w] &= 0;
				mat.data[h*(mat.step) + 3 * w + 1] &= 0;
				mat.data[h*(mat.step) + 3 * w + 2] |= 255;
			}
		}
}


void COpencvprocess::CopyMatDataToMem(const Mat& mat)
{
	if (ImageData)
		free(ImageData);

	int height = mat.rows;
	int offset = 0 == mat.step % 4 ? 0 : 4 - mat.step%4;
	int size_i = (offset+mat.step) * mat.rows;
	ImageData = (uchar*)malloc(size_i);
	uchar *tmp = ImageData;
	uchar *idata = mat.data;

	for (int h = 0; h < height; h++)
		{
			memcpy(tmp, idata, mat.step);
			tmp = tmp + mat.step + offset;
			idata = idata + mat.step;
		}
}

void COpencvprocess::CopyMatDataToMem(const Mat& mat, uchar* ImageData)
{
	int height = mat.rows;
	int offset = 0 == mat.step % 4 ? 0 : 4 - mat.step % 4;
	int size_i = (offset + mat.step) * mat.rows;
	ImageData = (uchar*)malloc(size_i);
	uchar *tmp = ImageData;
	uchar *idata = mat.data;

	for (int h = 0; h < height; h++)
	{
		memcpy(tmp, idata, mat.step);
		tmp = tmp + mat.step + offset;
		idata = idata + mat.step;
	}
}


void COpencvprocess::MarkMaskImg(const Mat& mat, Mat& mask, int type)
{
	int width = mat.cols;
	int height = mat.rows;
	mask = mat.clone();
	if(type == 1)
	{
		for (int h = 0; h < height; h++)
			for (int w = 0; w < width; w++)
		{

			if (!mat.data[h*mat.step + w])
			{
				mask.data[h*mat.step + w] |= 0xFF;
			}
		}
	}
	else if (0 == type)
	{
		for (int h = 0; h < height; h++)
			for (int w = 0; w < width; w++)
			{
				if (mat.data[h*mat.step + w])
				{
					mask.data[h*mat.step + w] |= 0xFF;
				}
			}
	}
	else
		return;
}



int COpencvprocess::getAllBlockSquare(const Mat& mat, vector<AreaInfo>& Runs, vector<AreaInfoIndex>& InfoIndex)
{
	vector <int> rowRuns;
	AreaInfo Run;	
	Run.sign ^= Run.sign;
	AreaInfoIndex infoIndex;
	infoIndex.Square = 0,infoIndex.Signed = 0;
	vector<int> RunsDeleteRecord;
	Run.RunsSerial = -1;
	int widthStep = mat.step,h,w;
	const int height = mat.rows;
	const int width = mat.cols;
	const uchar* rowData = mat.data;
	const uchar white = 0xFF, black = 0x00;
	rowRuns.push_back(Run.RunsSerial+1);

	//step1 找到每一行的团
	for (h = 0; h < height; h++)
	{
		if (rowData[0])
		{
			Run.st = 0;
		}
		for (w = 1; w < width;w++)
		{
			if (white == rowData[w] && black == rowData[w - 1])
			{
				Run.st = w;
			}
			if  (w == (width - 1) && white == rowData[w])
			{
				Run.ed = w;
				Run.RusSubTo = ++Run.RunsSerial;
				Run.AreaSquare.isquare = Run.ed - Run.st + 1;
				Run.row = h;
				Runs.push_back(Run);
			}
			else if (white == rowData[w - 1] && black == rowData[w])
			{
				Run.ed = w - 1;
				Run.RusSubTo = ++Run.RunsSerial;
				Run.AreaSquare.isquare = Run.ed - Run.st + 1;
				Run.row = h;
				Runs.push_back(Run);
			}
		}
		rowRuns.push_back(Run.RunsSerial+1); //这个能找到每一行的第一个团在属于总团里的第几个
		rowData += widthStep;
	}

#if 1
	//step2 建立一个索引表，每一个表里的内容能够找到对应的团，第一次扫面只是找到了第一个块的区域而已
	int i1 = rowRuns[0], i2 = rowRuns[1];
	for (int k = i1; k < i2; k++)
	{
		infoIndex.index.clear();
		Runs[k].sign |= ADDED;
		infoIndex.Cloud = Runs[k].RusSubTo;
		infoIndex.index.push_back(Runs[k]);
		InfoIndex.push_back(infoIndex);
	}

	for (int h = 1; h < height;h++)
	{
		int index1 = rowRuns[h - 1]; //第一行，从左下角开始看
		int index2 = rowRuns[h];
		while (1)
		{
			if (index1 >= rowRuns[h]) 
			{
				for (int restindex2 = index2; restindex2 < rowRuns[h + 1]; restindex2++)
				{
					if (0 == (ISOLATE&Runs[restindex2].sign))
					{
						infoIndex.index.clear();
						Runs[restindex2].sign |= ADDED;
						infoIndex.Cloud = Runs[restindex2].RusSubTo;
						infoIndex.index.push_back(Runs[restindex2]);
						InfoIndex.push_back(infoIndex);
					}
				}
				break;
			}
			else if(index2 >= rowRuns[h + 1]) break;

			int st1 = Runs[index1].st, st2 = Runs[index2].st, ed1 = Runs[index1].ed, ed2 = Runs[index2].ed;
			if (isOverLap(st1, ed1, st2, ed2))
			{				
				Runs[index2].sign |= ISOLATE;
				if (ed1 >= ed2)
					index2++;
				else
					index1++;			
			}
			else if (st1 >= ed2)
			{
				if (0 == (ISOLATE&Runs[index2].sign))
				{
					infoIndex.index.clear();
					Runs[index2].sign |= ADDED;
					infoIndex.Cloud = Runs[index2].RusSubTo;
					infoIndex.index.push_back(Runs[index2]);
					InfoIndex.push_back(infoIndex);
				}
				index2++;
			}
			else if (st2 > ed1)
			{
				index1++;
			}
		}		
	}
#endif
#if 1
	//最后的合并
	for (int h = 1; h < height;h++)
	{
		int index1 = rowRuns[h - 1]; //第一行，从左下角开始看
		int index2 = rowRuns[h];
		while (1)
		{
			if (index1 >= rowRuns[h] && index2 >= rowRuns[h + 1]) break;

			int st1 = Runs[index1].st, st2 = Runs[index2].st, ed1 = Runs[index1].ed, ed2 = Runs[index2].ed;
			if (isOverLap(st1, ed1, st2, ed2))
			{
				int Cloud1 = isAreadyHave(Runs[index1].RusSubTo, InfoIndex);
				int Cloud2 = isAreadyHave(Runs[index2].RusSubTo, InfoIndex);
				if (Runs[index2].RusSubTo >= Runs[index1].RusSubTo)
				{
					if (NOEXIST != Cloud1 && NOEXIST!= Cloud2) //两个都存在
					{
						vector_emerge(InfoIndex, Cloud1, Cloud2);
						Runs[index2].RusSubTo = Runs[index1].RusSubTo;
				
					}
					else if (NOEXIST != Cloud1 && NOEXIST == Cloud2) //cloud2不存在
					{
						InfoIndex[Cloud1].index.push_back(Runs[index2]);
						Runs[index2].RusSubTo = Runs[index1].RusSubTo;
						Runs[index2].sign |= ADDED;
				
					}
					else if (NOEXIST == Cloud1 && NOEXIST != Cloud2)//cloud1不存在
					{
						AfxMessageBox(L"eat shit");
					}
					else//两个都不存在
					{
						CString str;
						str.Format(_T("to %d and %d"), Runs[index1].RusSubTo, Runs[index2].RusSubTo);
						AfxMessageBox(str);
					}
				}
				else
				{


					if (NOEXIST == Cloud2 && NOEXIST == Cloud1)
					{

					}
		

				}

				if (ed1 >= ed2 && (index2 < rowRuns[h + 1]))
					index2++;
				else if (index1 < rowRuns[h])
					index1++;
				else
					index2++;

			}
			else if (st1 >= ed2)
			{
				index2++;
			}
			else
			{
				index1++;
			}
		}
	}
#endif
	//step3 将每个团的面积累加到对应的连通区域里面
	for (size_t m = 0; m < InfoIndex.size();m++)
		for (size_t n = 0;n < InfoIndex[m].index.size();n++)
		{
			InfoIndex[m].Square += InfoIndex[m].index[n].AreaSquare.isquare;
		}

	return 1;
}
//合并vector信息
void COpencvprocess::vector_emerge(vector<AreaInfoIndex>& vec, int inserted, int emerged)
{
	for (size_t i = 0;i < vec[emerged].index.size(); i++)
	{
		vec[inserted].index.insert(vec[inserted].index.begin(), vec[emerged].index.begin(), vec[emerged].index.end());
	}
	vec.erase(vec.begin() + emerged);
	return;
}

//判断是否有重叠
bool COpencvprocess::isOverLap(int& a, int& b, int& c, int& d)
{
	return ((d-a)*(c-b) <= 0);
}

void COpencvprocess::signBlockArea(Mat& mat, vector<AreaInfo>& Runs, vector<AreaInfoIndex>& InfoIndex, int maxarea, int minarea, float ratio,const COLOR& color)
{
	int widthStep = mat.step;
	uchar* idata = mat.data;
	for (size_t i = 0; i < InfoIndex.size(); i++)
	{
	//	if ((InfoIndex[i].Square>minarea) && (InfoIndex[i].Square < maxarea))
		{
			for (unsigned int j = 0; j < InfoIndex[i].index.size(); j++)
			{	
				//找到了表指向的团序列
				int RunsIt = InfoIndex[i].index[j].RunsSerial;
				int row = Runs[RunsIt].row;
				int st = Runs[RunsIt].st;
				int ed = Runs[RunsIt].ed;
				for (int point = st; point <= ed; point++)
				{
					idata[row*widthStep + 3*point	 ] = color.B;
					idata[row*widthStep + 3*point + 1] = color.G;
					idata[row*widthStep + 3*point + 2] = color.R;
				}
				
			}
		}

	};

}


int COpencvprocess::isAreadyHave(int Cloud, vector<AreaInfoIndex>& InfoIndex)
{
	int CloudSerial = NOEXIST;
	for (size_t i = 0;i < InfoIndex.size();i++)
	{
		if(InfoIndex[i].Cloud == Cloud)
		{
			CloudSerial = i;
			break;
		}
	}
	return CloudSerial;
}



void COpencvprocess::getSizeContours(vector<vector<Point>> &contours, size_t cmin, size_t cmax)
{
	if (cmax <= cmin) return;

	vector<vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end())
	{
		if ((itc->size()) < cmin || (itc->size()) > cmax)
		{
			itc = contours.erase(itc);
		}
		else ++itc;
	}
}


int COpencvprocess::getMaxIDandDeleteMin(vector<vector<Point>> &contours,size_t cmin)
{
	vector<vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end())
	{
		if ((itc->size()) < cmin)
		{
			itc = contours.erase(itc);
		}
		else ++itc;
	}

	int MaxID = 0;
	if (0 == contours.size()) return -1;
	size_t square = contours[0].size();
	
	for (size_t i = 0; i < contours.size(); i++)
	{
		size_t t = contours[i].size();
		if(t >= square)
		{
			square = t;
			MaxID = i;
		}
	}
	return MaxID;
}

void COpencvprocess::getAllBlackBlockinGray(Mat& black_mask, const Mat& gray, const Mat& white_mask)
{

	if (CV_8UC1 != black_mask.type() || CV_8UC1 != gray.type() || CV_8UC1 != white_mask.type())
		return;
	const int widthStep = black_mask.step;
	const int height = black_mask.rows;
	uchar* bdata = black_mask.data;
	uchar* gdata = gray.data;
	uchar* wdata = white_mask.data;

	for (int h = 0; h < height; h++)
	{	
		for (int w = 0; w < widthStep; w++)
			{
				if (wdata[w])
				//if (gdata[w] && wdata[w])
				{
					bdata[w] = gdata[w];
				}
				else
				{
					bdata[w] |= 0xFF;
				}
			}
		bdata += widthStep;
		gdata += widthStep;
		wdata += widthStep;
	}
};



void COpencvprocess::drawArrow(Mat& img, Point pStart, Point pEnd, int len, int alpha, Scalar& color, int thickness, int lineType)
{
	const double PI = 3.1415926;
	     Point arrow;
	     //计算 θ 角（最简单的一种情况在下面图示中已经展示，关键在于 atan2 函数，详情见下面）   
	     double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));
	     line(img, pStart, pEnd, color, thickness, lineType);
	     //计算箭角边的另一端的端点位置（上面的还是下面的要看箭头的指向，也就是pStart和pEnd的位置） 
	     arrow.x = (int)( pEnd.x + len * cos(angle + PI * alpha / 180));
	     arrow.y = (int)( pEnd.y + len * sin(angle + PI * alpha / 180));
	     line(img, pEnd, arrow, color, thickness, lineType);
		 arrow.x = (int)(pEnd.x + len * cos(angle - PI * alpha / 180));
		 arrow.y = (int)(pEnd.y + len * sin(angle - PI * alpha / 180));
	     line(img, pEnd, arrow, color, thickness, lineType);
}

int COpencvprocess::iabs(int a)
{
	if (a >= 0)
		return a;
	else
		return(0 - a);
}


bool COpencvprocess::show_error(Mat& mat, char* str)
{
	if(CV_8UC1 == mat.type())
		cvtColor(mat, mat, CV_GRAY2BGR);
	ColorInvert(mat);
	//Mat color(640, 480, CV_8UC3, Scalar(10, 10, 10));
	Point pstart;
	pstart.x = 10;
	pstart.y = 50;
	putText(mat, str, pstart, 1, 2, Scalar(64, 64, 255), 2, 8, false);
	ColorInvert(mat);
	return true;
}


int COpencvprocess::ThresHold(Mat& mat, Mat& dst)
{
	if (mat.empty())
		return -1;
	if (g.ini.m_imageDealMode == 0)//OTSU
	{
		int thrd = otsuThreshold(mat) + g.ini.m_idFixThreshold;// +20;//+80;
		if (thrd > 254) thrd = 254;
		else if (thrd < 0) thrd = 1;

		threshold(mat, dst, thrd, 255, THRESH_BINARY);
	
		return thrd;
	}
	else if (g.ini.m_imageDealMode == 1)
	{
		int blockSize = 25;
		//模糊去除噪点，降低误检率
		if (g.ini.m_idFixThreshold > 254)
			g.ini.m_idFixThreshold = 254;
		else if (g.ini.m_idFixThreshold<1)
			g.ini.m_idFixThreshold = 1;
		//adaptiveThreshold(mat, dst, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, ini->m_idFixThreshold);
		threshold(mat, dst, g.ini.m_idFixThreshold, 255, THRESH_BINARY);
	

		return g.ini.m_idFixThreshold;
	}

	return -1;
}


void COpencvprocess::DeleteCenterCircle(Mat& mat, const Point& pt, unsigned int radius)
{
	if (CV_8UC1 != mat.type())
		return;
//	Mat tmp(mat.size(), CV_8U, Scalar(0));
	const int widthStep = mat.step;
	const int height = mat.rows;
	uchar* sdata = mat.data;
	//uchar* ddata = tmp.data;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (((h - pt.y)*(h - pt.y) + (w - pt.x)*(w - pt.x))<radius*radius)
			{
				sdata[w] &= 0x00;
			}
		}
		sdata += widthStep;

	}
}


void COpencvprocess::exAdaptiveThreshold(const Mat& src, Mat& dst, int ThresHoldFixed, int BlockRange, int type)
{
	//图像长宽不是块尺寸成倍大小的问题，所以在图像的区域右侧和下侧没有办法达到BlockRange宽度
	const int GrayScale = 256;
	dst = src.clone();
	const int num_cols = (src.cols-1) / BlockRange + 1;
	const int num_rows = (src.rows-1) / BlockRange + 1;

	uchar* idata = src.data;  //指向像素数据的指针
	uchar* ddata = dst.data;


	for (int rows_index = 0; rows_index < num_rows; rows_index += 1) //行指针
	{
		for (int cols_index = 0; cols_index < num_cols; cols_index += 1)//列指针
		{
			int pixelCount[GrayScale] = { 0 };
			float pixelPro[GrayScale] = { 0 };
			int i, j, pixelSum = BlockRange*BlockRange, threshold = 0;

			//统计灰度级中每个像素在整幅图像中的个数  
			for (int i = rows_index*BlockRange; i < (rows_index + BlockRange); i++)//被分出的每个区域进行大津二值化法
			{
				for (int j = cols_index*BlockRange; j < (cols_index + BlockRange); j++)

				{


					pixelCount[(int)idata[i * src.step + j]]++;  //将像素值作为计数数组的下标
				}
			}

			//计算每个像素在整幅图像中的比例  
			float maxPro = 0.0;
			int kk = 0;
			for (i = 0; i < GrayScale; i++)
			{
				pixelPro[i] = (float)pixelCount[i] / pixelSum;
				if (pixelPro[i] > maxPro)
				{
					maxPro = pixelPro[i];
					kk = i;
				}
			}

			//遍历灰度级[0,255]  
			float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
			for (i = 0; i < GrayScale; i++)     // i作为阈值
			{
				w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
				for (j = 0; j < GrayScale; j++)
				{
					if (j <= i)   //背景部分  
					{
						w0 += pixelPro[j];
						u0tmp += j * pixelPro[j];
					}
					else   //前景部分  
					{
						w1 += pixelPro[j];
						u1tmp += j * pixelPro[j];
					}
				}
				u0 = u0tmp / w0;
				u1 = u1tmp / w1;
				u = u0tmp + u1tmp;
				deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
				if (deltaTmp > deltaMax)
				{
					deltaMax = deltaTmp;
					threshold = i;
				}
			}


			//二值化
			if (CV_THRESH_BINARY_INV == type)
			{
				for (int i = rows_index; i < (rows_index + BlockRange); i++)//被分出的每个区域进行大津二值化法
				{
					for (int j = cols_index; j < (cols_index + BlockRange); j++)
					{
						if (ddata[i * src.step + j] < i) idata[i * src.step + j] &= 0x00;
						else ddata[i * src.step + j] |= 0xFF;
					}
				}

			}

		}
	}



	
}


void COpencvprocess::getDirectImage(const Mat& src, const Mat& white_mask, Mat& dst, Point& pt, int direct)
{
	if (CV_8UC1 != src.type())
		return;
	dst = src.clone();
	const int widthStep = src.step;
	const int height = src.rows;
	uchar* sdata = src.data;
	uchar* ddata = dst.data;

	switch (direct)
	{
	case UP:
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < widthStep; w++)
			{
				if (h > pt.y)
				{
					if (!sdata[w])
						ddata[w] |= 0xFF;
					else
						ddata[w] &= 0x00;
				}
				else
					ddata[w] |= 0xFF;

			}
			sdata += widthStep;
			ddata += widthStep;
		}
		break;
	case DOWN:
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < widthStep; w++)
			{
				if (h <= pt.y)
				{
					if (!sdata[w])
						ddata[w] |= 0xFF;
					else
						ddata[w] &= 0x00;
				}
				else
					ddata[w] |= 0xFF;
			}
			sdata += widthStep;
			ddata += widthStep;
		}
		//	dst = l_dst.clone(); 
		break;
	case LEFT:
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < widthStep; w++)
			{
				if (w < pt.x)
				{
					if (!sdata[w])
						ddata[w] |= 0xFF;
					else
						ddata[w] &= 0x00;
				}
				else
					ddata[w] |= 0xFF;
			}
			sdata += widthStep;
			ddata += widthStep;
		}
		//	dst = l_dst.clone(); 
		break;
	case RIGHT:
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < widthStep; w++)
			{
				if (w >= pt.x)
				{
					if (!sdata[w])
						ddata[w] |= 0xFF;
					else
						ddata[w] &= 0x00;
				}
				else
					ddata[w] |= 0xFF;
			}
			sdata += widthStep;
			ddata += widthStep;
		}
		//dst = l_dst.clone();

		break;
	default:break;
	}
}


void COpencvprocess::MixMatImg(const Mat& src, Mat& dst)
{
	if (src.empty() || dst.empty()) return;
	Mat color, gray;


	if (CV_8UC1 != src.type())
	{
		color = src.clone();
	}
	else
	{
		cvtColor(src, color, CV_GRAY2BGR);
	}

	if (CV_8UC1 != dst.type())
	{
		cvtColor(dst, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = dst.clone();
	}


	const int widthStep = gray.step;
	const int height = gray.rows;
	uchar* ddata = gray.data;
	uchar* cdata = color.data;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{

			if (!ddata[w])
			{
				cdata[3 * w] &= 0x00;
				cdata[3 * w + 1] &= 0x00;
				cdata[3 * w + 2] |= 0xFF;
			}


		}
		cdata += color.step;
		ddata += gray.step;
	}

	dst = color.clone();
}


int COpencvprocess::getDotsNumber(const Mat& src, uchar gray)
{
	if (src.empty()) return 0;
	const int widthStep = src.step;
	const int height = src.rows;
	uchar* sdata = src.data;
	int sum = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < widthStep; w++)
		{
			if (gray == sdata[w])
			{
				sum++;
			}
		}
		sdata += widthStep;	
	}
	return sum;
}


CString COpencvprocess::SaveFile(cv::Mat src,CString str/* = L""*/)
{
	//写在一起了 供以后参考
	if (src.empty()) return L"";

	//获取时间
	if (L"" == str)
	{
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format(_T("%4d-%d-%d_"), st.wYear, st.wMonth, st.wDay);
		strTime.Format(_T("%d-%d-%d"), st.wHour, st.wMinute, st.wSecond);
		str = strDate + strTime;
	}
	//获取路径
	CString AppPath;
	::GetModuleFileName(GetModuleHandle(NULL), AppPath.GetBuffer(300), 300);
	AppPath.ReleaseBuffer();
	AppPath = AppPath.Left(AppPath.ReverseFind('\\'));
	AppPath = AppPath + L"\\" + L"Image";
	CreateDirectory(AppPath, NULL);
	str = AppPath +L"\\"+ str + L".bmp";
	
	//CString转成char*
	int n = str.GetLength(); //获取str的字符数  
	int len = WideCharToMultiByte(CP_ACP, 0, str, n, NULL, 0, NULL, NULL); //获取宽字节字符的大小，大小是按字节计算的
	char *pChar = new char[len + 1]; //以字节为单位  
	if (n > 4092) pChar[0] = '\0';
	WideCharToMultiByte(CP_ACP, 0, str, n, pChar, len, NULL, NULL); //宽字节编码转换成多字节编码  
	pChar[len] = '\0'; //多字节字符以'\0'结束 

	imwrite(pChar, src);
	delete pChar;

	return str;
}


void COpencvprocess::get_mask_image(const Mat& src, Mat& dst, vector<vector<Point>>& contours, int ID)
{
	if (src.empty()) return;

	
		Mat Mask(src.size(), CV_8U, Scalar(0));
		const int widthStep = src.step;
		const int height = src.rows;
		uchar *sdata = src.data;
		uchar *ddata = Mask.data;
		drawContours(Mask, contours, ID/*获得的轮廓序号*/, Scalar(255), CV_FILLED/*2*/);

		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < widthStep; w++)
			{
				ddata[w] &= sdata[w];
			}
			sdata += widthStep;
			ddata += widthStep;
		}
	
		dst = Mask.clone();
}









float COpencvprocess::find_circle_radius(Mat& src, int& lower, int& higher, int *point)
{

	int width = src.cols;
	int height = src.rows;
	int widthStep = src.step;
	int pix_total = 0;
	int tmp;

	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
		{
			if (src.data[h*widthStep + w])
			{
				lower = higher = h;
				break;
			}
		}


	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
		{
			if ((uchar)255 == src.data[h*widthStep + w])
			{
				tmp = h;
				if (lower > tmp) lower = tmp;
				if (higher < tmp)
				{
					higher = tmp;
					//	point[X] = w;
					//	point[Y] = h;
				}
			}
		}
	return 0;
}



#if 0
int COpencvprocess::MatImgProcess(Mat& mat)
{
	//#pragma omp parallel
	{
		//多核心执行这个
		if (mat.empty())
			return INVALID;
		try

		{
			Mat gray, binnaryimg, color, grayforcont, graybilate;
			Point pend, pstart;
			vector<vector<Point>> contours;
			if (CV_8UC1 != mat.type())
			{
				color = mat.clone();
				cvtColor(mat, gray, CV_BGR2GRAY);
			}
			else
			{
				gray = mat.clone();
				cvtColor(mat, color, CV_GRAY2BGR);
			}
			image_mono_cesus(gray);

			//模糊去除噪点，降低误检率
			cv::bilateralFilter(gray, graybilate, 10, 20.0, 5);

			//GaussianBlur(gray, gray, Size(7, 7), 1, 1);
			//二值化
			//int threshold = ThresHold(graybilate, binnaryimg);

			cv::threshold(graybilate, grayforcont, ini->m_threshold2, 255, THRESH_BINARY);	//获取筷子的二值化

			cv::findContours(grayforcont, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

			int MaxID = getMaxIDandDeleteMin(contours, 1100);
			//if (0 != contours.size()) cout << contours[0].size() << endl;
			//用于找到白色最大连通区域
			if (-1 == MaxID)
			{
				m_ResultMatImg = mat.clone();
				show_error(m_ResultMatImg, "No object found!");
				return NOOBJECT;
			}

			//得到筷子的轮廓
			Mat white_mask(graybilate.size(), CV_8U, Scalar(0));
			cv::drawContours(white_mask, contours, MaxID, Scalar(255), CV_FILLED/*2*/);   // -1 表示所有轮廓
			cv::erode(white_mask, white_mask, Mat(), cv::Point(-1, -1), 1);

			Mat black_mask(graybilate.size(), CV_8U, Scalar(0));
			//adaptiveThreshold(gray, black_mask, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 105, 64);
			getAllBlackBlockinGray(black_mask, graybilate, white_mask);
			int threshold;
#ifdef TEST
			threshold = ThresHold(graybilate, binnaryimg);
			cout << "old version " << threshold << endl;
#endif
			if (ini->m_imageDealMode == 0)//OTSU
			{
				threshold = getOtsuByMask(graybilate, white_mask) + ini->m_idFixThreshold;
#ifdef TEST
				cout << "new version " << threshold << endl;
#endif
				if (threshold > 254) threshold = 254;
				else if (threshold < 0) threshold = 1;
			}
			else
			{
				threshold = ini->m_idFixThreshold;
				if (threshold > 254) threshold = 254;
				else if (threshold < 0) threshold = 1;
			}

			cv::adaptiveThreshold(black_mask, black_mask, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, ini->m_blocksize, (double)threshold / 7);
			m_ResultMatImg = black_mask.clone();
			//分别找到两张图的重心
			cv::Point pt;
			getWidthAndHeight(white_mask, pt);
			float width = pt.x;
			float height = pt.y;
			float ratio = width / height;
			if (2.0 < ratio || ratio < 0.5)
			{
				show_error(m_ResultMatImg, "Obj Ratio Outrange");
				return NODOTS;
			}
			find_gravity_center(white_mask, pstart);
			find_gravity_center(black_mask, pend);

			if (pend.x == 0 && pend.y == 0)
			{
				show_error(m_ResultMatImg, "There are no dots!");
				return NODOTS;
			}

			int xlen = iabs(pend.x - pstart.x);
			int ylen = iabs(pend.y - pstart.y);

			if (xlen > ylen)
			{
				pend.y = pstart.y;
				if (pend.x > pstart.x)
				{
					pend.x = pstart.x + 150;
				}
				else
				{
					pend.x = pstart.x - 150;
				}
			}
			else
			{
				pend.x = pstart.x;
				if (pend.y > pstart.y)
				{
					pend.y = pstart.y + 150;
				}
				else
				{
					pend.y = pstart.y - 150;
				}
			}

			cvtColor(m_ResultMatImg, m_ResultMatImg, CV_GRAY2BGR);
			drawArrow(m_ResultMatImg, pstart, pend, 30, 10, Scalar(250, 250, 0), 2, CV_AA);

			switch (ini->m_direction)
			{
			case UP:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return UP;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return DOWN;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return LEFT;
				}
				else
				{
					return RIGHT;
				}
				break;
			case DOWN:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return DOWN;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return UP;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return RIGHT;
				}
				else
				{
					return LEFT;
				}
				break;
			case LEFT:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return RIGHT;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return LEFT;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return UP;
				}
				else
				{
					return DOWN;
				}
				break;
			case RIGHT:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return LEFT;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return RIGHT;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return DOWN;
				}
				else
				{
					return UP;
				}
				break;
			default:
				if (pend.y > pstart.y && pend.x == pstart.x)
				{
					return UP;
				}
				else if ((pend.y < pstart.y && pend.x == pstart.x))
				{
					return DOWN;
				}
				else if ((pend.y == pstart.y && pend.x > pstart.x))
				{
					return LEFT;
				}
				else
				{
					return RIGHT;
				}
				break;
			}
			return INVALID;

			return true;
		}
		catch (...)
		{
			AfxMessageBox(L"图像处理错误");
			return false;
		}
		return false;
	}
}
#endif


#if 0
bool COpencvprocess::FindLine()
{
	int total1 = 4, total2 = 4;

	double b1 = 0.0, b2 = 0.0, a1 = 0.0, a2 = 0.0, sigma1 = 0.0, sigma2 = 0.0, sigma3 = 0.0, sigma4 = 0.0;
	double Laveragex = 0.0;
	double Laveragey = 0.0;
	double Raveragex = 0.0;
	double Raveragey = 0.0;

	int i;
	for (i = 0; i < 4; ++i) Laveragex += 1.0*m_coordinate[LLINESAMPLE1 + i][X] / 4;
	for (i = 0; i < 4; ++i) Laveragey += 1.0*m_coordinate[LLINESAMPLE1 + i][Y] / 4;
	for (i = 0; i < 4; ++i) Raveragex += 1.0*m_coordinate[RLINESAMPLE1 + i][X] / 4;
	for (i = 0; i < 4; ++i) Raveragey += 1.0*m_coordinate[RLINESAMPLE1 + i][Y] / 4;

	for (i = 0; i < 4; ++i) {
		sigma1 += m_coordinate[LLINESAMPLE1 + i][X] * m_coordinate[LLINESAMPLE1 + i][Y];
	}
	sigma1 -= 4 * Laveragex*Laveragey;
	for (i = 0; i < 4; ++i) {
		sigma2 += m_coordinate[LLINESAMPLE1 + i][X] * m_coordinate[LLINESAMPLE1 + i][X];
	}
	sigma2 -= 4 * Laveragex*Laveragex;
	b1 = sigma1 / sigma2;
	a1 = Laveragey - b1*Laveragex;

	for (i = 0; i < 4; ++i) {
		sigma3 += m_coordinate[RLINESAMPLE1 + i][X] * m_coordinate[RLINESAMPLE1 + i][Y];
	}
	sigma3 -= 4 * Raveragex*Raveragey;
	for (i = 0; i < 4; ++i) {
		sigma4 += m_coordinate[RLINESAMPLE1 + i][X] * m_coordinate[RLINESAMPLE1 + i][X];
	}
	sigma4 -= 4 * Raveragex*Raveragex;
	b2 = sigma3 / sigma4;
	a2 = Raveragey - b1*Raveragex;

	Lb = b1;
	La = a1;
	Rb = b2;
	Ra = a2;

	return true;
}
#endif



#if 0
// 寻找取样点，不过只是找左右方向的线的
bool COpencvprocess::FindLineSample(IplImage* Ipl)
{
	int dy;
	int iwidth = Ipl->width;
	int iheight = Ipl->height;

	dy = ImgROI.height / 4;

	int i = 0;
	for (int h = ImgROI.ypos; i < 4/*h < (ImgROI.height + ImgROI.ypos)*/; h += dy, i++)
		for (int w = ImgROI.xpos; w < (ImgROI.width + ImgROI.xpos); ++w) {
			int first = (unsigned char)(Ipl->imageData[(Ipl->height - h - 1)*Ipl->widthStep + w* Ipl->nChannels]);
			int second = (unsigned char)(Ipl->imageData[(Ipl->height - h - 1)*Ipl->widthStep + w* Ipl->nChannels + 1]);

			if ((first - second) < -128)
			{
				m_coordinate[LLINESAMPLE1 + i][X] = w + 1;
				m_coordinate[LLINESAMPLE1 + i][Y] = iheight - h - 1;

			}
			else if ((first - second) > 128)
			{
				m_coordinate[RLINESAMPLE1 + i][X] = w;
				m_coordinate[RLINESAMPLE1 + i][Y] = iheight - h - 1;
			}
		}
	return true;
}

#endif


#if 0

void COpencvprocess::DrawLine(IplImage* Ipl)
{
	int x1s = m_coordinate[LLINESAMPLE1][X];
	int x1e = m_coordinate[LLINESAMPLE4][X];
	int y1s = m_coordinate[LLINESAMPLE1][Y];
	int y1e = m_coordinate[LLINESAMPLE4][Y];

	int x2s = m_coordinate[RLINESAMPLE1][X];
	int x2e = m_coordinate[RLINESAMPLE4][X];
	int y2s = m_coordinate[RLINESAMPLE1][Y];
	int y2e = m_coordinate[RLINESAMPLE4][Y];

	CvPoint pt1s = cvPoint(x1s, y1s);
	CvPoint pt1e = cvPoint(x1s, y1e);
	CvPoint pt2s = cvPoint(x2s, y2s);
	CvPoint pt2e = cvPoint(x2e, y2e);

	cvLine(Ipl, pt1s, pt1e, CV_RGB(0, 255, 0), 2, 8, 0);
	cvLine(Ipl, pt2s, pt2e, CV_RGB(0, 255, 0), 2, 8, 0);
}
#endif