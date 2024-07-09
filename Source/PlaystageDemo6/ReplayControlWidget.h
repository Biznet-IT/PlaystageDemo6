// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReplayControlWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO6_API UReplayControlWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void PlayReplay();

    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void PauseReplay();

    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void FastForwardReplay();

    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void RewindReplay();

    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void RestartReplay();

    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void CreateEditableCamera();

    UFUNCTION(BlueprintCallable, Category = "Replay Control")
    void SeekReplay(float Time);
};
};
