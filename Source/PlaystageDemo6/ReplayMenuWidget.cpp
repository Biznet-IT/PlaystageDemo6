// Biznet It 2024


#include "ReplayMenuWidget.h"
#include "ReplayPlayerController.h"
#include "ReplayListItem.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Components/ListView.h"


void UReplayMenuWidget::PopulateReplayList()
{
    if (!ListView_Replays) return;

    ListView_Replays->ClearListItems();

    IFileManager& FileManager = IFileManager::Get();
    FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
    TArray<FString> ReplayFiles;
    FileManager.FindFiles(ReplayFiles, *ReplayDirectory, TEXT("*.demo"));

    for (const FString& ReplayFile : ReplayFiles)
    {
        UReplayListItem* ListItem = CreateWidget<UReplayListItem>(this, UReplayListItem::StaticClass());
        ListItem->ReplayName = FPaths::GetBaseFilename(ReplayFile);
        ListView_Replays->AddItem(ListItem);
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
    if (!ListView_Replays) return;

    ListView_Replays->ClearListItems();

    IFileManager& FileManager = IFileManager::Get();
    FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
    TArray<FString> ReplayFiles;
    FileManager.FindFiles(ReplayFiles, *ReplayDirectory, TEXT("*.demo"));

    for (const FString& ReplayFile : ReplayFiles)
    {
        if (ReplayFile.Contains(SearchText))
        {
            UReplayListItem* ListItem = CreateWidget<UReplayListItem>(this, UReplayListItem::StaticClass());
            ListItem->ReplayName = FPaths::GetBaseFilename(ReplayFile);
            ListView_Replays->AddItem(ListItem);
        }
    }
}