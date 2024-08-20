// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGToolsDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "SKGMLEDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "SKG Map Editor Settings"))
class SKGMULTIPLAYERLEVELEDITOR_API USKGMLEDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	USKGMLEDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "SaveLoad")
	FString MapsFolderDirectory = "Saved/UserSavedMaps";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "SaveLoad")
	FString BaseMapExtension = ".skmap";
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "SaveLoad")
	FString AdditionMapExtension = ".skmap";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "MapEditorComponent")
	float MaxSelectionDistance = 100000.0f;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "MapEditorComponent")
	FSKGMLESnappingConfiguration DefaultSnappingConfiguration;
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "MapEditorComponent")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel = ECC_GameTraceChannel1;
};
