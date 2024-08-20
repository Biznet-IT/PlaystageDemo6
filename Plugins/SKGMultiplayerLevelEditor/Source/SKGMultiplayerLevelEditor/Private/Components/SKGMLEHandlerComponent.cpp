// Copyright 2023, Dakota Dawe, All rights reserved


#include "Components/SKGMLEHandlerComponent.h"
#include "Components/SKGMLECharacterMovement.h"
#include "Interfaces/SKGMLEInterface.h"
#include "Subsystems/SKGMLEGizmoWorldSubsystem.h"
#include "Statics/SKGMLEStatics.h"
#include "Actors/SKGMapEditorDirectionalLight.h"

#include "DrawDebugHelpers.h"
#include "Editor/SKGMLEDeveloperSettings.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

const FName SKGMLE_HISTORYCHANGED_FUNCTION = FName("OnHistoryChanged");
const FName SKGMLE_HISTORYCHANGED_FUNCTIONMINMAX = FName("OnHistoryChanged");
const FName SKGMLE_CHANGE_FUNCTION = FName("OnChange");
const FName SKGMAPEDITOR_WORLDSETTINGSCHANGED_FUNCTION = FName("OnWorldChange");

USKGMLEHandlerComponent::USKGMLEHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	SetIsReplicatedByDefault(true);

	MaxSelectDistance = 100000.0f;
	ReplicationDelay = 0.1f;
	bPerformMovementModeCheck = true;
	bCanSelect = true;
	bAppendPaste = false;
	bHasBeenInitialized = false;
}

void USKGMLEHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (const USKGMLEDeveloperSettings* Settings = GetDefault<USKGMLEDeveloperSettings>())
	{
		DefaultSnappingConfiguration = Settings->DefaultSnappingConfiguration;
		SetSnappingConfiguration(DefaultSnappingConfiguration);
		MaxSelectDistance = Settings->MaxSelectionDistance;
		TraceCollisionChannel = Settings->TraceCollisionChannel;
	}
}

void USKGMLEHandlerComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (GizmoSubystem)
	{
		GizmoSubystem->CleanupMemory();
	}
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void USKGMLEHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GizmoSubystem)
	{
		GizmoSubystem->Tick(DeltaTime);

		AccumulativeTime += 1.0f * DeltaTime;
		if (AccumulativeTime > ReplicationDelay)
		{
			AccumulativeTime = 0.0f;
			
			TArray<FSKGMLEItemTransform> ActorTransforms;
			const bool bUpdate = GizmoSubystem->GetSelectedActorTransforms(ActorTransforms);
			if (bUpdate && ActorTransforms.Num())
			{
				//UE_LOG(LogTemp, Warning, TEXT("SENDING UPDATE TO SERVER"));
				if (HasAuthority())
				{
					Server_SetActorTransforms_Implementation(ActorTransforms);
				}
				else
				{
					Server_SetActorTransforms(ActorTransforms);
				}
			}
		}

		if (SelectedActorRelativeTransforms.Num() && !GizmoSubystem->GetGizmoTransform().Equals(FTransform()))
		{
			for (FSKGMLEItemTransform& MapEditorActorTransform : SelectedActorRelativeTransforms)
			{
				if (IsValid(MapEditorActorTransform.Actor))
				{
					FVector CurrentScale = MapEditorActorTransform.Actor->GetActorScale();
					FTransform RelativeInverse = MapEditorActorTransform.ActorTransform.GetRelativeTransformReverse(GizmoSubystem->GetGizmoTransform());
					RelativeInverse.SetScale3D(CurrentScale);
					MapEditorActorTransform.Actor->SetActorTransform(RelativeInverse);
				}
			}
		}
		
		if (LocalActors.Num() && LocalActors[0])
		{
			FTransform LocalActorTransform;
			TArray<AActor*> IgnoredActors = { OwningPawn };
			IgnoredActors.Append(LocalActors);
			bool bValidSpawnedActor = false;
			if (SpawnedActors.Num())
			{
				bValidSpawnedActor = true;
				IgnoredActors.Append(SpawnedActors);
			}
			if (USKGMLEStatics::GetMouseWorldTransform(PlayerController, LocalActorTransform, 5000.0f, IgnoredActors))
			{
				LocalActors[0]->SetActorTransform(LocalActorTransform);
				if (bValidSpawnedActor)
				{
					Server_UpdatePrePlacedTransform(LocalActorTransform);
				}
			}
		}
	}
}

