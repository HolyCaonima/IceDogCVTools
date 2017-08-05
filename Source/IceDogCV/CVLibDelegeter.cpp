// Fill out your copyright notice in the Description page of Project Settings.

#include "IceDogCV.h"
#include "Runtime/UMG/Public/UMG.h"
#include "CVLibDelegeter.h"


// Sets default values
ACVLibDelegeter::ACVLibDelegeter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACVLibDelegeter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACVLibDelegeter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACVLibDelegeter::LoadImage(FString url, UImage* imageRef)
{	
	IceDogImage* iceDogImg = new IceDogImage();
	iceDogImg->LoadFromUrl(std::string(TCHAR_TO_ANSI(*url)));
	iceDogImg->GenerateHistogram();
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(iceDogImg, imageRef));
}

void ACVLibDelegeter::AddImage(UImage* img, int32 width, int32 height, FVector initColor)
{
	Pixel* pxData = new Pixel[width*height];
	for (int i = 0; i < width*height; i++)
	{
		pxData[i] = Pixel(initColor.X, initColor.Y, initColor.Z);
	}
	IceDogImage* iceDogImg = new IceDogImage(ColorSpace::RGB,width,height,pxData);
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(iceDogImg, img));
}

void ACVLibDelegeter::UpdateTextureRegions(UTexture2D * Texture, FUpdateTextureRegion2D * Regions, uint32 SrcPitch, uint8 * SrcData)
{
	if (Texture->Resource)
	{
		struct FUpdateTextureRegionsData
		{
			FTexture2DResource* Texture2DResource;
			FUpdateTextureRegion2D* Regions;
			uint32 SrcPitch;
			uint8* SrcData;
		};

		FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

		RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
		RegionData->Regions = Regions;
		RegionData->SrcPitch = SrcPitch;
		RegionData->SrcData = SrcData;

		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			UpdateTextureRegionsData,
			FUpdateTextureRegionsData*, RegionData, RegionData,
			{
				RHIUpdateTexture2D(RegionData->Texture2DResource->GetTexture2DRHI(), 0, *(RegionData->Regions), RegionData->SrcPitch, RegionData->SrcData);
		delete[] RegionData->SrcData;
		delete RegionData;
			});
	}
}


IceDogImage* ACVLibDelegeter::GetIceDogImageUsingUImage(UImage* img)
{
	for (auto i : dispImages)
	{
		if (std::get<1>(i) == img) { return std::get<0>(i); }
	}
	return nullptr;
}

// update all the texture hold by dynamic materials
void ACVLibDelegeter::UpdateTexture()
{
	struct InData
	{
		uint8 _0;
		uint8 _1;
		uint8 _2;
		uint8 _3;
	};

	for (auto i: dispImages)
	{
		IceDogImage* dataSource = std::get<0>(i);
		UImage* dataTarget = std::get<1>(i);

		auto TargetSize = FVector2D(dataSource->GetWidth(), dataSource->GetHeight());

		UTexture2D* VideoTexture = UTexture2D::CreateTransient(TargetSize.X, TargetSize.Y);
		VideoTexture->UpdateResource();
		FUpdateTextureRegion2D* VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TargetSize.X, TargetSize.Y);

		Pixel* readyData = dataSource->GetPixelDataCopy();
		InData* feedData = new InData[TargetSize.X*TargetSize.Y];
		for (int i = 0; i < TargetSize.X*TargetSize.Y; ++i)
		{
			feedData[i]._0 = readyData[i].BH;
			feedData[i]._1 = readyData[i].GS;
			feedData[i]._2 = readyData[i].RV;
			feedData[i]._3 = 255;
		}
		delete[] readyData;

		// submit the data to render pipe line for update
		UpdateTextureRegions(VideoTexture, VideoUpdateTextureRegion, (uint32)(4 * TargetSize.X),(uint8*)feedData);
		auto oldBrush = dataTarget->Brush;
		oldBrush.ImageSize.X = TargetSize.X;
		oldBrush.ImageSize.Y = TargetSize.Y;
		dataTarget->SetBrush(oldBrush);
		dataTarget->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), VideoTexture);
	}
}

