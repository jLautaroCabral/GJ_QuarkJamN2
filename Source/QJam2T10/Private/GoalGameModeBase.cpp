// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalGameModeBase.h"

AGoalGameModeBase::AGoalGameModeBase()
{
}

// Get leaderboard data and store it inside LeaderboardEntries
void AGoalGameModeBase::OnResponseReceivedGetHighScores(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully)
{
	if (!bConnectedSuccesfully)
	{
		switch (Request->GetStatus()) {
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Error, TEXT("Connection failed."));
		default:
			UE_LOG(LogTemp, Error, TEXT("Request failed."));
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Response %s"), *Response->GetContentAsString());
	FString ResponseContent = *Response->GetContentAsString();

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseContent);
	TSharedPtr<FJsonObject> OutObject;
	TArray<TSharedPtr<FJsonValue>> FirebaseDocumentsArray;

	UE_LOG(LogTemp, Display, TEXT("======= TRYING TO DESEARIALIZE"));
	if (FJsonSerializer::Deserialize(JsonReader, OutObject))
	{
		FLeaderboardUserInfoItem newItem;
		const TArray<TSharedPtr<FJsonValue>> documents = OutObject->GetArrayField(TEXT("documents"));

		for (int i = 0; i < documents.Num(); ++i)
		{
			const TSharedPtr<FJsonObject> item = documents[i]->AsObject();

			if (item)
			{
				TSharedPtr<FJsonObject> documentFields = item->GetObjectField(TEXT("fields"));
				TSharedPtr<FJsonObject> documentUsernameField = documentFields->GetObjectField(TEXT("username"));
				UE_LOG(LogTemp, Display, TEXT("Player N%i; Username %s"), i, *documentUsernameField->GetStringField("stringValue"));
				newItem.Username = *documentUsernameField->GetStringField("stringValue");
				LeaderboardEntries.Add(newItem);
			}
		}

		UE_LOG(LogTemp, Display, TEXT("===================================== END GET REQUEST"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize Leaderboard response"));
	}

	if (OnResponseReceivedPostHighScoreDelegate.IsBound())
	{
		OnResponseReceivedPostHighScoreDelegate.Broadcast(bConnectedSuccesfully);
	}
}

void AGoalGameModeBase::OnResponseReceivedPostHighScore(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully)
{
	if (bConnectedSuccesfully)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *Response->GetContentAsString());

		if (OnResponseReceivedGetHighScoresDelegate.IsBound())
		{
			OnResponseReceivedGetHighScoresDelegate.Broadcast(bConnectedSuccesfully);
		}

		UE_LOG(LogTemp, Display, TEXT("===================================== END POST REQUEST"));
	}
}


void AGoalGameModeBase::TryToUpdateLeaderboardEntries()
{
	// Perform GET Request
	UE_LOG(LogTemp, Display, TEXT("===================================== START REQUEST"));
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AGoalGameModeBase::OnResponseReceivedGetHighScores);
	Request->SetURL("https://firestore.googleapis.com/v1/projects/quarkjam2/databases/(default)/documents/leaderboard?orderBy=score%20desc&pageSize=5");
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AGoalGameModeBase::TryToAddAnotherEntryInGlobalLeaderboard(FLeaderboardUserInfoItem LeaderboardEntry)
{
	FHttpRequestRef RequestPost = FHttpModule::Get().CreateRequest();

	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetObjectField("fields", RequestObj);
	const TSharedPtr<FJsonObject> RequestObjFields = RequestObj->GetObjectField("fields");
	RequestObjFields->SetObjectField("score", RequestObjFields);

	TSharedRef<FJsonObject> rootObject = MakeShared<FJsonObject>();
	TSharedRef<FJsonObject> fieldsObject = MakeShared<FJsonObject>();
	TSharedRef<FJsonObject> scoreObject = MakeShared<FJsonObject>();
	TSharedRef<FJsonObject> nameObject = MakeShared<FJsonObject>();

	nameObject->SetStringField(TEXT("stringValue"), LeaderboardEntry.Username);
	scoreObject->SetNumberField(TEXT("integerValue"), LeaderboardEntry.Score);

	fieldsObject->SetObjectField(TEXT("score"), scoreObject);
	fieldsObject->SetObjectField(TEXT("username"), nameObject);
	rootObject->SetObjectField(TEXT("fields"), fieldsObject);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(rootObject, Writer);

	FString URL = FString::Printf(TEXT("https://firestore.googleapis.com/v1/projects/quarkjam2/databases/(default)/documents/leaderboard?documentId=%s"), *LeaderboardEntry.Username);

	RequestPost->OnProcessRequestComplete().BindUObject(this, &AGoalGameModeBase::OnResponseReceivedPostHighScore);
	RequestPost->SetURL(URL);
	RequestPost->SetVerb("POST");
	RequestPost->SetHeader("Content-Type", "application/json");
	RequestPost->SetContentAsString(RequestBody);
	RequestPost->ProcessRequest();
}

void AGoalGameModeBase::StartPlay()
{
	Super::StartPlay();
}