void USKGMLEHandlerComponent::SetBoolPropertyValue(AActor* Actor, FSKGMLEBoolProperty Property, bool Value)
{
	USKGMLEStatics::SetBoolProperty(Actor, Property, Value);
}

void USKGMLEHandlerComponent::SetFloatPropertyValue(AActor* Actor, FSKGMLEFloatProperty Property, double Value)
{
	USKGMLEStatics::SetFloatProperty(Actor, Property, Value);
}

void USKGMLEHandlerComponent::SetIntPropertyValue(AActor* Actor, FSKGMLEIntProperty Property, int32 Value)
{
	USKGMLEStatics::SetIntProperty(Actor, Property, Value);
}

void USKGMLEHandlerComponent::SetTextPropertyValue(AActor* Actor, FSKGMLETextProperty Property, FText Value)
{
	USKGMLEStatics::SetTextProperty(Actor, Property, Value);
}

void USKGMLEHandlerComponent::SetNamePropertyValue(AActor* Actor, FSKGMLENameProperty Property, FName Value)
{
	USKGMLEStatics::SetNameProperty(Actor, Property, Value);
}

void USKGMLEHandlerComponent::SetStringPropertyValue(AActor* Actor, FSKGMLEStringProperty Property, FString Value)
{
	USKGMLEStatics::SetStringProperty(Actor, Property, Value);
}

void USKGMLEHandlerComponent::Init()
{
	if (!bHasBeenInitialized)
	{
		bHasBeenInitialized = true;
		if (ReplicationDelay < 0.01f)
		{
			ReplicationDelay = 0.01f;
		}

		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASKGMapEditorDirectionalLight::StaticClass(), Actors);
		for (AActor* Actor : Actors)
		{
			if (ASKGMapEditorDirectionalLight* Light = Cast<ASKGMapEditorDirectionalLight>(Actor))
			{
				DirectionalLight = Light;
				break;
			}
		}
	
		OwningPawn = GetOwner<APawn>();
		if (OwningPawn && OwningPawn->IsLocallyControlled())
		{
			if (DirectionalLight)
			{
				DirectionalLight->BindOnWorldSettingsChanged(this, SKGMAPEDITOR_WORLDSETTINGSCHANGED_FUNCTION);
			}
		
			PlayerController = OwningPawn->GetController<APlayerController>();
		
			GizmoSubystem = GetWorld()->GetSubsystem<USKGMLEGizmoWorldSubsystem>();
			if (GizmoSubystem)
			{
				GizmoSubystem->Init(PlayerController, GetWorld(), DefaultSnappingConfiguration, false);
				GizmoSubystem->BindToOnHistoryChanged(this, SKGMLE_HISTORYCHANGED_FUNCTION);
				GizmoSubystem->BindToOnHistoryChangedMinMax(this, SKGMLE_HISTORYCHANGED_FUNCTIONMINMAX);
				GizmoSubystem->BindOnChange(this, SKGMLE_CHANGE_FUNCTION);
			}
		}
		else
		{
			SetComponentTickEnabled(false);
		}

		CurrentSnappingConfiguration = DefaultSnappingConfiguration;
		OnChangeDelegate.Broadcast(ESKGMLEActionType::SnappingConfiguration);
	}
}

void USKGMLEHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(USKGMLEHandlerComponent, SpawnedActors, COND_OwnerOnly);
}

FHitResult USKGMLEHandlerComponent::MouseTraceSingle(const float Distance, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine)
{
	if (PlayerController)
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector End = WorldLocation + WorldDirection * Distance;
			if (bDrawDebugLine)
			{
				DrawDebugLine(GetWorld(), WorldLocation, End, FColor::Red, false, 8.0f, 0, 1.0f);
			}
			
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			FHitResult HitResult;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, End, CollisionChannel, Params))
			{
				return HitResult;
			}
		}
	}
	return FHitResult();
}

bool USKGMLEHandlerComponent::PerformMovementModeCheck() const
{
	if (bPerformMovementModeCheck)
	{
		if (bInMovementMode)
		{
			return false;
		}
	}
	return true;
}

void USKGMLEHandlerComponent::OnHistoryChanged(ESKGMLEHistoryActionType ActionType) const
{
	OnHistoryChangedDelegate.Broadcast(ActionType); OnChangeDelegate.Broadcast(ESKGMLEActionType::Transform);
}

