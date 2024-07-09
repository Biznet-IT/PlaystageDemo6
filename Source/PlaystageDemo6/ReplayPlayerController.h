// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ReplayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO6_API AReplayPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    //function to start recording a replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StartRecording();

    //function to stop recording a replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StopRecording();

    //function to play a replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PlayReplay(const FString& ReplayName);

private:
    FString GetTimestamp() const;
};
