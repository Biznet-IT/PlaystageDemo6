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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	FCharacterInfo()
	
		: Mesh(nullptr), AnimBP(nullptr), Thumbnail(nullptr), Name(TEXT(""))
		{

		}
	FCharacterInfo(USkeletalMesh* InMesh, TSubclassOf<UAnimInstance> InAnimBP, UTexture2D* InThumbnail, FString InName)
		: Mesh(InMesh), AnimBP(InAnimBP), Thumbnail(InThumbnail), Name(InName)
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