bool USKGMLEHandlerComponent::Server_UpdatePrePlacedTransform_Validate(const FTransform& Transform)
{
	return true;
}

void USKGMLEHandlerComponent::Server_UpdatePrePlacedTransform_Implementation(const FTransform& Transform)
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->SetActorTransform(Transform);
		}
	}
}

void USKGMLEHandlerComponent::SetReturnPawn(APawn* Pawn)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		ReturnPawn = Pawn;
	}
}

bool USKGMLEHandlerComponent::Server_UnpossessToReturnPawn_Validate()
{
	return true;
}

void USKGMLEHandlerComponent::Server_UnpossessToReturnPawn_Implementation()
{
	UnpossessToReturnPawn();
}

void USKGMLEHandlerComponent::UnpossessToReturnPawn()
{
	if (GizmoSubystem)
	{
		GizmoSubystem->Grab(nullptr, false);
	}
	if (HasAuthority())
	{
		if (OwningPawn && ReturnPawn)
		{
			OwningPawn->GetController()->Possess(ReturnPawn);
		}
	}
	else
	{
		Server_UnpossessToReturnPawn();
	}
}

void USKGMLEHandlerComponent::CleanupAllSpawnedActors()
{
	for (int32 i = 0; i < AllOwnedActors.Num(); ++i)
	{
		if (!IsValid(AllOwnedActors[i]) || AttemptedDeletedActors.Contains(AllOwnedActors[i]))
		{
			AllOwnedActors.RemoveAt(i, 1, false);
		}
	}
	AllOwnedActors.Shrink();
	AttemptedDeletedActors.Empty();
}

void USKGMLEHandlerComponent::OnRep_SpawnedActor()
{
	if (SpawnedActors.Num() && GizmoSubystem)
	{
		const FTransform CurrentTransform = GizmoSubystem->GetGizmoTransform();
		GizmoSubystem->Grab(nullptr, false);
		if (LocalActors.Num())
		{
			USKGMLEStatics::HideSelection(SpawnedActors, true);
		}
		else
		{
			for (AActor* Actor : SpawnedActors)
			{
				if (IsValid(Actor))
				{
					GizmoSubystem->Grab(Actor, true);
					if (bIsDrag)
					{
						if (IsValid(Actor) && Actor->Implements<USKGMLEInterface>())
						{
							ISKGMLEInterface::Execute_OnGrabbed(Actor);
						}
					}
				}
			}
			if (bIsDrag)
			{
				bIsDrag = false;
				GizmoSubystem->LeftMouseDown();
				GizmoSubystem->SetSelectionAtTransform(CurrentTransform, false);
			}
		}

		CleanupAllSpawnedActors();
		AllOwnedActors.Append(SpawnedActors);
		OnChangeDelegate.Broadcast(ESKGMLEActionType::OwnedActorsChange);
		GizmoSubystem->GetSelectedActorTransformsFromGizmo(SelectedActorRelativeTransforms);
	}
}

bool USKGMLEHandlerComponent::Server_SpawnActor_Validate(TSubclassOf<AActor> ActorClass, const FTransform& Transform, bool bPrePlace)
{
	return true;
}

void USKGMLEHandlerComponent::Server_SpawnActor_Implementation(TSubclassOf<AActor> ActorClass, const FTransform& Transform, bool bPrePlace)
{
	SpawnActor(ActorClass, Transform, bPrePlace);
}

AActor* USKGMLEHandlerComponent::SpawnActor(TSubclassOf<AActor> ActorClass, const FTransform& Transform, bool bPrePlace)
{
	if (IsValid(ActorClass))
	{
		if (GizmoSubystem)
		{
			GizmoSubystem->Grab(nullptr, false);
		}
		SpawnedActors.Empty();
		bAppendPaste = false;
		if (bPrePlace && OwningPawn && OwningPawn->IsLocallyControlled())
		{
			AActor* Actor = SpawnActorLocalPrePlace(ActorClass, Transform);
			if (IsValid(Actor) && Actor->Implements<USKGMLEInterface>())
			{
				ISKGMLEInterface::Execute_OnGrabbed(Actor);
			}
		}
		if (HasAuthority())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwningPawn;
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClass, Transform, SpawnParams);
			SpawnedActors.Add(SpawnedActor);
			if (OwningPawn && OwningPawn->IsLocallyControlled())
			{
				OnRep_SpawnedActor();
			}
			return SpawnedActor;
		}
		else
		{
			Server_SpawnActor(ActorClass, Transform, bPrePlace);
		}
	}
	return nullptr;
}

