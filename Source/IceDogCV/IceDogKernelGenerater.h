// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <opencv/cv.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <vector>
#include <thread>

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

/**
 * 
 */
class ICEDOGCV_API IceDogKernelGenerater
{
public:
	/* generate the mean value kernel */
	static Kernel GenerateMeanValueKernel(int kernelSize);
	/* generate the gauss kernel */
	static Kernel GenerateGaussKernel(int kernelSize, float yeta);
	/* generate laplacian 3x3 kernel */
	static Kernel GenerateLaplacian3x3Kernel();
	/* generate sobelX 3x3 kernel */
	static Kernel GenerateSobelX3x3Kernel();
	/* generate sobelY 3x3 kernel */
	static Kernel GenerateSobelY3x3Kernel();
	/* generate prewittX 3x3 kernel */
	static Kernel GeneratePrewittX3x3Kernel();
	/* generate prewittY 3x3 kernel */
	static Kernel GeneratePrewittY3x3Kernel();
	/* generate corrosion expand operation kernel */
	static Kernel GenerateECKernel(int kernelSize, float instMultipler = 1);
	/* generate cross expand corrosion operation kernel */
	static Kernel GenerateCross3x3ECKernel(float instMultipler = 1);
private:
	static inline float GaussValule2Dem(float h, float w, float yeta);
};
