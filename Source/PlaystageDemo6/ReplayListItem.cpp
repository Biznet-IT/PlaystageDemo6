// Biznet It 2024


#include "ReplayListItem.h"
#include "ReplayMenuWidget.h"
#include "Components/TextBlock.h"
//include panel widget
#include "Components/PanelWidget.h"
#include "Components/EditableTextBox.h"


void UReplayListItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (EditableTextBox_ReplayName)
    {
        EditableTextBox_ReplayName->SetText(FText::FromString(ReplayInfo.FriendlyName));
    }
}

void UReplayListItem::OnPlayClicked()
{
    /*
    UReplayMenuWidget* ParentMenu = Cast<UReplayMenuWidget>(GetParent());
    if (ParentMenu)
    {
        ParentMenu->PlayReplay(ReplayName);
    }
    */
}

void UReplayListItem::OnRenameClicked()
{
    /*
    UReplayMenuWidget* ParentMenu = Cast<UReplayMenuWidget>(GetParent());
    if (ParentMenu)
    {
        // Logic to prompt user for new name and rename the replay
        FString NewName = "NewReplayName"; // Replace with actual new name input by the user
        ParentMenu->RenameReplay(ReplayName, NewName);
    }
    */
}

void UReplayListItem::OnDeleteClicked()
{
    /*
    UReplayMenuWidget* ParentMenu = Cast<UReplayMenuWidget>(GetParent());
    if (ParentMenu)
    {
        ParentMenu->DeleteReplay(ReplayName);
    }
    */
}

void UReplayListItem::InitializeReplayItem(const FS_ReplayInfo& InReplayInfo)
{
    ReplayInfo = InReplayInfo;

    if (EditableTextBox_ReplayName)
    {
        EditableTextBox_ReplayName->SetText(FText::FromString(ReplayInfo.FriendlyName));
    }
}