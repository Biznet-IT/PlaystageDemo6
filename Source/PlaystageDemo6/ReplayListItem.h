// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReplayGameInstance.h"
#include "ReplayListItem.generated.h"

/**
 * 
 */
 //Functional
UCLASS()
class PLAYSTAGEDEMO6_API UReplayListItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UEditableTextBox* EditableTextBox_ReplayName;

    UPROPERTY(BlueprintReadOnly, Category = "Replays")
    FS_ReplayInfo ReplayInfo;

    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OnPlayClicked();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OnRenameClicked();

    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OnDeleteClicked();

    void Initialize(const FS_ReplayInfo& InReplayInfo);
};
