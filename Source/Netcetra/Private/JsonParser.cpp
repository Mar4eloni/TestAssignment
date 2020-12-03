// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParser.h"
#include "JsonUtilities.h"
#include "Items/BallPoint.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathVectorConstants.h"
//#include "DataStorageAndCalculation.h"
//#include "KismetStringLibrary.h"

// Sets default values
AJsonParser::AJsonParser()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Http = &FHttpModule::Get();
	AuthTokenName = "Netcetera-Auth-Token";
	AuthTokenCode = "n4woc1HUxOGVoRO3UihJN2TRTYGHgw";
	//DataVectors = new UDataStorageAndCalculation();
	//ReferencePoint1 = new FVector();

}

// Called when the game starts or when spawned
void AJsonParser::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AJsonParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJsonParser::RequestBuilder(FString sceneid)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	JsonObject->SetStringField(TEXT("sceneId"), sceneid);
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);



	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	Request->SetVerb("POST");
	Request->SetHeader(AuthTokenName, AuthTokenCode);
	Request->SetHeader("Content-Type", TEXT("application/json"));
	//This is the url on which to process the request
	Request->SetURL("https://arstudio-test.azurewebsites.net/api/v2/scenes");
	// adding the body to the header
	Request->SetContentAsString(OutputString);
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonParser::OnResponseReceived);


	Request->ProcessRequest();
}

void AJsonParser::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (bWasSuccessful && FJsonSerializer::Deserialize(Reader, JsonObject))
	{

		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ObjectResponse, 0, 0);


		ReferencePoint1.X = ObjectResponse.referencepoint1.x;
		ReferencePoint1.Y = ObjectResponse.referencepoint1.y;
		ReferencePoint1.Z = ObjectResponse.referencepoint1.z;

		ReferencePoint2.X = ObjectResponse.referencepoint2.x;
		ReferencePoint2.Y = ObjectResponse.referencepoint2.y;
		ReferencePoint2.Z = ObjectResponse.referencepoint2.z;

		Item1.X = ObjectResponse.items[0].position.x;
		Item1.Y = ObjectResponse.items[0].position.y;
		Item1.Z = ObjectResponse.items[0].position.z;

		Item2.X = ObjectResponse.items[1].position.x;
		Item2.Y = ObjectResponse.items[1].position.y;
		Item2.Z = ObjectResponse.items[1].position.z;

		Item3.X = ObjectResponse.items[2].position.x;
		Item3.Y = ObjectResponse.items[2].position.y;
		Item3.Z = ObjectResponse.items[2].position.z;



		CalculateDifferences();
		SpawnBalls(ReferencePoint1, ReferencePoint2, Item1, Item2, Item3);
		CalculateNewPositions();
	}
}

FVector AJsonParser::CalculateDiference(FVector& ReferencePoint, FVector& ItemPoint)
{
	return ReferencePoint - ItemPoint;
}

void AJsonParser::CalculateDifferences()
{
	TArray<FVector> verticies = {
		ReferencePoint1,
		ReferencePoint2,
		Item1,
		Item2,
		Item3 };

	CenterOfPolygon = CalculateCenterOfPolygon(verticies, verticies.Num());



	DistanceItem1ToCenter = FVector::Dist(CenterOfPolygon, Item1);
	DistanceItem2ToCenter = FVector::Dist(CenterOfPolygon, Item2);
	DistanceItem3ToCenter = FVector::Dist(CenterOfPolygon, Item3);
	DistanceRef1ToCenter = FVector::Dist(CenterOfPolygon, ReferencePoint1);
	DistanceRef2ToCenter = FVector::Dist(CenterOfPolygon, ReferencePoint2);
	


	ScaleDistance = FVector::Dist(ReferencePoint1, ReferencePoint2);//ReferencePoint1 - ReferencePoint2;

}

FVector AJsonParser::CalculateNewPosition(FVector& NewReferencePoint, FVector& DiferenceItemPoint)
{
	return NewReferencePoint + DiferenceItemPoint;
}

