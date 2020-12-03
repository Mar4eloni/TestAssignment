// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "JsonParser.generated.h"

/*
	"name": "Item 1",
	"color": "#FF2020",
	"position": {
		"x": 0,
		"y": 0,
		"z": 3
	  }
*/

USTRUCT()
struct FReferencePoint
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY()
	float x;

	UPROPERTY()
	float y;

	UPROPERTY()
	float z;
};

USTRUCT()
struct FItemPoint
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY()
	FString name;

	UPROPERTY()
	FString color;

	UPROPERTY()
	FReferencePoint position;
};

USTRUCT()
struct FJsonResponseReturn
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY()
	FString id;

	UPROPERTY()
	FString name;

	UPROPERTY()
	FReferencePoint referencepoint1;
	
	UPROPERTY()
	FReferencePoint referencepoint2;

	UPROPERTY()
	TArray<FItemPoint> items;
};

class UDataStorageAndCalculation;

UCLASS(Blueprintable)
class NETCETRA_API AJsonParser : public AActor
{
	GENERATED_BODY()
	
private:
	FString AuthTokenName;
	FString AuthTokenCode;

public:	
	// Sets default values for this actor's properties
	AJsonParser();

	FHttpModule* Http;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Getting data")
	void RequestBuilder(FString sceneid);

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FJsonResponseReturn ObjectResponse;

	/*UPROPERTY(BlueprintReadWrite, Category = "Vector Data")
	UDataStorageAndCalculation* DataVectors;*/

	// DATA FROM THE API
	
	FVector ReferencePoint1;
	FVector ReferencePoint2;
	float ScaleDistance;

	FVector Item1;
	FVector Item2;
	FVector Item3;

	FVector CenterOfPolygon;

	FVector ScaleBetweenRefPoints;

	FVector NewReferencePoint1;
	FVector NewReferencePoint2;

	float DistanceItem1ToCenter;
	float DistanceItem2ToCenter;
	float DistanceItem3ToCenter;
	float DistanceRef1ToCenter;
	float DistanceRef2ToCenter;

	FVector NewItem1;
	FVector NewItem2;
	FVector NewItem3;

	UFUNCTION(BlueprintCallable)
	FVector CalculateDiference(FVector& ReferencePoint, FVector& ItemPoint);

	UFUNCTION(BlueprintCallable)
	void CalculateDifferences();

	UFUNCTION(BlueprintCallable)
	FVector CalculateNewPosition(FVector& NewReferencePoint, FVector& DiferenceItemPoint);

	UFUNCTION(BlueprintCallable)
	void CalculateNewPositions();

	UFUNCTION(BlueprintCallable)
	float CalculateAngle(FVector ref1, FVector ref2);

	UFUNCTION(BlueprintCallable)
	void SpawnBalls(FVector referencePoint1, FVector referencePoint2, FVector item1,  FVector item2, FVector item3);
	//(FVector& referencePoint1, FVector& referencePoint2, FVector& item1, FVector& item2, FVector& item3);

	UFUNCTION(BlueprintCallable)
	FVector CalculateCenterOfPolygon(const TArray<FVector> verticies, int vertexCount);

	UFUNCTION(BlueprintCallable)
	void CalculatePositions(FVector RotationAxis, float Angle);

	UFUNCTION(BlueprintCallable)
	FVector CalculateZRotation(float Angle);
	UFUNCTION(BlueprintCallable)
	FVector CalculateYRotation(float Angle);
	UFUNCTION(BlueprintCallable)
	FVector CalculateXRotation( float Angle);

};
