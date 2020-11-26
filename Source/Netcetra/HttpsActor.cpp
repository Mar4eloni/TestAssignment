// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpsActor.h"

void AHttpsActor::MyHttpCall()
{
	// json body for the sceneID
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	JsonObject->SetStringField(TEXT("sceneId"), TEXT("AMfICy52sInkFdwmQHR9gje"));
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);



	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Netcetera-Auth-Token"), TEXT("n4woc1HUxOGVoRO3UihJN2TRTYGHgw"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	//This is the url on which to process the request
	Request->SetURL("https://arstudio-test.azurewebsites.net/api/v2/scenes");
	// adding the body to the header
	Request->SetContentAsString(OutputString);
	Request->OnProcessRequestComplete().BindUObject(this, &AHttpsActor::OnResponseReceived);
	
	//Request->SetHeader(TEXT("Netcetera-Auth-Token"), "n4woc1HUxOGVoRO3UihJN2TRTYGHgw");
	
	
	Request->ProcessRequest();
}

void AHttpsActor::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (bWasSuccessful && FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		//Get the value of the json object by field name
		int32 recievedInt = JsonObject->GetIntegerField("sceneId");
		FString recevedObject = JsonObject->GetStringField("name"); //Reader->GetValueAsString();
		

		//Output it to the engine
		//GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::FromInt(recievedInt));
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, recevedObject);
	}
}

// Sets default values
AHttpsActor::AHttpsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
}

// Called when the game starts or when spawned
void AHttpsActor::BeginPlay()
{
	MyHttpCall();
	Super::BeginPlay();

	
}

// Called every frame
void AHttpsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

