// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "ReplayCharacter.h"
#include "Blueprint/UserWidget.h"
#include "CharSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO6_API UCharSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Character Selection")
    void SelectCharacter(int32 CharacterIndex);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Selection")
    TArray<FCharacterInfo> CharacterInfos;

    //virtual void NativeConstruct() override;
};
