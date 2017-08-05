// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>



// TODO:  在此处引用程序需要的其他头文件

#include <opencv/cv.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <vector>
#include <thread>

namespace IceDogCVLib
{
	enum class ColorSpace
	{
		RGB,
		HSV
	};

	struct Pixel
	{
		uchar BH;
		uchar GS;
		uchar RV;
		Pixel() :BH(0), GS(0), RV(0) {}
		Pixel(uchar bh, uchar gs, uchar rv) :BH(bh), GS(gs), RV(rv) {}

		void ToGray()
		{
			int gray = GetGray();
			BH = gray;
			GS = gray;
			RV = gray;
		}

		float GetGray()
		{
			return ((float)BH + (float)GS + (float)RV) / 3.0f;
		}

		Pixel operator+(Pixel& rightPixel)
		{
			int tb = (int)BH + (int)rightPixel.BH;
			int ts = (int)GS + (int)rightPixel.GS;
			int tr = (int)RV + (int)rightPixel.RV;
			return Pixel(tb > 255 ? 255 : tb, ts > 255 ? 255 : ts, tr > 255 ? 255 : tr);
		}

		Pixel operator-(Pixel& rightPixel)
		{
			int tb = (int)BH - (int)rightPixel.BH;
			int ts = (int)GS - (int)rightPixel.GS;
			int tr = (int)RV - (int)rightPixel.RV;
			return Pixel(tb < 0 ? 0 : tb, ts < 0 ? 0 : ts, tr < 0 ? 0 : tr);
		}

		double DistanceTo(Pixel& rightPixel)
		{
			double a, b, c;
			a = static_cast<double>(BH) - static_cast<double>(rightPixel.BH);
			b = static_cast<double>(GS) - static_cast<double>(rightPixel.GS);
			c = static_cast<double>(RV) - static_cast<double>(rightPixel.RV);
			return std::sqrt(a*a + b*b + c*c);
		}

		int Sum()
		{
			return BH + GS + RV;
		}
	};

	class Kernel
	{
	public:
		Kernel(int ksize, float* kData)
		{
			kernelSize = ksize;
			kernelData = std::shared_ptr<float>(kData, [](float* pt) {delete[] pt; });
		}
		int GetKernelSize() { return kernelSize; }
		float GetKernelData(int x, int y)
		{
			assert(x < kernelSize&&x >= 0 && y < kernelSize&&y >= 0);
			return kernelData.get()[y*kernelSize + x];
		}
	private:
		// the size of the kernel
		int kernelSize;
		// the data of the kernel
		std::shared_ptr<float> kernelData;
	};
}
