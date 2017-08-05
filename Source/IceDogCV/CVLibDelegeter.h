// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "IceDogImage.h"
#include "CVLibDelegeter.generated.h"

UCLASS()
class ICEDOGCV_API ACVLibDelegeter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACVLibDelegeter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// for blueprint call for image loading
	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void LoadImage(FString url, UImage* imageRef);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void AddImage(UImage* img,int32 width,int32 height,FVector initColor);

	// for bp call for texture update
	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void UpdateTexture();

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void RemoveImage(UImage* imageRef);

	UFUNCTION(BlueprintCallable, Category= "CVLIB")
	void SaveImage(FString url, UImage* imageRef);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void EqulizeImage(UImage* imageRef);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void AddSaltPepperNoise(UImage* imageRef,float prop);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void AddRandomNoise(UImage* imageRef);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void AddGaussianNoise(UImage* imageRef, float sigmoid);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void OverNeighborhoodAvgFilter(UImage* imageRef, int32 kernelSize, float T);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void OverMidValueFilter(UImage* imageRef, int32 kernelSize, float T);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void GaussBlurFilter(UImage* imageRef, int32 kernelSize, float yeta);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void GenearteLaplacianFeature(UImage* imageRef,UImage* newImage);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void GenerateSobelEdgeFeature(UImage* imageRef, UImage* newImage);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void GenerateRobertsEdgeFeature(UImage* imageRef, UImage* newImage, int32 threshold = 0);

	UFUNCTION(BlueprintCallable, Category="CVLIB")
	void GeneratePrewittEdgeFeature(UImage* imageRef, UImage* newImage);

	UFUNCTION(BlueprintCallable, Category="CVLIB")
	void GenerateDifferentEdge(UImage* imageRef, UImage* newImage,int32 threshold=0);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void AddTwoImage(UImage* imageFirst, UImage* imageSecond, UImage* outImage);

	UFUNCTION(BlueprintCallable, Category="CVLIB")
	void SubTwoImage(UImage* imageFirst, UImage* imageSecond, UImage* outImage);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void DetectAndDrawCircle(UImage* imageRef);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void SplitImageUsingOTSU(UImage* imageRef,UImage* imageFront, UImage* imageBack);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void SplitImageUsingKMeansPP(UImage* imageRef, TArray<UImage*> numImages);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void ExpandOrCorrosionImage(UImage* imageRef,bool useCustom,TArray<float> customKernel,bool useCross3x3,bool expandImage,int32 kernelSize,float multiplyer=1);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void ApplyCustomKernel(UImage* imageRef, TArray<float> kernelDefine,int32 klSize);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void DrawHistogramFromAToB(UImage* imageRef, UImage* targetImg,int32 chanel);

	UFUNCTION(BlueprintCallable, Category = "CVLIB")
	void CreateImageCopy(UImage* imageRefm, UImage* newTarget);
	
private:
	// update the texture data
	void UpdateTextureRegions(UTexture2D * Texture, FUpdateTextureRegion2D * Regions, uint32 SrcPitch, uint8 * SrcData);

	// get the icedogimage with Uimage
	IceDogImage* GetIceDogImageUsingUImage(UImage* img);

private:
	TArray<std::tuple<IceDogImage*,UImage*>> dispImages;
};
