// Biznet It 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ReplayCharacter.generated.h"

//Functional
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Tags;

	FCharacterInfo()
		: Mesh(nullptr), AnimBP(nullptr), Thumbnail(nullptr), Name(TEXT(""))
	{
	}

	FCharacterInfo(USkeletalMesh* InMesh, TSubclassOf<UAnimInstance> InAnimBP, UTexture2D* InThumbnail, FString InName, TArray<FString> InTags)
		: Mesh(InMesh), AnimBP(InAnimBP), Thumbnail(InThumbnail), Name(InName), Tags(InTags)
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

	UPROPERTY(ReplicatedUsing = OnRep_CharacterMesh)
	USkeletalMesh* CharacterMesh;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterAnimBP)
	TSubclassOf<UAnimInstance> CharacterAnimBP;

	UFUNCTION()
	void OnRep_CharacterMesh();

	UFUNCTION()
	void OnRep_CharacterAnimBP();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetCharacterMeshAndAnimBP(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCharacterMeshAndAnimBP(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP);

	UFUNCTION(BlueprintCallable, Category = "Character Customization")
	void SetCharacterMeshAndAnimBP(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UFUNCTION(BlueprintCallable, Category = "Character Customization")
	//void SetCharacterMeshAndAnimBP(FCharacterInfo CharacterInfo);
/*
protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterInfo)
	FCharacterInfo CharacterInfo;

	UFUNCTION()
	void OnRep_CharacterInfo();
	*/
};
