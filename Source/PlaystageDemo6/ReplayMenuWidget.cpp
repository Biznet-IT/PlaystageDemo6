// Biznet It 2024


#include "ReplayMenuWidget.h"
#include "ReplayPlayerController.h"
#include "ReplayListItem.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Components/ListView.h"
#include "Components/ScrollBox.h"
#include "ReplayGameInstance.h"


void UReplayMenuWidget::PopulateReplayList()
{
    if (!ScrollBox_Replays) return;

    ScrollBox_Replays->ClearChildren();

    UReplayGameInstance* GameInstance = Cast<UReplayGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        TArray<FS_ReplayInfo> ReplayInfos;
        GameInstance->GetReplayInfo(ReplayInfos);

        for (const FS_ReplayInfo& ReplayInfo : ReplayInfos)
        {
            UReplayListItem* ListItem = CreateWidget<UReplayListItem>(this, UReplayListItem::StaticClass());
            ListItem->InitializeReplayItem(ReplayInfo);
            ScrollBox_Replays->AddChild(ListItem);
        }
    }
}


void UReplayMenuWidget::PlayReplay(const FString& ReplayName)
{
    // Call function to play replay using the replay name
}

void UReplayMenuWidget::RenameReplay(const FString& OldReplayName, const FString& NewReplayName)
{
    // Call function to rename the replay
}

void UReplayMenuWidget::DeleteReplay(const FString& ReplayName)
{
    // Call function to delete the replay
}

void UReplayMenuWidget::SearchReplays(const FString& SearchText)
{
    if (!ScrollBox_Replays) return;

    ScrollBox_Replays->ClearChildren();

    UReplayGameInstance* GameInstance = Cast<UReplayGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        TArray<FS_ReplayInfo> ReplayInfos;
        GameInstance->GetReplayInfo(ReplayInfos);

        for (const FS_ReplayInfo& ReplayInfo : ReplayInfos)
        {
            if (ReplayInfo.Name.Contains(SearchText) || ReplayInfo.FriendlyName.Contains(SearchText))
            {
                UReplayListItem* ListItem = CreateWidget<UReplayListItem>(this, UReplayListItem::StaticClass());
                ListItem->InitializeReplayItem(ReplayInfo);
                ScrollBox_Replays->AddChild(ListItem);
            }
        }
    }
}