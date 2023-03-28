// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Math/UnrealMathUtility.h"
#include "Http.h"
#include "Json.h"
#include "GoalGameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FLeaderboardUserInfoItem
{
	GENERATED_USTRUCT_BODY()

public:
	FLeaderboardUserInfoItem() = default;
    //~ The following member variable will be accessible by Blueprint Graphs:
    // This is the tooltip for our test variable.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Variables")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Variables")
    int32 Score;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveResponse, bool, succesfully);

UCLASS(Blueprintable)
class QJAM2T10_API AGoalGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGoalGameModeBase();
	virtual void StartPlay() override;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnReceiveResponse OnResponseReceivedGetHighScoresDelegate;
	//UPROPERTY(BlueprintCallable, BlueprintAssignable)
	//FOnReceiveResponse OnResponseReceivedPostHighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLeaderboardUserInfoItem> LeaderboardEntries;

	UFUNCTION(BlueprintCallable)
	void TryToUpdateLeaderboardEntries();
	UFUNCTION(BlueprintCallable)
	void TryToAddAnotherEntryInGlobalLeaderboard(FLeaderboardUserInfoItem LeaderboardEnty);

	// Dummy so far
	UFUNCTION(BlueprintImplementableEvent)
		void ResponseReceivedGetHighScores();

	//// Dummy so far
	//UFUNCTION(BlueprintImplementableEvent)
		//void ResponseReceivedPostHighScore();

private:
	void OnResponseReceivedGetHighScores(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully);
	void OnResponseReceivedPostHighScore(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully);
};
