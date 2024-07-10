// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ReplayGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FS_ReplayInfo
{
    GENERATED_BODY()

    FS_ReplayInfo() {}
    FS_ReplayInfo(FString NewName, FString NewFriendlyName, FDateTime NewTimestamp, float NewLengthInMS)
        : Name(NewName), FriendlyName(NewFriendlyName), Timestamp(NewTimestamp), LengthInMS(NewLengthInMS) {}

    UPROPERTY(BlueprintReadOnly, Category = "Replay")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Replay")
    FString FriendlyName;

    UPROPERTY(BlueprintReadOnly, Category = "Replay")
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadOnly, Category = "Replay")
    float LengthInMS;
};

UCLASS()
class PLAYSTAGEDEMO6_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PopulateReplayList();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void GetReplayInfo(TArray<FS_ReplayInfo>& OutReplayInfos);
};