AActor* USKGMLEHandlerComponent::SpawnActorLocalPrePlace(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	if (IsValid(ActorClass))
	{
		if (GizmoSubystem)
		{
			GizmoSubystem->Grab(nullptr, false);
		}
		AActor* LocalActor = GetWorld()->SpawnActorDeferred<AActor>(ActorClass, Transform);
		LocalActor->Tags.Add(FName("PrePlace"));
		// Incase the actor is set to replicate and spawned by the server so other clients dont see it
		LocalActor->SetReplicates(false);
		UGameplayStatics::FinishSpawningActor(LocalActor, Transform);
		LocalActors.Add(LocalActor);
		return LocalActor;
	}
	return nullptr;
}

void USKGMLEHandlerComponent::TryDuplicateDrag()
{
	if (GizmoSubystem)
	{
		if (GizmoSubystem->IsHoveringGizmo())
		{
			CopySelection();
			if (CopiedActors.CopiedActors.Num())
			{
				bIsDrag = true;
				GizmoSubystem->LeftMouseDown();
				GizmoSubystem->ClearSelectedKeepGizmo();
				SelectedActorRelativeTransforms.Empty();
				Paste(true);
			}
		}
	}
}

void USKGMLEHandlerComponent::EnterMovementMode(bool bEnter)
{
	bInMovementMode = bEnter;
	if (OwningPawn)
	{
		if (USKGMLECharacterMovement* MovementComponent = Cast<USKGMLECharacterMovement>(OwningPawn->GetMovementComponent()))
		{
			MovementComponent->EnterMovementMode(bInMovementMode);
		}
	}
}

FHitResult USKGMLEHandlerComponent::MouseTrace(float Distance, bool bDrawDebugLine)
{
	return MouseTraceSingle(Distance, TraceCollisionChannel, bDrawDebugLine);
}

void USKGMLEHandlerComponent::FindAndGrab(bool bAppendSelection)
{
	if (!PerformMovementModeCheck() || !bCanSelect)
	{
		return;
	}
	if (GizmoSubystem)
	{
		if (!GizmoSubystem->IsHoveringGizmo())
		{
			const FHitResult TraceResult = MouseTrace(MaxSelectDistance, false);
			AActor* HitActor = TraceResult.GetActor();
			bAppendPaste = false;
			//GizmoSubystem->Grab(HitActor, bAppendSelection);
			if (IsValid(HitActor) && HitActor->Implements<USKGMLEInterface>())
			{
				if (ISKGMLEInterface::Execute_CanSelect(HitActor))
				{
					GizmoSubystem->Grab(HitActor, bAppendSelection);
					ISKGMLEInterface::Execute_OnGrabbed(HitActor);
				}
				else
				{
					GizmoSubystem->Grab(nullptr, bAppendSelection);
				}
			}
			else
			{
				GizmoSubystem->Grab(nullptr, bAppendSelection);
			}
		}
		GizmoSubystem->LeftMouseDown();
		GizmoSubystem->GetSelectedActorTransformsFromGizmo(SelectedActorRelativeTransforms);
	}
}

void USKGMLEHandlerComponent::Grab(AActor* Actor, bool bAppendSelection)
{
	if (!IsValid(Actor) || !PerformMovementModeCheck() || !bCanSelect)
	{
		return;
	}
	if (Actor->Implements<USKGMLEInterface>() && ISKGMLEInterface::Execute_CanSelect(Actor) && GizmoSubystem)
	{
		bAppendPaste = false;
		GizmoSubystem->Grab(Actor, bAppendSelection);
		
		GizmoSubystem->LeftMouseDown();
		GizmoSubystem->GetSelectedActorTransformsFromGizmo(SelectedActorRelativeTransforms);

		ISKGMLEInterface::Execute_OnGrabbed(Actor);
	}
}

bool USKGMLEHandlerComponent::Server_SetActorTransforms_Validate(const TArray<FSKGMLEItemTransform>& ActorTransforms)
{
	return true;
}

