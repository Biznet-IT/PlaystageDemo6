// Biznet It 2024


#include "ReplayCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimBlueprint.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AReplayCharacter::AReplayCharacter()
{

	bReplicates = true;

}


void AReplayCharacter::SetCharacterMeshAndAnimBP(FCharacterInfo InCharacterInfo)
{
	if (HasAuthority())
	{
		CharacterInfo = InCharacterInfo;
		OnRep_CharacterInfo();
	}
}

void AReplayCharacter::OnRep_CharacterInfo()
{
	if (CharacterInfo.Mesh)
	{
		GetMesh()->SetSkeletalMesh(CharacterInfo.Mesh);
	}

	if (CharacterInfo.AnimBP)
	{
		GetMesh()->SetAnimInstanceClass(CharacterInfo.AnimBP);
	}
}


void AReplayCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Add properties to replicate
	DOREPLIFETIME(AReplayCharacter, CharacterInfo);
}