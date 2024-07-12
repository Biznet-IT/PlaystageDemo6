// Biznet It 2024


#include "ReplayCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimBlueprint.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AReplayCharacter::AReplayCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AReplayCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReplayCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AReplayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AReplayCharacter::SetCharacterMeshAndAnimBP(TSubclassOf<AReplayCharacter> CharacterClass, USkeletalMesh* Mesh, UClass* AnimBP)
{
	if (CharacterClass)
	{
		AReplayCharacter* ReplayCharacter = CharacterClass->GetDefaultObject<AReplayCharacter>();

		if (ReplayCharacter)
		{
			GetMesh()->SetSkeletalMesh(ReplayCharacter->GetMesh()->SkeletalMesh);
			GetMesh()->SetAnimInstanceClass(ReplayCharacter->GetMesh()->AnimClass);

			// Assuming the mesh change needs to be replicated
			OnRep_CharacterMesh();
		}
	}

}

void AReplayCharacter::OnRep_CharacterMesh()
{
	//GetMesh()->SetSkeletalMesh(GetMesh()->SkeletalMesh);
	//GetMesh()->SetAnimInstanceClass(GetMesh()->AnimClass);
}

void AReplayCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Add properties to replicate
	//DOREPLIFETIME(AReplayCharacter, Mesh);
}