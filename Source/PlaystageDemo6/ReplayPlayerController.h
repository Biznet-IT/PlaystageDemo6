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
    FString StartRecording();

    //function to stop recording a replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StopRecording();

    //function to play a replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PlayReplay(const FString& ReplayName);

	//function to rename a replay
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void RenameReplay(const FString& OldReplayName, const FString& NewReplayName);

	//function to delete a replay
	UFUNCTION(BlueprintCallable, Category = "Replays")  
	void DeleteReplay(const FString& ReplayName);

	AReplayPlayerController(const FObjectInitializer& ObjectInitializer);

	/* Set the Paused State of the Running Replay to bDoPause. Return new Pause State */
	UFUNCTION(BlueprintCallable, Category = "CurrentReplay")
	bool SetCurrentReplayPausedState(bool bDoPause);

	/* Gets the Max Number of Seconds that were recorded in the current Replay */
	UFUNCTION(BlueprintCallable, Category = "CurrentReplay")
	int32 GetCurrentReplayTotalTimeInSeconds() const;

	/* Gets the Second we are currently watching in the Replay */
	UFUNCTION(BlueprintCallable, Category = "CurrentReplay")
	int32 GetCurrentReplayCurrentTimeInSeconds() const;

	/* Jumps to the specified Second in the Replay we are watching */
	UFUNCTION(BlueprintCallable, Category = "CurrentReplay")
	void SetCurrentReplayTimeToSeconds(int32 Seconds);

	/* Changes the PlayRate of the Replay we are watching, enabling FastForward or SlowMotion */
	UFUNCTION(BlueprintCallable, Category = "CurrentReplay")
	void SetCurrentReplayPlayRate(float PlayRate = 1.f);

	/* Stop current Replay and return */
	UFUNCTION(BlueprintCallable, Category = "CurrentReplay")
	void StopCurrentReplay();

private:
    FString GetTimestamp() const;
};