void AJsonParser::CalculateNewPositions()
{
	NewReferencePoint1.X = -0.1114778f;
	NewReferencePoint1.Y = 0.5314918f;
	NewReferencePoint1.Z = 0.7510971f;// new FVector(, 0.5314918f, 0.7510971f);
	NewReferencePoint2.X = 1.562134;
	NewReferencePoint2.Y = 1.106552;
	NewReferencePoint2.Z = 1.682938;//new FVector(1.562134, 1.106552, 1.682938);



	FQuat BetweenQuat = FQuat::FindBetweenVectors(CenterOfPolygon, ReferencePoint1);//NewReferencePoint1);
	float Ref1Angle = 0.0f;
	FVector RotationAxisRef1;
	BetweenQuat.ToAxisAndAngle(RotationAxisRef1, Ref1Angle);
	//FRotator rot = FRotator::vec
	RotationAxisRef1.Normalize();

	FQuat BetweenQuat2 = FQuat::FindBetweenVectors(CenterOfPolygon, ReferencePoint2);
	float Ref2Angle = 0.0f;
	FVector RotationAxisRef2;
	BetweenQuat2.ToAxisAndAngle(RotationAxisRef2, Ref2Angle);

	FQuat BetweenQuatItem1 = FQuat::FindBetweenVectors(CenterOfPolygon, Item1);
	float Item1Angle = 0.0f;
	FVector RotationAxisItem1;
	BetweenQuat2.ToAxisAndAngle(RotationAxisItem1, Item1Angle);

	FQuat BetweenQuatItem2 = FQuat::FindBetweenVectors(CenterOfPolygon, Item2);
	float Item2Angle = 0.0f;
	FVector RotationAxisItem2;
	BetweenQuat2.ToAxisAndAngle(RotationAxisItem2, Item2Angle);

	FQuat BetweenItem3 = FQuat::FindBetweenVectors(CenterOfPolygon, Item3);
	float Item3Angle = 0.0f;
	FVector RotationAxisItem3;
	BetweenQuat2.ToAxisAndAngle(RotationAxisItem3, Item3Angle);

	FQuat BetweenNewRefP = FQuat::FindBetweenVectors(CenterOfPolygon, NewReferencePoint1);
	float NewRefPAngle = 0.0f;
	FVector RotationAxisNewRefP;
	BetweenQuat2.ToAxisAndAngle(RotationAxisNewRefP, NewRefPAngle);

	FQuat BetweenNewRefP2 = FQuat::FindBetweenVectors(CenterOfPolygon, NewReferencePoint2);
	float NewRef2PAngle = 0.0f;
	FVector RotationAxisNewRefP2;
	BetweenQuat2.ToAxisAndAngle(RotationAxisNewRefP2, NewRef2PAngle);

	FQuat f = FQuat::FindBetweenVectors(ReferencePoint2 - ReferencePoint1, NewReferencePoint2 - ReferencePoint1);
	float theta = 0.0f;
	FVector RotationAxis;
	f.ToAxisAndAngle(RotationAxis, theta);

	UE_LOG(LogTemp, Warning, TEXT("The AngleBetween is: %f"), theta);

	/*NewItem1 = f.RotateVector(Item1);
	NewItem2 = f.RotateVector(Item2);
	NewItem3 = f.RotateVector(Item3);*/

	float testAngle = CalculateAngle(ReferencePoint2 - ReferencePoint1, NewReferencePoint2 - ReferencePoint1);
	const FVector testRotAxis = FVector::CrossProduct(ReferencePoint2 - ReferencePoint1, NewReferencePoint2 - ReferencePoint1);
	UE_LOG(LogTemp, Warning, TEXT("The TEST AngleBetween is: %f"), testAngle);

	NewItem1 = Item1.RotateAngleAxis(testAngle, testRotAxis);
	NewItem2 = Item2.RotateAngleAxis(testAngle, testRotAxis);
	NewItem3 = Item3.RotateAngleAxis(testAngle, testRotAxis);

	//Rodriges rotation equation
	/*NewItem1 = (Item1 * FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle))) +
		((RotationAxisNewRefP * Item1) * FMath::Sin(FMath::RadiansToDegrees(NewRefPAngle))) +
		(RotationAxisNewRefP * (RotationAxisNewRefP * Item1)) * (1 - FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle)));
	FVector temp2 = Item2 * FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle));
	FVector temp3 = Item3 * FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle));*/
	//https://math.stackexchange.com/questions/1917449/rotate-polygon-around-center-and-get-the-coordinates/1917485
	float NewScaleDistance = FVector::Dist(NewReferencePoint1, NewReferencePoint2);
	{
		UE_LOG(LogTemp, Warning, TEXT("The Scale Value is: %f"), NewScaleDistance - ScaleDistance);
	}
	//CenterOfPolygon.


	UE_LOG(LogTemp, Warning, TEXT("The Rotation Axis vector: %f , %f , %f"),
		RotationAxis.X, RotationAxis.Y, RotationAxis.Z);
	UE_LOG(LogTemp, Warning, TEXT("The TEst Rotation Axis vector: %f , %f , %f"),
		testRotAxis.X, testRotAxis.Y, testRotAxis.Z);

	//UE_LOG(LogTemp, Warning, TEXT("The diference angle from new and old position: %f"), FMath::RadiansToDegrees(NewRefPAngle) - FMath::RadiansToDegrees(Ref1Angle));
	//Item1.rotate

 	//NewItem1 = Item1.RotateAngleAxis(NewRefPAngle - Item1Angle, CenterOfPolygon);
	//NewItem2 = Item2.RotateAngleAxis(NewRefPAngle - Item2Angle, CenterOfPolygon);
	//NewItem3 = Item3.RotateAngleAxis(NewRefPAngle - Item2Angle, CenterOfPolygon);

	/// <summary>
	/// PROBLEM SO AGLITE !!!!
	/// </summary>
	
	FVector RefPointDifference = ReferencePoint1 - NewReferencePoint1;
	float angleRefPoints = FMath::Atan2(RefPointDifference.Y,RefPointDifference.X);
	//float angleRefPoints2 = FMath::Atan2(RefPointDifference.Z, RefPointDifference.X);

	FVector Ref2PointDifference = ReferencePoint2 - NewReferencePoint2;
	float angleRefPoints2 = FMath::Atan2(Ref2PointDifference.Y, Ref2PointDifference.X);
	UE_LOG(LogTemp, Warning, TEXT("The AngleBetween refpoint and newrefpoint is: %f"), angleRefPoints);
	UE_LOG(LogTemp, Warning, TEXT("The AngleBetween refpoint and newrefpoint is: %f"), angleRefPoints2);

	/*NewItem1 = Item1.RotateAngleAxis(FMath::RadiansToDegrees(angleRefPoints) - 180, CenterOfPolygon);
	NewItem2 = Item2.RotateAngleAxis(FMath::RadiansToDegrees(angleRefPoints) - 180, CenterOfPolygon);
	NewItem3 = Item3.RotateAngleAxis(FMath::RadiansToDegrees(angleRefPoints) - 180, CenterOfPolygon);*/// FVector(0.0, 1.0, 0.0));

	/*NewItem1 = NewItem1.RotateAngleAxis(FMath::RadiansToDegrees(angleRefPoints2) - 180, CenterOfPolygon);
	NewItem2 = NewItem2.RotateAngleAxis(FMath::RadiansToDegrees(angleRefPoints2) - 180, CenterOfPolygon);
	NewItem3 = NewItem3.RotateAngleAxis(FMath::RadiansToDegrees(angleRefPoints2) - 180, CenterOfPolygon);*/

	/*if (RotationAxisNewRefP.X != 0.0)
	{
		NewItem1 = CalculateXRotation(Item1Angle - NewRefPAngle);
		NewItem2 = CalculateXRotation(Item2Angle - NewRefPAngle);
		NewItem3 = CalculateXRotation(Item3Angle - NewRefPAngle);
	}
	if (RotationAxisNewRefP.Y != 0.0)
	{
		NewItem1 = CalculateYRotation(Item1Angle - NewRefPAngle);
		NewItem2 = CalculateYRotation(Item2Angle - NewRefPAngle);
		NewItem3 = CalculateYRotation(Item3Angle - NewRefPAngle);
	}*/
	//if (RotationAxisNewRefP.Z != 0.0)
	//{
	//	//NewItem1 = CalculateZRotation(Item1Angle - NewRefPAngle);
	//}

	//CalculatePositions(RotationAxis, theta);//FMath::RadiansToDegrees(theta));
	//CalculatePositions(RotationAxisNewRefP, FMath::RadiansToDegrees(NewRefPAngle));
	//CalculatePositions(RotationAxisNewRefP2, NewRef2PAngle);


	/*NewItem1 = Item1.RotateAngleAxis(NewRefPAngle, RotationAxisNewRefP);
	NewItem1 = Item2.RotateAngleAxis(NewRefPAngle, RotationAxisNewRefP);
	NewItem1 = Item3.RotateAngleAxis(NewRefPAngle, RotationAxisNewRefP);*/

	/*NewItem3 = -(Item3 * FMath::Cos(NewRefPAngle)) + (FVector::CrossProduct(NewReferencePoint1, Item3) * FMath::Sin(NewRefPAngle)) +
		(NewReferencePoint1 * FVector::DotProduct(NewReferencePoint1, Item3)) * (1 - FMath::Cos(NewRefPAngle));*/

		

	UE_LOG(LogTemp, Warning, TEXT("The Center Polygon vector: %f , %f , %f"),
		CenterOfPolygon.X, CenterOfPolygon.Y, CenterOfPolygon.Z);



	GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Magenta, FString::Printf(TEXT("The Item 1 coords: %f , %f , %f"), NewItem1.X, NewItem1.Y, NewItem1.Z));
	UE_LOG(LogTemp, Warning, TEXT("The Item 1 coords: %f , %f , %f"), NewItem1.X, NewItem1.Y, NewItem1.Z);
	GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Magenta, FString::Printf(TEXT("The Item 2 coords: %f , %f , %f"), NewItem2.X, NewItem2.Y, NewItem2.Z));
	UE_LOG(LogTemp, Warning, TEXT("The Item 2 coords: %f , %f , %f"), NewItem1.X, NewItem2.Y, NewItem2.Z);
	GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Magenta, FString::Printf(TEXT("The Item 3 coords: %f , %f , %f"), NewItem3.X, NewItem3.Y, NewItem3.Z));
	UE_LOG(LogTemp, Warning, TEXT("The Item 3 coords: %f , %f , %f"), NewItem3.X, NewItem3.Y, NewItem3.Z);


	GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Green, FString::Printf(TEXT("The Quat Angle: %f Quat Axis: X: %f,Y: %f,Z: %f "),
		FMath::RadiansToDegrees(Ref1Angle), RotationAxisRef1.X, RotationAxisRef1.Y, RotationAxisRef1.Z));

	/*GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Blue, FString::Printf(TEXT("The Quat Angle: %f Quat Axis: X: %f,Y: %f,Z: %f "),
		FMath::RadiansToDegrees(Ref2Angle), RotationAxisRef2.X, RotationAxisRef2.Y, RotationAxisRef2.Z));

	GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Blue, FString::Printf(TEXT("The Product Quat Angle: %f Quat Axis: X: %f,Y: %f,Z: %f "),
		FMath::RadiansToDegrees(ProductRotAngle), ProductRotationAxis.X, ProductRotationAxis.Y, ProductRotationAxis.Z));*/


	UWorld* world = GetWorld();
	DrawDebugSphere(world, NewReferencePoint1 * 100, 10, 32, FColor::White, false, 100.0f);
	DrawDebugSphere(world, NewReferencePoint2 * 100, 10, 32, FColor::White, false, 100.0f);
	DrawDebugSphere(world, NewItem1 * 100, 10, 32, FColor::Red, false, 100.0f);
	DrawDebugSphere(world, NewItem2 * 100, 10, 32, FColor::Green, false, 100.0f);
	DrawDebugSphere(world, NewItem3 * 100, 10, 32, FColor::Blue, false, 100.0f);
	//DrawDebugSphere(world, CenterOfPolygon * 100, 10, 32, FColor::Black, false, 30.0f);




}