void USKGMLEHandlerComponent::Server_SetActorTransforms_Implementation(const TArray<FSKGMLEItemTransform>& ActorTransforms)
{
	for (const FSKGMLEItemTransform& ActorTransform : ActorTransforms)
	{
		if (IsValid(ActorTransform.Actor))
		{
			ActorTransform.Actor->SetActorTransform(ActorTransform.ActorTransform);
			if (ActorTransform.Actor->Implements<USKGMLEInterface>())
			{
				ISKGMLEInterface::Execute_OnScaleChanged(ActorTransform.Actor, ActorTransform.ActorTransform.GetScale3D());
			}
		}
	}
}

void USKGMLEHandlerComponent::Release()
{
	if (!PerformMovementModeCheck())
	{
		return;
	}
	if (GizmoSubystem)
	{
		const int32 SelectedActorCount = GizmoSubystem->GetSelectedActors().Num();
		if (SelectedActorCount > 0)
		{
			AActor* Actor = GizmoSubystem->GetSelectedActors()[SelectedActorCount - 1];
			if (IsValid(Actor) && Actor->Implements<USKGMLEInterface>())
			{
				ISKGMLEInterface::Execute_OnRelease(Actor);
			}
		}
		
		GizmoSubystem->LeftMouseUp();
		TArray<FSKGMLEItemTransform> ActorTransforms;
		GizmoSubystem->GetSelectedActorTransforms(ActorTransforms, true);
		Server_SetActorTransforms(ActorTransforms);
	}
}

void USKGMLEHandlerComponent::PlacePrePlacedActor()
{
	if (LocalActors.Num())
	{
		bool bValidSpawnedActor = false;
		if (SpawnedActors.Num() && IsValid(SpawnedActors[0]))
		{
			if (GizmoSubystem)
			{
				GizmoSubystem->Grab(SpawnedActors[0], false);
			}
			bValidSpawnedActor = true;
			USKGMLEStatics::HideSelection(SpawnedActors, false);
		}
		if (LocalActors.Num() && LocalActors[0])
		{
			if (!bValidSpawnedActor)
			{
				SpawnActor(LocalActors[0]->GetClass(), LocalActors[0]->GetActorTransform(), false);
			}
			LocalActors[0]->Destroy();
			LocalActors.Empty();
		}
	}
}

void USKGMLEHandlerComponent::SetCoordinateSystem(ESKGMLECoordinate CoordinateSystem)
{
	if (!PerformMovementModeCheck())
	{
		return;
	}
	if (GizmoSubystem)
	{
		GizmoSubystem->SetCoordinateSystem(CoordinateSystem);
		OnChangeDelegate.Broadcast(ESKGMLEActionType::CoordinateSystem);
	}
}

ESKGMLECoordinate USKGMLEHandlerComponent::GetCoordinateSystem() const
{
	if (GizmoSubystem)
	{
		return GizmoSubystem->GetCoordinateSystem();
	}
	return ESKGMLECoordinate::World;
}

void USKGMLEHandlerComponent::SetSnappingConfiguration(const FSKGMLESnappingConfiguration& SnappingConfiguration)
{
	CurrentSnappingConfiguration = SnappingConfiguration;
	if (GizmoSubystem)
	{
		GizmoSubystem->UpdateSnappingConfiguration(CurrentSnappingConfiguration);
		OnChangeDelegate.Broadcast(ESKGMLEActionType::SnappingConfiguration);
	}
}

void USKGMLEHandlerComponent::SetGizmoType(ESKGMLEGizmoType GizmoType)
{
	if (!PerformMovementModeCheck())
	{
		return;
	}
	if (GizmoSubystem)
	{
		GizmoSubystem->SetGizmoType(GizmoType);
		OnChangeDelegate.Broadcast(ESKGMLEActionType::GizmoType);
	}
}

ESKGMLEGizmoType USKGMLEHandlerComponent::GetGizmoType() const
{
	if (GizmoSubystem)
	{
		return GizmoSubystem->GetGizmoType();
	}
	return ESKGMLEGizmoType::StandardTranslateRotate;
}

void USKGMLEHandlerComponent::AddLocalActor(AActor* Actor)
{
	if (IsValid(Actor))
	{
		LocalActors.Add(Actor);
	}
}

