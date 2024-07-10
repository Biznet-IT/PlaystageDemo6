// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReplayListItem.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO6_API UReplayListItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly, Category = "Replays")
    FString ReplayName;

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OnPlayClicked();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OnRenameClicked();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OnDeleteClicked();
};
