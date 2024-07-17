// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ReplayCharacter.generated.h"

USTRUCT(BlueprintType)
struct FCharacterInfo
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	FCharacterInfo()
	
		: Mesh(nullptr), AnimBP(nullptr)
		{

		}
	FCharacterInfo(USkeletalMesh* InMesh, TSubclassOf<UAnimInstance> InAnimBP)

		: Mesh(InMesh), AnimBP(InAnimBP)
	{

	}
};

/**
 *
 */
UCLASS()
class PLAYSTAGEDEMO6_API AReplayCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AReplayCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character Customization")
	void SetCharacterMeshAndAnimBP(FCharacterInfo CharacterInfo);

protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterInfo)
	FCharacterInfo CharacterInfo;

	UFUNCTION()
	void OnRep_CharacterInfo();

};
