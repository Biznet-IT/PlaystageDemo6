// Biznet It 2024

#include "ReplayPlayerController.h"
#include "Engine/World.h"
#include "Misc/DateTime.h"
#include "Engine/DemoNetDriver.h"
#include "Net/UnrealNetwork.h"
#include "ReplayCharacter.h"

FString AReplayPlayerController::GetTimestamp() const
{
    FDateTime Now = FDateTime::Now();
    return Now.ToString(TEXT("%Y%m%d%H%M%S"));
}

FString AReplayPlayerController::StartRecording()
{
    UWorld* World = GEngine->GameViewport->GetWorld();
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (PlayerController)
    {
        FString ReplayName = FString::Printf(TEXT("Replay_%s"), *GetTimestamp());
        PlayerController->ConsoleCommand(FString::Printf(TEXT("DemoRec %s"), *ReplayName), true);
		return ReplayName;
    }
	return "";
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



AReplayPlayerController::AReplayPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;
}

bool AReplayPlayerController::SetCurrentReplayPausedState(bool bDoPause)
{
	AWorldSettings* WorldSettings = GetWorldSettings();
	//Set MotionBlur off and Anti Aliasing to FXAA in order to bypass the pause-bug of both
	//static const auto CVarAA = IConsoleManager::Get().FindConsoleVariable(TEXT("DefaultFeature.AntiAliasing"));

	//static const auto CVarMB = IConsoleManager::Get().FindConsoleVariable(TEXT("DefaultFeature.MotionBlur"));

	if (bDoPause)
	{
		//PreviousAASetting = CVarAA->GetInt();
		//PreviousMBSetting = CVarMB->GetInt();

		// Set MotionBlur to OFF, Anti-Aliasing to FXAA
		//CVarAA->Set(1);
		//CVarMB->Set(0);

		WorldSettings->SetPauserPlayerState(PlayerState);
		return true;
	}
	// Rest MotionBlur and AA
	//CVarAA->Set(PreviousAASetting);
	//CVarMB->Set(PreviousMBSetting);

	WorldSettings->SetPauserPlayerState(NULL);
	return false;
}

int32 AReplayPlayerController::GetCurrentReplayTotalTimeInSeconds() const
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			return GetWorld()->GetDemoNetDriver()->GetDemoTotalTime();
		}
	}
	return 0.f;
}

int32 AReplayPlayerController::GetCurrentReplayCurrentTimeInSeconds() const
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			return GetWorld()->GetDemoNetDriver()->GetDemoCurrentTime();
		}
	}
	return 0.f;
}

void AReplayPlayerController::SetCurrentReplayTimeToSeconds(int32 Seconds)
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			GetWorld()->GetDemoNetDriver()->GotoTimeInSeconds(Seconds);
		}
	}
}

void AReplayPlayerController::SetCurrentReplayPlayRate(float PlayRate)
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			GetWorld()->GetWorldSettings()->DemoPlayTimeDilation = PlayRate;
		}
	}
}

void AReplayPlayerController::StopCurrentReplay()
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			GetWorld()->GetDemoNetDriver()->StopDemo();
		}
	}
}

void AReplayPlayerController::ServerSelectCharacter_Implementation(const FCharacterInfo& CharacterInfo)
{
	AReplayCharacter* PlayerCharacter = Cast<AReplayCharacter>(GetCharacter());
	if (PlayerCharacter)
	{
		PlayerCharacter->SetCharacterMeshAndAnimBP(CharacterInfo.Mesh, CharacterInfo.AnimBP);
	}
}

bool AReplayPlayerController::ServerSelectCharacter_Validate(const FCharacterInfo& CharacterInfo)
{
	return true; // Añade lógica de validación si es necesario
}

void AReplayPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AReplayPlayerController, SelectedCharacterInfo);
}