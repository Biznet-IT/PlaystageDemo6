// Biznet It 2024


#include "ReplayListItem.h"
#include "ReplayMenuWidget.h"
#include "Components/TextBlock.h"


void UReplayListItem::OnPlayClicked()
{
    UReplayMenuWidget* ParentMenu = Cast<UReplayMenuWidget>(GetParent());
    if (ParentMenu)
    {
        ParentMenu->PlayReplay(ReplayName);
    }
}

void UReplayListItem::OnRenameClicked()
{
    UReplayMenuWidget* ParentMenu = Cast<UReplayMenuWidget>(GetParent());
    if (ParentMenu)
    {
        // Logic to prompt user for new name and rename the replay
        FString NewName = "NewReplayName"; // Replace with actual new name input by the user
        ParentMenu->RenameReplay(ReplayName, NewName);
    }
}

void UReplayListItem::OnDeleteClicked()
{
    UReplayMenuWidget* ParentMenu = Cast<UReplayMenuWidget>(GetParent());
    if (ParentMenu)
    {
        ParentMenu->DeleteReplay(ReplayName);
    }
}
