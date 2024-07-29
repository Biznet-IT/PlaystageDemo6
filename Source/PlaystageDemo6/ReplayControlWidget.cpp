// Biznet It 2024


#include "ReplayControlWidget.h"
#include "GameFramework/PlayerController.h"
#include "ReplayPlayerController.h"
#include "ReplayGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"
#include "Camera/CameraActor.h"

void UReplayControlWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

//camera functions
/*
void UReplayControlWidget::SetCameraPosition(FVector Location, FRotator Rotation)
{
	// set the cameraPawn position and rotation
	if (ReplayPlayerController)
	{
		//ReplayPlayerController->SetCameraPosition(Location, Rotation);
		ADefaultPawn* CameraPawn = ReplayPlayerController->GetCameraPawn();
		if (CameraPawn)
		{
			CameraPawn->SetActorLocation(Location);
			CameraPawn->SetActorRotation(Rotation);
		}
	}
}*/

//replay control functions
void UReplayControlWidget::PlayReplay()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        PlayerController->ConsoleCommand(TEXT("DemoPlay"), true);
    }
}

void UReplayControlWidget::PauseReplay()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        PlayerController->ConsoleCommand(TEXT("DemoPause"), true);
    }
}

void UReplayControlWidget::FastForwardReplay()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        PlayerController->ConsoleCommand(TEXT("DemoFastForward"), true);
    }
}

void UReplayControlWidget::RewindReplay()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        PlayerController->ConsoleCommand(TEXT("DemoRewind"), true);
    }
}

void UReplayControlWidget::RestartReplay()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        PlayerController->ConsoleCommand(TEXT("DemoRestart"), true);
    }
}

void UReplayControlWidget::CreateEditableCamera()
{
    // Implement logic to create and place an editable camera at the current replay time
    if (UWorld* World = GetWorld())
    {
        FVector CameraLocation = World->GetFirstPlayerController()->GetPawn()->GetActorLocation();
        FRotator CameraRotation = World->GetFirstPlayerController()->GetPawn()->GetActorRotation();

        ACameraActor* NewCamera = World->SpawnActor<ACameraActor>(CameraLocation, CameraRotation);
        // Additional logic to store and manage the new camera actor
    }
}

void UReplayControlWidget::SeekReplay(float Time)
{
    if (UWorld* World = GetWorld())
    {
        if (UDemoNetDriver* DemoNetDriver = World->GetDemoNetDriver())
        {
            DemoNetDriver->GotoTimeInSeconds(Time);
        }
    }
}

void UReplayControlWidget::OnCameraSelected(int32 CameraIndex)
{
    if (ReplayPlayerController)
    {
		UReplayGameInstance* GI = Cast<UReplayGameInstance>(UGameplayStatics::GetGameInstance(this));
        if (GI)
        {
			const TArray<FS_CameraInfo>& CameraList = GI->GetCameraList();
            /*if (CameraList.IsValidIndex(CameraIndex))
            {
				FS_CameraInfo CameraInfo = CameraList[CameraIndex];
				SetCameraPosition(CameraInfo.Location, CameraInfo.Rotation);
            }*/
        }
	}
}