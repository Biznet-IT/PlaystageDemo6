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

void AReplayCharacter::OnRep_CharacterMesh()
{
	GetMesh()->SetSkeletalMesh(CharacterMesh);
}

void AReplayCharacter::OnRep_CharacterAnimBP()
{
	GetMesh()->SetAnimInstanceClass(CharacterAnimBP);
}

void AReplayCharacter::ServerSetCharacterMeshAndAnimBP_Implementation(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP)
{
	CharacterMesh = NewMesh;
	CharacterAnimBP = NewAnimBP;

	MulticastSetCharacterMeshAndAnimBP(NewMesh, NewAnimBP);
}

bool AReplayCharacter::ServerSetCharacterMeshAndAnimBP_Validate(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP)
{
	return true;
}

void AReplayCharacter::MulticastSetCharacterMeshAndAnimBP_Implementation(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP)
{
	GetMesh()->SetSkeletalMesh(NewMesh);
	GetMesh()->SetAnimInstanceClass(NewAnimBP);
}

void AReplayCharacter::SetCharacterMeshAndAnimBP(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP)
{
	if (HasAuthority())
	{
		ServerSetCharacterMeshAndAnimBP(NewMesh, NewAnimBP);
	}
	else
	{
		ServerSetCharacterMeshAndAnimBP(NewMesh, NewAnimBP);
	}
}

void AReplayCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Add properties to replicate
	DOREPLIFETIME(AReplayCharacter, CharacterMesh);
	DOREPLIFETIME(AReplayCharacter, CharacterAnimBP);
}
/*
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
*/