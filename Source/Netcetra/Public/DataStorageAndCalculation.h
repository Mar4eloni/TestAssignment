// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataStorageAndCalculation.generated.h"

/**
 * 
 */
UCLASS()
class NETCETRA_API UDataStorageAndCalculation : public UObject
{
	GENERATED_BODY()

public:
	UDataStorageAndCalculation();

	FVector ReferencePoint1;
	FVector ReferencePoint2;

	FVector Item1;
	FVector Item2;
	FVector Item3;

	FVector NewReferencePoint1;
	FVector NewReferencePoint2;

	FVector DiferenceItem1;
	FVector DiferenceItem2;
	FVector DiferenceItem3;

	UFUNCTION()
	FVector CalculateDiference(FVector& ReferencePoint, FVector& ItemPoint);

	UFUNCTION()
	FVector CalculateNewPosition(FVector& NewReferencePoint, FVector& DiferenceItemPoint);
	
};
