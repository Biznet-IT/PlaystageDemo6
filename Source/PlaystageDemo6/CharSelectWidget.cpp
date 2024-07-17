// Biznet It 2024


#include "CharSelectWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"
#include "ReplayPlayerController.h"

void UCharSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Inicializar la lista de personajes filtrados con todos los personajes
    FilteredCharacterInfos = CharacterInfos;
    //UpdateCharacterList();
}

// Actualizar la lista de personajes
//void UCharSelectWidget::UpdateCharacterList()
//{ }

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

void UCharSelectWidget::FilterCharactersByName(const FString& Name)
{
    FilteredCharacterInfos.Empty();
    for (const FCharacterInfo& Info : CharacterInfos)
    {
        if (Info.Name.Contains(Name))
        {
            FilteredCharacterInfos.Add(Info);
        }
    }
    //UpdateCharacterList();
}

void UCharSelectWidget::FilterCharactersByTag(const FString& Tag)
{
    FilteredCharacterInfos.Empty();
    for (const FCharacterInfo& Info : CharacterInfos)
    {
        if (Info.Tags.Contains(Tag))
        {
            FilteredCharacterInfos.Add(Info);
        }
    }
    //UpdateCharacterList();
}