TArray<AActor*> USKGMLEHandlerComponent::GetAllActors(bool bReCache, bool bShowNonSelectableActors)
{
	if (bReCache || !bShowNonSelectableActors)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USKGMLEInterface::StaticClass(), Actors);
		AllActors.Empty();
		AllActors.Reserve(Actors.Num());
		for (AActor* Actor : Actors)
		{
			if (IsValid(Actor) && !Actor->ActorHasTag(FName("PrePlace")))
			{
				if (!bShowNonSelectableActors)
				{
					if (ISKGMLEInterface::Execute_CanSelect(Actor))
					{
						AllActors.Add(Actor);
					}
				}
				else
				{
					AllActors.Add(Actor);
				}
			}
		}
	}
	return AllActors;
}

TArray<AActor*> USKGMLEHandlerComponent::GetAllOwnedActors(bool bClean, bool bShowNonSelectableActors)
{
	if (bClean)
	{
		CleanupAllSpawnedActors();
	}
	
	TArray<AActor*> Actors = AllOwnedActors;
	if (!bShowNonSelectableActors)
	{
		Actors.Empty();
		Actors.Reserve(AllOwnedActors.Num());
		for (AActor* Actor : AllOwnedActors)
		{
			if (IsValid(Actor) && ISKGMLEInterface::Execute_CanSelect(Actor))
			{
				Actors.Add(Actor);
			}
		}
		Actors.Shrink();
	}
	return Actors;
}

void USKGMLEHandlerComponent::EnableHighlightingOfSelection(bool bEnable)
{
	if (!PerformMovementModeCheck())
	{
		return;
	}
	if (GizmoSubystem)
	{
		GizmoSubystem->EnableHighlightingOfSelection(bEnable);
		OnChangeDelegate.Broadcast(ESKGMLEActionType::HighlightingToggle);
	}
}

TArray<AActor*> USKGMLEHandlerComponent::GetSelection()
{
	if (GizmoSubystem)
	{
		return GizmoSubystem->GetSelectedActors();
	}
	return TArray<AActor*>();
}

void USKGMLEHandlerComponent::SetSelectionAtTransform(const FTransform& Transform, bool bMoveSelectionToCenter)
{
	if (GizmoSubystem)
	{
		GizmoSubystem->SetSelectionAtTransform(Transform, bMoveSelectionToCenter);
		TArray<FSKGMLEItemTransform> ActorTransforms;
		GizmoSubystem->GetSelectedActorTransforms(ActorTransforms, true);
		Server_SetActorTransforms(ActorTransforms);
	}
}

bool USKGMLEHandlerComponent::GetSelectionTransform(FTransform& OUTTransform)
{
	if (GizmoSubystem)
	{
		OUTTransform = GizmoSubystem->GetGizmoTransform();
		return !OUTTransform.Equals(FTransform());
	}
	return false;
}

bool USKGMLEHandlerComponent::Server_ApplyMaterial_Validate(const FSKGMLEMeshComponent& MaterialData)
{
	return true;
}

void USKGMLEHandlerComponent::Server_ApplyMaterial_Implementation(const FSKGMLEMeshComponent& MaterialData)
{
	ApplyMaterial(MaterialData);
}

void USKGMLEHandlerComponent::ApplyMaterial(const FSKGMLEMeshComponent& MaterialData)
{
	if (IsValid(MaterialData.MeshComponent))
	{
		if (USKGMLEStatics::ApplyMaterial(MaterialData))
		{
			if (HasAuthority())
			{
				AActor* Actor = MaterialData.MeshComponent->GetOwner();
				if (IsValid(Actor) && Actor->Implements<USKGMLEInterface>())
				{
					const FSKGMLEItemMaterialData ItemMaterialData = USKGMLEStatics::GetItemMaterialData(Actor);
					ISKGMLEInterface::Execute_OnMaterialLoaded(Actor, ItemMaterialData);
				}
			}
			else
			{
				Server_ApplyMaterial(MaterialData);
			}
		}
	}
}

void USKGMLEHandlerComponent::Undo()
{
	if (GizmoSubystem)
	{
		GizmoSubystem->PerformAction(ESKGMLEHistoryActionType::Undo);
	}
}

void USKGMLEHandlerComponent::Redo()
{
	if (GizmoSubystem)
	{
		GizmoSubystem->PerformAction(ESKGMLEHistoryActionType::Redo);
	}
}

bool USKGMLEHandlerComponent::Server_Delete_Validate(const TArray<AActor*>& Actors, bool bDeleteOnlyOwned)
{
	return true;
}

