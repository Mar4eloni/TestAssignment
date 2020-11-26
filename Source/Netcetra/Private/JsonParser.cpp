// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParser.h"
#include "JsonUtilities.h"
#include "Items/BallPoint.h"
#include "DrawDebugHelpers.h"
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
	TArray<FVector2D> verticies = {
		FVector2D(ReferencePoint1.X,ReferencePoint1.Z),
		FVector2D(ReferencePoint2.X,ReferencePoint2.Z),
		FVector2D(Item1.X,Item1.Z),
		FVector2D(Item2.X,Item2.Z),
		FVector2D(Item3.X,Item3.Z) };
	TArray<FVector2D> verticies1 = {
		FVector2D(ReferencePoint1.X,ReferencePoint1.Y),
		FVector2D(ReferencePoint2.X,ReferencePoint2.Y),
		FVector2D(Item1.X,Item1.Y),
		FVector2D(Item2.X,Item2.Y),
		FVector2D(Item3.X,Item3.Y) };

	CenterOfPolygonXZ = CalculateCenterOfPolygon(verticies, 5);
	CenterOfPolygonXZ.Normalize();
	CenterOfPolygonXY = CalculateCenterOfPolygon(verticies1, 5);
	CenterOfPolygonXY.Normalize();

	DiferenceItem1FromRef1.X = CenterOfPolygonXZ.X - Item1.X;
	DiferenceItem1FromRef1.Y = CenterOfPolygonXY.Y - Item1.Y;
	DiferenceItem1FromRef1.Z = CenterOfPolygonXZ.Y - Item1.Z;

	DiferenceItem2FromRef1.X = CenterOfPolygonXZ.X - Item2.X;
	DiferenceItem2FromRef1.Y = CenterOfPolygonXY.Y - Item2.Y;
	DiferenceItem2FromRef1.Z = CenterOfPolygonXZ.Y - Item2.Z;

	DiferenceItem3FromRef1.X = CenterOfPolygonXZ.X - Item3.X;
	DiferenceItem3FromRef1.Y = CenterOfPolygonXY.Y - Item3.Y;
	DiferenceItem3FromRef1.Z = CenterOfPolygonXZ.Y - Item3.Z;

	CenterOfPolygon.X = CenterOfPolygonXZ.X;
	CenterOfPolygon.Y = CenterOfPolygonXY.Y;
	CenterOfPolygon.Z = CenterOfPolygonXZ.Y;

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


	/*FVector RotationAxis = NewReferencePoint2 - NewReferencePoint1;
	RotationAxis.Normalize();
	FRotator* Rotator1 = new FRotator(RotationAxis.Y, RotationAxis.Z, RotationAxis.X);*/

	/*FVector temp;
	temp.X = CenterOfPolygon.X;
	temp.Y = 0.f;
	temp.Z = CenterOfPolygon.Y;*/


	FQuat BetweenQuat = FQuat::FindBetweenVectors(ReferencePoint1, CenterOfPolygon);//NewReferencePoint1);
	float Ref1Angle = 0.0f;
	FVector RotationAxisRef1;
	BetweenQuat.ToAxisAndAngle(RotationAxisRef1, Ref1Angle);
	//FRotator rot = FRotator::vec
	RotationAxisRef1.Normalize();

	FQuat BetweenQuat2 = FQuat::FindBetweenVectors(ReferencePoint2, CenterOfPolygon);
	float Ref2Angle = 0.0f;
	FVector RotationAxisRef2;
	BetweenQuat2.ToAxisAndAngle(RotationAxisRef2, Ref2Angle);

	FQuat BetweenQuatItem1 = FQuat::FindBetweenVectors(Item1, CenterOfPolygon);
	float Item1Angle = 0.0f;
	FVector RotationAxisItem1;
	BetweenQuat2.ToAxisAndAngle(RotationAxisItem1, Item1Angle);

	FQuat BetweenQuatItem2 = FQuat::FindBetweenVectors(Item2, CenterOfPolygon);
	float Item2Angle = 0.0f;
	FVector RotationAxisItem2;
	BetweenQuat2.ToAxisAndAngle(RotationAxisItem2, Item2Angle);

	FQuat BetweenItem3 = FQuat::FindBetweenVectors(Item3, CenterOfPolygon);
	float Item3Angle = 0.0f;
	FVector RotationAxisItem3;
	BetweenQuat2.ToAxisAndAngle(RotationAxisItem3, Item3Angle);

	FQuat BetweenNewRefP = FQuat::FindBetweenVectors(NewReferencePoint1, CenterOfPolygon);
	float NewRefPAngle = 0.0f;
	FVector RotationAxisNewRefP;
	BetweenQuat2.ToAxisAndAngle(RotationAxisNewRefP, NewRefPAngle);
	FQuat BetweenNewRefP2 = FQuat::FindBetweenVectors(NewReferencePoint2, CenterOfPolygon);
	float NewRef2PAngle = 0.0f;
	FVector RotationAxisNewRefP2;
	BetweenQuat2.ToAxisAndAngle(RotationAxisNewRefP2, NewRef2PAngle);


	/*NewItem1 = (RotationAxisNewRefP * (Item1 - CenterOfPolygon)) + CenterOfPolygon;
	NewItem2 = (RotationAxisNewRefP * (Item2 - CenterOfPolygon)) + CenterOfPolygon;
	NewItem3 = (RotationAxisNewRefP * (Item3 - CenterOfPolygon)) + CenterOfPolygon;*/


	FMatrix mat = FMatrix(
		FPlane(FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle)), -FMath::Sin(FMath::RadiansToDegrees(NewRefPAngle)), 0.f, 0.f),
		FPlane(FMath::Sin(FMath::RadiansToDegrees(NewRefPAngle)), FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle)), 0, 0),
		FPlane(0, 0, 1, 0),
		FPlane(0, 0, 0, 1)
	);
	//Rodriges rotation equation
	/*NewItem1 = (Item1 * FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle))) +
		((RotationAxisNewRefP * Item1) * FMath::Sin(FMath::RadiansToDegrees(NewRefPAngle))) +
		(RotationAxisNewRefP * (RotationAxisNewRefP * Item1)) * (1 - FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle)));
	FVector temp2 = Item2 * FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle));
	FVector temp3 = Item3 * FMath::Cos(FMath::RadiansToDegrees(NewRefPAngle));*/
	//https://math.stackexchange.com/questions/1917449/rotate-polygon-around-center-and-get-the-coordinates/1917485
	FVector temp1 = Item1 - CenterOfPolygon;
	FVector temp2 = Item2 - CenterOfPolygon;
	FVector temp3 = Item3 - CenterOfPolygon;

	NewItem1 = (RotationAxisNewRefP * temp1) + CenterOfPolygon;
	NewItem2 = (RotationAxisNewRefP2 * temp2) + CenterOfPolygon;
	NewItem3 = (RotationAxisNewRefP * temp3) + CenterOfPolygon;


	/*FQuat pitchQuat(RotationAxisRef1, Angle);
	BetweenQuat *= pitchQuat;
	BetweenQuat.Normalize();
	BetweenQuat.ToAxisAndAngle(RotationAxisRef1, Angle);*/



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


}
FVector2D AJsonParser::CalculateCenterOfPolygon(const TArray<FVector2D> verticies, int vertexCount)
{
	FVector2D centroid = { 0, 0 };
	double signedArea = 0.0;
	double x0 = 0.0; // Current vertex X
	double y0 = 0.0; // Current vertex Y
	double x1 = 0.0; // Next vertex X
	double y1 = 0.0; // Next vertex Y
	double a = 0.0;  // Partial signed area

	// For all vertices
	int i = 0;
	for (i = 0; i < vertexCount; ++i)
	{
		// Z is up in Unreal Engine ...
		x0 = verticies[i].X;
		y0 = verticies[i].Y;
		x1 = verticies[(i + 1) % vertexCount].X;
		y1 = verticies[(i + 1) % vertexCount].Y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid.X += (x0 + x1) * a;
		centroid.Y += (y0 + y1) * a;
	}

	signedArea *= 0.5;
	centroid.X /= (6.0 * signedArea);
	centroid.Y /= (6.0 * signedArea);

	return centroid;
}
//FMatrix::
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
	//FVector::RotateAngleAxis

