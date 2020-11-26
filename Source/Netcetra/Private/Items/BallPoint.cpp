// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BallPoint.h"
#include "Components/SphereComponent.h"

// Sets default values
ABallPoint::ABallPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("sphere"));
	RootComponent = Sphere;
	Name = TEXT("Ball");
}

// Called when the game starts or when spawned
void ABallPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABallPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallPoint::SetValueToPosition(float x, float y, float z)
{
	Position.X = x;
	Position.Y = y;
	Position.Z = z;
}