void ACVLibDelegeter::RemoveImage(UImage* imageRef)
{
	for (auto i : dispImages)
	{
		if (std::get<1>(i) == imageRef)
		{
			delete std::get<0>(i);
			dispImages.Remove(i);
			break;
		}
	}
}

void ACVLibDelegeter::SaveImage(FString url, UImage* imageRef)
{
	for (auto i : dispImages)
	{
		if (std::get<1>(i) == imageRef)
		{
			cv::imwrite(std::string(TCHAR_TO_ANSI(*url)), std::get<0>(i)->StoreToMat());
			break;
		}
	}
}

void ACVLibDelegeter::EqulizeImage(UImage* imageRef)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	source->ConvertToHSV();
	source->GenerateHistogram();
	*source = source->EqulizeHistogram();
	source->ConvertToBGR();
	source->GenerateHistogram();
}

void ACVLibDelegeter::AddSaltPepperNoise(UImage* imageRef,float prop)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateSaltPepperNoiseMap(prop);
	source->GenerateHistogram();
}

void ACVLibDelegeter::AddRandomNoise(UImage* imageRef)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateRandomNoiseMap();
	source->GenerateHistogram();
}

void ACVLibDelegeter::AddGaussianNoise(UImage* imageRef, float sigmoid)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateGaussNoiseMap(sigmoid);
	source->GenerateHistogram();
}

void ACVLibDelegeter::OverNeighborhoodAvgFilter(UImage* imageRef, int32 kernelSize, float T)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateOverNeighborhoodAvgFilterMap(kernelSize, T);
	source->GenerateHistogram();
}

void ACVLibDelegeter::OverMidValueFilter(UImage* imageRef, int32 kernelSize, float T)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateOverMidValueFilterMap(kernelSize, T);
	source->GenerateHistogram();
}

void ACVLibDelegeter::GaussBlurFilter(UImage* imageRef, int32 kernelSize, float yeta)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateGaussBlurFilterMap(kernelSize, yeta);
	source->GenerateHistogram();
}

void ACVLibDelegeter::GenearteLaplacianFeature(UImage* imageRef, UImage* newImage)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* target = new IceDogImage();
	*target = source->GenerateLaplacianEdgeMap();
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(target, newImage));
}

void ACVLibDelegeter::GenerateSobelEdgeFeature(UImage* imageRef, UImage* newImage)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* target = new IceDogImage();
	*target = source->GenerateSobelEdgeMap();
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(target, newImage));
}

void ACVLibDelegeter::GenerateRobertsEdgeFeature(UImage* imageRef, UImage* newImage, int32 threshold /*= 0*/)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* target = new IceDogImage();
	*target = source->GenerateRobertsEdgeMap(threshold);
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(target, newImage));
}

void ACVLibDelegeter::GeneratePrewittEdgeFeature(UImage* imageRef, UImage* newImage)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* target = new IceDogImage();
	*target = source->GeneratePrewittEdgeMap();
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(target, newImage));
}

void ACVLibDelegeter::GenerateDifferentEdge(UImage* imageRef, UImage* newImage, int32 threshold/*=0*/)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* target = new IceDogImage();
	*target = source->GenerateDifferentEdgeMap(threshold);
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(target, newImage));
}

void ACVLibDelegeter::AddTwoImage(UImage* imageFirst, UImage* imageSecond, UImage* outImage)
{
	IceDogImage* first = GetIceDogImageUsingUImage(imageFirst);
	IceDogImage* second = GetIceDogImageUsingUImage(imageSecond);
	IceDogImage* sumImage = new IceDogImage();
	*sumImage = *first + *second;
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(sumImage, outImage));
}

