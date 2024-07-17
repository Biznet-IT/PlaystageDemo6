// Biznet It 2024


#include "CharSelectWidget.h"
#include "ReplayPlayerController.h"

void UCharSelectWidget::SelectCharacter(int32 CharacterIndex)
{
    if (CharacterIndex >= 0 && CharacterIndex < CharacterInfos.Num())
    {
        AReplayPlayerController* PlayerController = Cast<AReplayPlayerController>(GetOwningPlayer());
        if (PlayerController)
        {
            PlayerController->ServerSelectCharacter(CharacterInfos[CharacterIndex]);
        }
    }
}