void USKGMLEHandlerComponent::Server_Delete_Implementation(const TArray<AActor*>& Actors, bool bDeleteOnlyOwned)
{
	Delete(Actors, bDeleteOnlyOwned);
}
// REFACTOR!!!!!!!!!!!!!!!!!!!!!
void USKGMLEHandlerComponent::Delete(TArray<AActor*> Actors, bool bDeleteOnlyOwned)
{
	if (Actors.Num())
	{
		if (bDeleteOnlyOwned && OwningPawn && OwningPawn->IsLocallyControlled())
		{
			TArray<AActor*> MatchingOwnedActors;
			MatchingOwnedActors.Reserve(AllOwnedActors.Num());
			for (AActor* Actor : Actors)
			{
				if (IsValid(Actor) && AllOwnedActors.Contains(Actor))
				{
					MatchingOwnedActors.Add(Actor);
				}
			}
			MatchingOwnedActors.Shrink();
			Actors = MatchingOwnedActors;
		}
		
		for (AActor* Actor : Actors)
		{
			if (IsValid(Actor) && Actor->Implements<USKGMLEInterface>())
			{
				ISKGMLEInterface::Execute_OnDeleted(Actor);
			}
		}
		
		if (HasAuthority())
		{
			for (AActor* Actor : Actors)
			{
				if (IsValid(Actor))
				{
					Actor->Destroy();
				}
			}
		}
		else
		{
			Server_Delete(Actors, bDeleteOnlyOwned);
		}
	}
}

void USKGMLEHandlerComponent::DeleteSelection(bool bDeleteOnlyOwned)
{
	if (GizmoSubystem)
	{
		AttemptedDeletedActors = GizmoSubystem->GetSelectedActors();
		Delete(AttemptedDeletedActors, bDeleteOnlyOwned);
		
		SelectedActorRelativeTransforms.Empty();
		GizmoSubystem->Grab(nullptr, false);
		CleanupAllSpawnedActors();
		OnChangeDelegate.Broadcast(ESKGMLEActionType::Delete);
	}
}

bool USKGMLEHandlerComponent::Server_SpawnActorPaste_Validate(FSKGMLECopiedActors CopiedActorsToSpawn, const FTransform& Transform, bool bCopyProperties, const FString& Prefix)
{
	return true;
}

void USKGMLEHandlerComponent::Server_SpawnActorPaste_Implementation(FSKGMLECopiedActors CopiedActorsToSpawn, const FTransform& Transform, bool bCopyProperties, const FString& Prefix)
{
	SpawnActorPaste(CopiedActorsToSpawn, Transform, bCopyProperties, Prefix);
}

AActor* USKGMLEHandlerComponent::SpawnActorPaste(FSKGMLECopiedActors CopiedActorsToSpawn, const FTransform& Transform, bool bCopyProperties, FString Prefix)
{
	if (CopiedActorsToSpawn.CopiedActors.Num())
	{
		bAppendPaste = true;
		if (HasAuthority())
		{
			SpawnedActors.Empty();
			SpawnedActors.Reserve(CopiedActorsToSpawn.CopiedActors.Num());
			for (FSKGMLECopiedActor CopiedActor : CopiedActorsToSpawn.CopiedActors)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = OwningPawn;
				if (AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(CopiedActor.ActorClass, CopiedActor.RelativeTransform.GetRelativeTransformReverse(Transform), SpawnParams))
				{
					if (CopiedActor.Actor)
					{
						FSKGMLEAllProperties Properties = USKGMLEStatics::GetAllProperties(CopiedActor.Actor, Prefix);
						USKGMLEStatics::CopyAllProperties(SpawnedActor, Properties.FloatProperties, Properties.IntProperties, Properties.BoolProperties, Properties.TextProperties, Properties.NameProperties, Properties.StringProperties);
					
						for (FSKGMLEFloatProperty Property : Properties.FloatProperties)
						{
							UE_LOG(LogTemp, Warning, TEXT("Name: %s   Value: %f"), *Property.PropertyName.ToString(), Property.CurrentValue);
						}
						SpawnedActor->SetActorScale3D(CopiedActor.WorldScale);
						UE_LOG(LogTemp, Warning, TEXT("Scale: %s"), *CopiedActor.WorldScale.ToString());
					}
					SpawnedActors.Add(SpawnedActor);
				}
			}
			if (OwningPawn && OwningPawn->IsLocallyControlled())
			{
				OnRep_SpawnedActor();
			}
			
			return SpawnedActors[SpawnedActors.Num() - 1];
		}
		else
		{
			Server_SpawnActorPaste(CopiedActorsToSpawn, Transform, bCopyProperties, Prefix);
		}
	}
	return nullptr;
}

