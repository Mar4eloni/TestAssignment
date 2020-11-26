// Fill out your copyright notice in the Description page of Project Settings.


#include "DataStorageAndCalculation.h"

UDataStorageAndCalculation::UDataStorageAndCalculation()
{

}

FVector UDataStorageAndCalculation::CalculateDiference(FVector& ReferencePoint, FVector& ItemPoint)
{
    return ReferencePoint-ItemPoint;
}

FVector UDataStorageAndCalculation::CalculateNewPosition(FVector& NewReferencePoint, FVector& DiferenceItemPoint)
{
    return NewReferencePoint+DiferenceItemPoint;
}