void ACVLibDelegeter::SubTwoImage(UImage* imageFirst, UImage* imageSecond, UImage* outImage)
{
	IceDogImage* first = GetIceDogImageUsingUImage(imageFirst);
	IceDogImage* second = GetIceDogImageUsingUImage(imageSecond);
	IceDogImage* sumImage = new IceDogImage();
	*sumImage = *first - *second;
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(sumImage, outImage));
}

void ACVLibDelegeter::DetectAndDrawCircle(UImage* imageRef)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	auto circlePlace= source->FindCircleUsingHough();
	for (auto& i : circlePlace)
	{
		source->DrawCircle(std::get<0>(i), std::get<1>(i), std::get<2>(i), Pixel(220, 30, 30));
	}
	source->GenerateHistogram();
}

void ACVLibDelegeter::SplitImageUsingOTSU(UImage* imageRef, UImage* imageFront, UImage* imageBack)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* frontImg = new IceDogImage();
	IceDogImage* backImg = new IceDogImage();
	source->SplitImageUsingOTSU(*frontImg, *backImg);
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(frontImg, imageFront));
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(backImg, imageBack));
	frontImg->GenerateHistogram();
	backImg->GenerateHistogram();
}

void ACVLibDelegeter::SplitImageUsingKMeansPP(UImage* imageRef, TArray<UImage*> numImages)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	auto imageResult = source->SplitImageUsingKeamsPP(numImages.Num());
	for (size_t i=0;i<imageResult.size();++i)
	{
		IceDogImage* newImage = new IceDogImage();
		*newImage = imageResult[i];
		newImage->GenerateHistogram();
		dispImages.Add(std::tuple<IceDogImage*, UImage*>(newImage, numImages[i]));
	}
}

void ACVLibDelegeter::ExpandOrCorrosionImage(UImage* imageRef, bool useCustom, TArray<float> customKernel, bool useCross3x3, bool expandImage, int32 kernelSize, float multiplyer)
{
	Kernel procKernel = IceDogKernelGenerater::GenerateECKernel(kernelSize, multiplyer);
	if (useCross3x3)
	{
		procKernel = IceDogKernelGenerater::GenerateCross3x3ECKernel(multiplyer);
	}
	if (useCustom)
	{
		float* klData = new float[customKernel.Num()];
		for (int i=0;i<customKernel.Num();++i)
		{
			klData[i] = customKernel[i];
		}
		procKernel = Kernel(sqrtf(customKernel.Num()), klData);
	}

	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	if (expandImage)
	{
		*source = source->ExpandCorrosionImage(procKernel);
	}
	else
	{
		*source = source->ExpandCorrosionImage(procKernel, 0);
	}
}

void ACVLibDelegeter::ApplyCustomKernel(UImage* imageRef, TArray<float> kernelDefine, int32 klSize)
{
	float* kldata = new float[klSize*klSize];
	for (int i=0;i<kernelDefine.Num();++i)
	{
		kldata[i] = kernelDefine[i];
	}
	Kernel kl(klSize, kldata);
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	*source = source->GenerateImageUseCustomKernel(kl);
}

void ACVLibDelegeter::DrawHistogramFromAToB(UImage* imageRef, UImage* targetImg, int32 chanel)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	IceDogImage* target = GetIceDogImageUsingUImage(targetImg);
	source->GenerateHistogram();
	IceDogImage temp;
	temp.LoadFromMat(source->DrawHisto2Mat(chanel));
	*target = temp;
}

void ACVLibDelegeter::CreateImageCopy(UImage* imageRef, UImage* newTarget)
{
	IceDogImage* source = GetIceDogImageUsingUImage(imageRef);
	Pixel* pxCp= source->GetPixelDataCopy();
	IceDogImage* newImg = new IceDogImage(ColorSpace::RGB, source->GetWidth(), source->GetHeight(), pxCp);
	newImg->GenerateHistogram();
	dispImages.Add(std::tuple<IceDogImage*, UImage*>(newImg, newTarget));
}
