// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
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

/**
 * 
 */
UCLASS(Blueprintable)
class QJAM2T10_API AGoalGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGoalGameModeBase();

	virtual void StartPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLeaderboardUserInfoItem> LeaderboardEntries;

	UFUNCTION(BlueprintCallable)
	void TryToUpdateLeaderboardEntries();
	UFUNCTION(BlueprintCallable)
	void TryToAddAnotherEntryInGlobalLeaderboard(FLeaderboardUserInfoItem LeaderboardEnty);

private:
	void OnResponseReceivedSample(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully);

	void OnResponseReceivedGetHighScores(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully);
	void OnResponseReceivedPostHighScore(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccesfully);
};
