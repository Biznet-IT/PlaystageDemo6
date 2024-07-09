// Biznet It 2024


#include "ReplayMenuWidget.h"
#include "ReplayPlayerController.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UReplayMenuWidget::StartRecording()
{
    if (AReplayPlayerController* PlayerController = Cast<AReplayPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PlayerController->StartRecording();
    }
}

void UReplayMenuWidget::StopRecording()
{
    if (AReplayPlayerController* PlayerController = Cast<AReplayPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PlayerController->StopRecording();
    }
}

TArray<FString> UReplayMenuWidget::ShowReplayList()
{
    TArray<FString> ReplayList = GetReplayList();
	return ReplayList;
    // Populate your ScrollBox or ListView with ReplayList
}

TArray<FString> UReplayMenuWidget::GetReplayList()
{
    TArray<FString> ReplayList;
    IFileManager& FileManager = IFileManager::Get();
    FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
    FileManager.FindFiles(ReplayList, *ReplayDirectory, TEXT("*.demo"));
    return ReplayList;
}

void UReplayMenuWidget::SearchReplay(const FString& SearchText)
{
    TArray<FString> ReplayList = GetReplayList();
    ReplayList.RemoveAll([&SearchText](const FString& Replay) {
        return !Replay.Contains(SearchText);
        });
    // Populate your ScrollBox or ListView with filtered ReplayList
}

void UReplayMenuWidget::PlayReplay(const FString& ReplayName)
{
	FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
	FString ReplayPath = ReplayDirectory / ReplayName;
	if (AReplayPlayerController* PlayerController = Cast<AReplayPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->PlayReplay(ReplayPath);
	}
}

void UReplayMenuWidget::RenameReplay(const FString& OldName, const FString& NewName)
{
    FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
    FString OldPath = ReplayDirectory / OldName;
    FString NewPath = ReplayDirectory / NewName;
    IFileManager::Get().Move(*NewPath, *OldPath);
}

void UReplayMenuWidget::DeleteReplay(const FString& ReplayName)
{
    FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
    FString ReplayPath = ReplayDirectory / ReplayName;
    IFileManager::Get().Delete(*ReplayPath);
}
