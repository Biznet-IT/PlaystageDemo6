// Biznet It 2024

#pragma once
#include "ReplayPlayerController.h"
#include "Engine/World.h"
#include "Misc/DateTime.h"

FString AReplayPlayerController::GetTimestamp() const
{
    FDateTime Now = FDateTime::Now();
    return Now.ToString(TEXT("%Y%m%d%H%M%S"));
}

void AReplayPlayerController::StartRecording()
{
    UWorld* World = GEngine->GameViewport->GetWorld();
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (PlayerController)
    {
        FString ReplayName = FString::Printf(TEXT("Replay_%s"), *GetTimestamp());
        PlayerController->ConsoleCommand(FString::Printf(TEXT("DemoRec %s"), *ReplayName), true);
    }
}

void AReplayPlayerController::StopRecording()
{
    UWorld* World = GEngine->GameViewport->GetWorld();
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->ConsoleCommand("DemoStop", true);
    }
}

void AReplayPlayerController::PlayReplay(const FString& ReplayName)
{
    UWorld* World = GEngine->GameViewport->GetWorld();
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->ConsoleCommand(FString::Printf(TEXT("DemoPlay %s"), *ReplayName), true);
    }
}

void AReplayPlayerController::RenameReplay(const FString& OldReplayName, const FString& NewReplayName)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->ConsoleCommand(FString::Printf(TEXT("RenameDemo %s %s"), *OldReplayName, *NewReplayName), true);
	}
}

void AReplayPlayerController::DeleteReplay(const FString& ReplayName)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->ConsoleCommand(FString::Printf(TEXT("DeleteDemo %s"), *ReplayName), true);
	}
}