// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReplayMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO6_API UReplayMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StartRecording();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StopRecording();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void ShowReplayList();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void SearchReplay(const FString& SearchText);

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void RenameReplay(const FString& OldName, const FString& NewName);

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void DeleteReplay(const FString& ReplayName);

private:
    UFUNCTION(BlueprintCallable, Category = "Replays")
    TArray<FString> GetReplayList();
};
