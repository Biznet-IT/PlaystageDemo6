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
    UPROPERTY(meta = (BindWidget))
    class UListView* ListView_Replays;

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PopulateReplayList();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PlayReplay(const FString& ReplayName);

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void RenameReplay(const FString& OldReplayName, const FString& NewReplayName);

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void DeleteReplay(const FString& ReplayName);

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void SearchReplays(const FString& SearchText);
};