float AJsonParser::CalculateAngle(FVector ref1, FVector ref2)
{
	ref1.Normalize();
	ref2.Normalize();

	float dp = FVector::DotProduct(ref1, ref2);

	float rads = FMath::Acos(dp);
	float angle = FMath::RadiansToDegrees(rads);

	return angle;
}

void AJsonParser::SpawnBalls(FVector referencePoint1, FVector referencePoint2, FVector item1, FVector item2, FVector item3)
{
	/*
		ReferencePoint1
		ReferencePoint2
		Item1
		Item2
		Item3
	*/
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Ball Spawning"));

	UWorld* world = GetWorld();

	FActorSpawnParameters SpawnParams;


	DrawDebugSphere(world, referencePoint1 * 100, 10, 32, FColor::White, false, 30.0f);
	DrawDebugSphere(world, referencePoint2 * 100, 10, 32, FColor::White, false, 30.0f);
	DrawDebugSphere(world, item1 * 100, 10, 32, FColor::Red, false, 30.0f);
	DrawDebugSphere(world, item2 * 100, 10, 32, FColor::Green, false, 30.0f);
	DrawDebugSphere(world, item3 * 100, 10, 32, FColor::Blue, false, 30.0f);
	DrawDebugSphere(world, CenterOfPolygon * 100, 10, 32, FColor::Black, false, 30.0f);



}
FVector AJsonParser::CalculateCenterOfPolygon(const TArray<FVector> verticies, int vertexCount)
{
	/*
	 https://stackoverflow.com/questions/22465832/center-point-calculation-for-3d-polygon
	 https://math.stackexchange.com/questions/458293/find-the-centroid-of-a-3d-polygon
	 https://stackoverflow.com/questions/44356447/finding-the-centroid-of-a-polygon
	*/

	FVector centroid, s;
	FVector p1 = verticies[0];
	FVector p2 = verticies[1];
	float totalArea = 0.0;
	for (int i = 2; i < verticies.Num(); i++)
	{
		FVector p3 = verticies[i];
		FVector edge1 = p3 - p1;
		FVector edge2 = p3 - p2;
		FVector crossProduct = FVector::CrossProduct(edge1, edge2);
		float area = crossProduct.Size() / 2;
		s.X += area * ((p1.X + p2.X + p3.X) / 3);
		s.Y += area * ((p1.Y + p2.Y + p3.Y) / 3);
		s.Z += area * ((p1.Z + p2.Z + p3.Z) / 3);

		totalArea += area;
		p2 = p3;
	}
	centroid.X = s.X / totalArea;
	centroid.Y = s.Y / totalArea;
	centroid.Z = s.Z / totalArea;

	return centroid;

}
void AJsonParser::CalculatePositions(FVector RotationAxis, float Angle)
{
	/*
	*  ------------- Z-Axis --------------------
	x*cosTheta - y*sinTheta = x'
	x*sinTheta + y*cosTheta = y'
	z = z'
	 --------------- Y-Axis ---------------------
	x*cosTheta + z*sinTheta = x'
	y = y'
	-x*sinTheta + z*cosTheta = z
	 --------------- X-Axis ---------------------
	x = x'
	y*cosTheta - z*cosTheta = y'
	y*sinTheta - z*cosTheta = z'
	*/
	if (RotationAxis.X != 0.f)
	{
		NewItem1.X = Item1.X;
		NewItem1.Y = (Item1.Z * FMath::Cos(Angle)) - (Item1.Y * FMath::Cos(Angle));
		NewItem1.Z = (Item1.Z * FMath::Sin(Angle)) - (Item1.Y * FMath::Cos(Angle));
		NewItem2.X = Item2.X;
		NewItem2.Y = (Item2.Z * FMath::Cos(Angle)) - (Item2.Y * FMath::Cos(Angle));
		NewItem2.Z = (Item2.Z * FMath::Sin(Angle)) - (Item2.Y * FMath::Cos(Angle));
		NewItem3.X = Item3.X;
		NewItem3.Y =(Item3.Z * FMath::Cos(Angle)) - (Item3.Y * FMath::Cos(Angle));
		NewItem3.Z = (Item3.Z * FMath::Sin(Angle)) - (Item3.Y * FMath::Cos(Angle));

	}
	if (RotationAxis.Y != 0.f)
	{
		// Y is z axis in ue4
		NewItem1.X = (Item1.X * FMath::Cos(Angle)) - (Item1.Z * FMath::Sin(Angle));
		NewItem1.Y = Item1.Y; // this is Z
		NewItem1.Z = (Item1.X * FMath::Sin(Angle)) + (Item1.Z * FMath::Sin(Angle));
		NewItem2.X = (Item2.X * FMath::Cos(Angle)) - (Item2.Z * FMath::Sin(Angle));
		NewItem2.Y = Item2.Y; // this is Z
		NewItem2.Z = (Item2.X * FMath::Sin(Angle)) + (Item2.Z * FMath::Sin(Angle));
		NewItem3.X = (Item3.X * FMath::Cos(Angle)) - (Item3.Z * FMath::Sin(Angle));
		NewItem3.Y = Item3.Y; // this is Z
		NewItem3.Z = (Item3.X * FMath::Sin(Angle)) + (Item3.Z * FMath::Sin(Angle));

	}
	if (RotationAxis.Z != 0.f)
	{
		// Z is Y axis in ue4
		NewItem1.X = (Item1.X * FMath::Cos(Angle)) + (Item1.Y * FMath::Sin(Angle));
		NewItem1.Y = (-Item1.X * FMath::Sin(Angle)) + (Item1.Y * FMath::Cos(Angle));
		NewItem1.Z = Item1.Z;// this is Y axis 
		NewItem2.X = (Item2.X * FMath::Cos(Angle)) + (Item2.Y * FMath::Sin(Angle));
		NewItem2.Y = (-Item2.X * FMath::Sin(Angle)) + (Item2.Y * FMath::Cos(Angle));
		NewItem2.Z = Item2.Z;// this is Y axis
		NewItem3.X = (Item3.X * FMath::Cos(Angle)) + (Item3.Y * FMath::Sin(Angle));
		NewItem3.Y = (-Item3.X * FMath::Sin(Angle)) + (Item3.Y * FMath::Cos(Angle));
		NewItem3.Z = Item3.Z;// this is Y axis

	}

}
FVector AJsonParser::CalculateZRotation(float Angle)
{
	FVector RotVec;
	// Z is Y axis in ue4
	RotVec.X = (Item1.X * FMath::Cos(Angle)) + (Item1.Y * FMath::Sin(Angle));
	RotVec.Y = (-Item1.X * FMath::Sin(Angle)) + (Item1.Y * FMath::Cos(Angle));
	RotVec.Z = Item1.Z;// this is Y axis 
	return RotVec;
}
FVector AJsonParser::CalculateYRotation(float Angle)
{
	FVector RotVec;
	// Y is z axis in ue4
	RotVec.X = (Item1.X * FMath::Cos(Angle)) - (Item1.Z * FMath::Sin(Angle));
	RotVec.Y = Item1.Y; // this is Z
	RotVec.Z = (Item1.X * FMath::Sin(Angle)) + (Item1.Z * FMath::Sin(Angle));
	return RotVec;
	
}
FVector AJsonParser::CalculateXRotation(float Angle)
{
	FVector RotVec;
	RotVec.X = Item1.X;
	RotVec.Y = (Item1.Z * FMath::Cos(Angle)) - (Item1.Y * FMath::Cos(Angle));
	RotVec.Z = (Item1.Z * FMath::Sin(Angle)) - (Item1.Y * FMath::Cos(Angle));
	return RotVec;
}
/*
	[1,0,0,0]
	[0,1,0,0]
	[0,0,1,0]
	[0,0,0,1] - Identity Matrix

	[Sx,0,0,0]
	[0,Sy,0,0]
	[0,0,Sz,0]
	[0,0,0,1] - Scale matrix

	[1,0,0,0]
	[0,1,0,0]
	[0,0,1,0]
	[Tx,Ty,Tz,1]- Translation Matrix - Tx,Ty,Tz - new positions

	[1,0    ,0]
	[0,cos(),-sin()]
	[0,sin(),cos()] = Rx(@) Rotation for the X axis

	[cos(),0,sin()]
	[0    ,1,0]
	[-sin(),0,cos()] = Ry() Rotation for Y axis

	[cos(),-sin(),0]
	[sin(),cos(),0]
	[0    ,0    ,1]	= Rz() Rotation for Z axis

*/

