
#pragma once
#ifndef ______________SOURCE_________________
#define ______________SOURCE_________________


//重复声明，有用

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

#ifdef _WIN64
#pragma comment(lib,"..\\lib\\x64\\opencv_core2411.lib")
#pragma comment(lib,"..\\lib\\x64\\opencv_highgui2411.lib")
#pragma comment(lib,"..\\lib\\x64\\opencv_imgproc2411.lib")
#pragma comment(lib,"..\\lib\\x64\\opencv_calib3d2411.lib")





#pragma comment(lib,"..\\lib\\x64\\Dmc1380.lib")




#else
#pragma comment(lib,"..\\lib\\x86\\opencv_core2411.lib")
#pragma comment(lib,"..\\lib\\x86\\opencv_highgui2411.lib")
#pragma comment(lib,"..\\lib\\x86\\opencv_imgproc2411.lib")
#pragma comment(lib,"..\\lib\\x86\\opencv_calib3d2411.lib")


#pragma comment(lib,"..\\lib\\x86\\Dmc1380.lib")






#endif
















#endif