void USKGMLEHandlerComponent::CopySelection()
{
	CopiedActors.CopiedActors.Empty();
	if (GizmoSubystem && GizmoSubystem->GetSelectedActors().Num())
	{
		TArray<AActor*> SelectedActors = GizmoSubystem->GetSelectedActors();
		if (SelectedActors.Num() && IsValid(SelectedActors[0]))
		{
			CopiedActors.CopiedTransform = SelectedActors[0]->GetActorTransform();
			CopiedActors.CopiedActors.Reserve(SelectedActors.Num());
			for (AActor* Actor : SelectedActors)
			{
				if (IsValid(Actor))
				{
					FSKGMLECopiedActor CopiedActor;
					CopiedActor.ActorClass = Actor->GetClass();
					CopiedActor.RelativeTransform = CopiedActors.CopiedTransform.GetRelativeTransform(Actor->GetActorTransform());
					CopiedActor.Actor = Actor;
					CopiedActor.WorldScale = Actor->GetActorScale3D();
					CopiedActors.CopiedActors.Add(CopiedActor);
				}
			}
		}
	}
}

void USKGMLEHandlerComponent::Paste(bool bPasteAtCopiedLocation, bool bCopyProperties, FString Prefix)
{
	if (CopiedActors.CopiedActors.Num())
	{
		FTransform Transform = CopiedActors.CopiedTransform;
		if (!bPasteAtCopiedLocation)
		{
			Transform.SetLocation(MouseTraceSingle(10000.0f, ECC_Visibility).Location);
			if (Transform.GetLocation().Equals(FVector::ZeroVector))
			{
				return;
			}
		}
		SpawnActorPaste(CopiedActors, Transform, bCopyProperties, Prefix);
	}
}

bool USKGMLEHandlerComponent::Server_SetWorldBrightness_Validate(float Brightness)
{
	if (DirectionalLight)
	{
		return DirectionalLight->AllowClientAuthorativeChanges() ? true : false;
	}
	return false;
}

void USKGMLEHandlerComponent::Server_SetWorldBrightness_Implementation(float Brightness)
{
	SetWorldBrightness(Brightness);
}

bool USKGMLEHandlerComponent::Server_SetWorldTimeOfDay_Validate(float TimeOfDay)
{
	if (DirectionalLight)
	{
		return DirectionalLight->AllowClientAuthorativeChanges() ? true : false;
	}
	return false;
}

void USKGMLEHandlerComponent::Server_SetWorldTimeOfDay_Implementation(float TimeOfDay)
{
	SetWorldTimeOfDay(TimeOfDay);
}

void USKGMLEHandlerComponent::SetWorldTimeOfDay(float TimeOfDay)
{
	if (DirectionalLight && (DirectionalLight->AllowClientAuthorativeChanges() || HasAuthority()))
	{
		DirectionalLight->SetWorldTimeOfDay(TimeOfDay);
		if (!HasAuthority())
		{
			Server_SetWorldTimeOfDay(TimeOfDay);
		}
	}
}

void USKGMLEHandlerComponent::SetWorldBrightness(float Brightness)
{
	if (DirectionalLight && (DirectionalLight->AllowClientAuthorativeChanges() || HasAuthority()))
	{
		DirectionalLight->SetWorldBrightness(Brightness);
		if (!HasAuthority())
		{
			Server_SetWorldBrightness(Brightness);
		}
	}
}

FSKGDirectionalLightSettings USKGMLEHandlerComponent::GetDirectionalLightSettings()
{
	if (DirectionalLight)
	{
		return DirectionalLight->GetDirectionalLightSettings();
	}
	return FSKGDirectionalLightSettings();
}

void USKGMLEHandlerComponent::ForceGarbageCollection()
{
	if (IsValid(GEngine))
	{
		UE_LOG(LogTemp, Warning, TEXT("FORCING GARBAGE COLLECTION"));
		GEngine->ForceGarbageCollection(true);
	}
}
