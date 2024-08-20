// Copyright 2023, Dakota Dawe, All rights reserved


#include "Statics/SKGMLEStatics.h"
#include "Interfaces/SKGMLEInterface.h"

#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Actors/SKGMapEditorDirectionalLight.h"
#include "BaseGizmos/GizmoRenderingUtil.h"
#include "Editor/SKGMLEDeveloperSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/Base64.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"

FString USKGMLEStatics::SerializeLevel(UObject* WorldContextObject, bool bSaveAsBase, bool& Success)
{
	if (!WorldContextObject)
	{
		Success = false;
		return FString("World Actor Invalid");
	}

	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		FSKGMLESave Save;
		TArray<AActor*> DirectionalLights;
		UGameplayStatics::GetAllActorsOfClass(World, ASKGMapEditorDirectionalLight::StaticClass(), DirectionalLights);
		if (DirectionalLights.Num())
		{
			if (const ASKGMapEditorDirectionalLight* DirectionalLight = Cast<ASKGMapEditorDirectionalLight>(DirectionalLights[0]))
			{
				Save.WorldSettings.DirectionalLight = DirectionalLight->GetDirectionalLightSettings();
			}
		}
		
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(World, USKGMLEInterface::StaticClass(), Actors);
		
		Save.Items.Reserve(Actors.Num());
		for (AActor* Actor : Actors)
		{
			if (IsValid(Actor))
			{
				FSKGMLEItemSave ItemSave;
				ItemSave.ActorToSpawn = Actor->GetClass();
				ItemSave.ItemTransform = Actor->GetActorTransform();
				ItemSave.MeshComponents = GetMeshComponentData(Actor);
				ItemSave.bIsBaseItem = !ISKGMLEInterface::Execute_CanSelect(Actor) ? true : bSaveAsBase;
				ItemSave.BoolProperties = ISKGMLEInterface::Execute_GetAllBoolProperties(Actor);
				ItemSave.FloatProperties = ISKGMLEInterface::Execute_GetAllFloatProperties(Actor);
				ItemSave.IntProperties = ISKGMLEInterface::Execute_GetAllIntProperties(Actor);
				ItemSave.TextProperties = ISKGMLEInterface::Execute_GetAllTextProperties(Actor);
				ItemSave.NameProperties = ISKGMLEInterface::Execute_GetAllNameProperties(Actor);
				ItemSave.StringProperties = ISKGMLEInterface::Execute_GetAllStringProperties(Actor);
				Save.Items.Add(ItemSave);
			}
		}
		
		FString SerializedString;
		Success = FJsonObjectConverter::UStructToJsonObjectString(Save, SerializedString);
		return SerializedString;
	}
	return FString("Failed");
}

FSKGMLESave USKGMLEStatics::DeSerializeLevel(const FString& JsonString, bool& Success)
{
	FSKGMLESave MapItems;
	Success = false;
	if (!JsonString.IsEmpty())
	{
		Success = FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &MapItems, 0, 0);
	}
	return MapItems;
}

void USKGMLEStatics::LoadSavedMap(UObject* WorldContextObject, FSKGMLESave SaveData)
{
	if (!WorldContextObject || (WorldContextObject && WorldContextObject->GetWorld() && WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_Client))
	{
		return;
	}
	ClearMap(WorldContextObject);
	
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		TArray<AActor*> DirectionalLights;
		UGameplayStatics::GetAllActorsOfClass(World, ASKGMapEditorDirectionalLight::StaticClass(), DirectionalLights);
		if (DirectionalLights.Num())
		{
			if (ASKGMapEditorDirectionalLight* DirectionalLight = Cast<ASKGMapEditorDirectionalLight>(DirectionalLights[0]))
			{
				DirectionalLight->SetDirectionalLightSettings(SaveData.WorldSettings.DirectionalLight);
			}
		}
		
		for (FSKGMLEItemSave& Item : SaveData.Items)
		{
			if (IsValid(Item.ActorToSpawn))
			{
				if (AActor* Actor = World->SpawnActor<AActor>(Item.ActorToSpawn, Item.ItemTransform))
				{
					if (Actor->Implements<USKGMLEInterface>())
					{
						FSKGMLEItemMaterialData MaterialData;
						MaterialData.Actor = Actor;
						MaterialData.MeshComponents = GetMeshComponentData(Actor);

						TArray<UMeshComponent*> ActorMeshComponents = GetMeshComponentsFromActor(Actor);

						for (FSKGMLEMeshComponent& MeshComponent : MaterialData.MeshComponents)
						{
							UE_LOG(LogTemp, Warning, TEXT("MeshComponent: %s"), *MeshComponent.MeshComponentTag.ToString());
							if (MeshComponent.MeshComponentTag != NAME_None)
							{
								for (FSKGMLEMeshComponent& ItemMeshComponent : Item.MeshComponents)
								{
									bool bFoundMatch = false;
									for (UMeshComponent* ActorMeshComponent : ActorMeshComponents)
									{
										if (ActorMeshComponent && ActorMeshComponent->ComponentHasTag(MeshComponent.MeshComponentTag))
										{
											//UE_LOG(LogTemp, Warning, TEXT("FOUND MATCH: %s"), *MeshComponent.MeshComponentTag.ToString());
											for (FSKGMLEMaterial& Material : ItemMeshComponent.Materials)
											{
												UE_LOG(LogTemp, Warning, TEXT("FOUND Material: %s"), *Material.Material->GetName());
											}
											MeshComponent.Materials = ItemMeshComponent.Materials;
											bFoundMatch = true;
											break;
										}
									}
									if (bFoundMatch)
									{
										break;
									}
								}
							}
						}
						ISKGMLEInterface::Execute_SetCanSelect(Actor, !Item.bIsBaseItem);
						for (const FSKGMLEFloatProperty& Property : Item.FloatProperties)
						{
							SetFloatProperty(Actor, Property, Property.CurrentValue);
						}
						for (const FSKGMLEBoolProperty& Property : Item.BoolProperties)
						{
							SetBoolProperty(Actor, Property, Property.CurrentValue);
						}
						for (const FSKGMLEIntProperty& Property : Item.IntProperties)
						{
							SetIntProperty(Actor, Property, Property.CurrentValue);
						}
						for (const FSKGMLETextProperty& Property : Item.TextProperties)
                        {
                        	SetTextProperty(Actor, Property, Property.CurrentValue);
                        }
						for (const FSKGMLENameProperty& Property : Item.NameProperties)
						{
							SetNameProperty(Actor, Property, Property.CurrentValue);
						}
						for (const FSKGMLEStringProperty& Property : Item.StringProperties)
						{
							SetStringProperty(Actor, Property, Property.CurrentValue);
						}
						ISKGMLEInterface::Execute_OnMaterialLoaded(Actor, MaterialData);
					}
					Actor->SetActorScale3D(Item.ItemTransform.GetScale3D());
				}
			}
		}
	}
}

void USKGMLEStatics::LoadSavedMapFromJson(UObject* WorldContextObject, const FString& JsonString)
{
	if (!WorldContextObject || (WorldContextObject && WorldContextObject->GetWorld() && WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_Client) && !JsonString.IsEmpty())
	{
		return;
	}
	
	ClearMap(WorldContextObject);
	
	bool bSuccess = false;
	const FSKGMLESave MapItems = DeSerializeLevel(JsonString, bSuccess);
	LoadSavedMap(WorldContextObject, MapItems);
}

void USKGMLEStatics::ClearMap(UObject* WorldContextObject)
{
	if (!WorldContextObject || (WorldContextObject && WorldContextObject->GetWorld() && WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_Client))
	{
		return;
	}
	
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(World, USKGMLEInterface::StaticClass(), Actors);

		for (AActor* Actor : Actors)
		{
			if (Actor)
			{
				Actor->Destroy();
			}
		}
	}
}

FString USKGMLEStatics::EncodeString(const FString& StringToEncode)
{
	return FBase64::Encode(StringToEncode);
}

FString USKGMLEStatics::DecodeString(const FString& StringToEncode)
{
	FString Dest;
	FBase64::Decode(StringToEncode, Dest);
	return Dest;
}

bool USKGMLEStatics::LoadMapFromFile(UObject* WorldContextObject, const FString& MapDirectory, const FString& MapName, const FString& Extension, FString& OutString, FString& FullMapName)
{
	if (!WorldContextObject || MapName.IsEmpty()) return false;
	
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		const FString FileName = FString(LevelName + "&" + MapName);
		const FString FilePath = FString(MapDirectory + "/" + FileName + Extension);
		FString Dest;
		FullMapName = RemoveExtension(FileName);
		const bool Successful = FFileHelper::LoadFileToString(Dest, *FilePath);
		//OutString = DecodeString(Dest);
		OutString = Dest;
		return Successful;
	}
	return false;
}

bool USKGMLEStatics::SaveMapToFile(UObject* WorldContextObject, const FString& MapDirectory, const FString& MapName, const FString& StringToSave, bool bSaveAsBaseMap, FString& FullMapName)
{
	if (!WorldContextObject || MapName.IsEmpty() || StringToSave.IsEmpty()) return false;
	
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		FString FileExtension = ".skmap";
		if (const USKGMLEDeveloperSettings* Settings = GetDefault<USKGMLEDeveloperSettings>())
		{
			if (bSaveAsBaseMap)
			{
				FileExtension = Settings->BaseMapExtension;
			}
			else
			{
				FileExtension = Settings->AdditionMapExtension;
			}
		}
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		const FString FileName = FString(LevelName + "&" + MapName + FileExtension);
		const FString FilePath = FString(MapDirectory + "/" + FileName);
		FullMapName = RemoveExtension(FileName);
		return FFileHelper::SaveStringToFile(StringToSave, *FilePath);
	}
	return false;
}

bool USKGMLEStatics::DeleteFile(const FString& Directory, const FString& FileName)
{
	IFileManager& FileManager = IFileManager::Get();
	const FString FilePath = Directory + "/" + FileName;
	if (!FilePath.IsEmpty() && FileManager.FileExists(*FilePath))
	{
		return IFileManager::Get().Delete(*FilePath);
	}
	return false;
}

bool USKGMLEStatics::DoesMapExist(UObject* WorldContextObject, const FString& MapDirectory, const FString& MapName)
{
	if (!WorldContextObject || MapName.IsEmpty()) return false;
	
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		const FString FileName = FString(LevelName + "&" + MapName + ".skmap");
		const FString FilePath = FString(MapDirectory + "/" + FileName);
		return FPaths::FileExists(*FilePath);
	}
	return false;
}

FString USKGMLEStatics::GetRealMapName(const FString& MapName)
{
	int32 Index = MapName.Find("&");
	if (Index > -1)
	{
		++Index;
		Index = MapName.Len() - Index;
		return MapName.Right(Index);
	}
	return FString("Could Not Get Map Name");
}

TArray<FString> USKGMLEStatics::GetMapList(UObject* WorldContextObject, const FString& MapDirectory, const FString& Extension, bool bCutLevelname, bool bShowAllMaps)
{
	FString FilesDirectory = *(MapDirectory + "/");
	if (FPaths::DirectoryExists(FilesDirectory))
	{
		TArray<FString> FileNames = GetAllFilesOfType(MapDirectory, Extension);
		if (WorldContextObject && !bShowAllMaps)
		{
			if (UWorld* World = WorldContextObject->GetWorld())
			{
				StripInvalidMaps(UGameplayStatics::GetCurrentLevelName(World), FileNames);
			}
		}
		if (bCutLevelname)
		{
			for (FString& MapName : FileNames)
			{
				MapName = GetRealMapName(MapName);
			}
		}
		return FileNames;
	}
	return TArray<FString>();
}

TArray<FString> USKGMLEStatics::GetAllFilesOfType(const FString& Directory, const FString& Extension)
{
	FString FilesDirectory = *(Directory + "/");
	TArray<FString> Files;
	FilesDirectory += "*" + Extension;
	FFileManagerGeneric::Get().FindFiles(Files, *FilesDirectory, true, false);
	return Files;
}

FString USKGMLEStatics::RemoveExtension(const FString& String)
{
	int32 Index = String.Find(".");
	if (Index > -1)
	{
		int32 Difference = String.Len() - Index;
		Index = String.Len() - Difference;
		return String.Left(Index);
	}
	return String;
}

void USKGMLEStatics::StripInvalidMaps(const FString& WorldName, TArray<FString>& MapList)
{
	for (uint8 i = 0; i < MapList.Num(); ++i)
	{
		if (!MapList[i].Contains(WorldName))
		{
			MapList.RemoveSingle(MapList[i]);
		}
	}
	MapList.Shrink();
}

TArray<UMeshComponent*> USKGMLEStatics::GetMeshComponentsFromActor(AActor* Actor)
{
	TArray<UMeshComponent*> MeshComponents;
	if (IsValid(Actor))
	{
		Actor->GetComponents<UMeshComponent>(MeshComponents);
		for (int32 i = 0; i < MeshComponents.Num(); ++i)
		{
			if (MeshComponents[i])
			{
				bool bValid = false;
				for (const FName& Tag : MeshComponents[i]->ComponentTags)
				{
					if (Tag.ToString().Contains("SKG"))
					{
						bValid = true;
						break;
					}
				}
				if (!bValid)
				{
					MeshComponents.RemoveAt(i, 1, false);
				}
			}
		}
		
		MeshComponents.Shrink();
	}
	return MeshComponents;
}

TArray<FSKGMLEMeshComponent> USKGMLEStatics::GetMeshComponentData(AActor* Actor)
{
	TArray<FSKGMLEMeshComponent> MeshComponentDatas;
	if (IsValid(Actor))
	{
		TArray<UMeshComponent*> MeshComponents = GetMeshComponentsFromActor(Actor);
		MeshComponentDatas.Reserve(MeshComponents.Num());
		for (UMeshComponent* MeshComponent : MeshComponents)
		{
			FName MeshComponentTag = NAME_None;
			for (const FName& Tag : MeshComponent->ComponentTags)
			{
				if (Tag.ToString().Contains("SKG"))
				{
					MeshComponentTag = Tag;
					break;
				}
			}
			if (MeshComponentTag != NAME_None)
			{
				FSKGMLEMeshComponent MeshComponentData;
				MeshComponentData.MeshComponent = MeshComponent;
				MeshComponentData.MeshComponentTag = MeshComponentTag;
				MeshComponentData.Materials = GetMeshComponentMaterialData(MeshComponent);
				MeshComponentDatas.Add(MeshComponentData);
			}
		}
	}
	
	return MeshComponentDatas;
}

FSKGMLEItemMaterialData USKGMLEStatics::GetItemMaterialData(AActor* Actor)
{
	FSKGMLEItemMaterialData ItemMaterialData;
	if (IsValid(Actor))
	{
		ItemMaterialData.Actor = Actor;
		ItemMaterialData.MeshComponents = GetMeshComponentData(Actor);
	}
	return ItemMaterialData;
}

bool USKGMLEStatics::ApplyMaterial(const FSKGMLEMeshComponent& MaterialData)
{
	bool bSetAMaterial = false;
	if (IsValid(MaterialData.MeshComponent))
	{
		for (const FSKGMLEMaterial& Material : MaterialData.Materials)
		{
			if (Material.Material)
			{
				MaterialData.MeshComponent->SetMaterial(Material.MaterialIndex, Material.Material);
				bSetAMaterial = true;
			}
		}
	}
	return bSetAMaterial;
}

bool USKGMLEStatics::ApplyMaterialItem(const FSKGMLEItemMaterialData& MaterialData)
{
	if (IsValid(MaterialData.Actor))
	{
		TArray<UMeshComponent*> ActorMeshComponents = GetMeshComponentsFromActor(MaterialData.Actor);
		for (const FSKGMLEMeshComponent& MeshComponent : MaterialData.MeshComponents)
		{
			for (UMeshComponent* ActorMeshComponent : ActorMeshComponents)
			{
				if (ActorMeshComponent && ActorMeshComponent->ComponentHasTag(MeshComponent.MeshComponentTag))
				{
					for (const FSKGMLEMaterial& Material : MeshComponent.Materials)
					{
						if (IsValid(Material.Material))
						{
							ActorMeshComponent->SetMaterial(Material.MaterialIndex, Material.Material);
						}
					}
				}
			}
		}
	}
	return true;
}

TArray<FSKGMLEMaterial> USKGMLEStatics::GetMeshComponentMaterialData(UMeshComponent* MeshComponent)
{
	TArray<FSKGMLEMaterial> MaterialDatas;
	if (IsValid(MeshComponent))
	{
		const int32 MaterialCount = MeshComponent->GetNumMaterials();
		MaterialDatas.Reserve(MaterialCount);
		for (int32 i = 0; i < MaterialCount; ++i)
		{
			FSKGMLEMaterial MaterialData;
			MaterialData.MaterialIndex = i;
			MaterialData.Material = MeshComponent->GetMaterial(i);
			MaterialDatas.Add(MaterialData);
		}
	}
	
	return MaterialDatas;
}

void USKGMLEStatics::SetBoolProperty(AActor* Actor, FSKGMLEBoolProperty PropertyData, bool Value)
{
	if (IsValid(Actor))
	{
		if (FProperty* VariableProperty = Actor->GetClass()->FindPropertyByName(PropertyData.PropertyName))
		{
			if (const FBoolProperty* Property = CastField<FBoolProperty>(VariableProperty))
			{
				PropertyData.PropertyType = ESKGMLEVariableType::Float;
				Property->SetPropertyValue_InContainer(Actor, Value);
				ISKGMLEInterface::Execute_OnPropertyChanged(Actor, PropertyData);
				ISKGMLEInterface::Execute_OnBoolPropertyChanged(Actor, PropertyData);
			}
		}
	}
}

void USKGMLEStatics::SetFloatProperty(AActor* Actor, FSKGMLEFloatProperty PropertyData, double Value)
{
	if (IsValid(Actor))
	{
		if (FProperty* VariableProperty = Actor->GetClass()->FindPropertyByName(PropertyData.PropertyName))
		{
			if (const FDoubleProperty* Property = CastField<FDoubleProperty>(VariableProperty))
			{
				PropertyData.PropertyType = ESKGMLEVariableType::Float;
				Property->SetPropertyValue_InContainer(Actor, Value);
				ISKGMLEInterface::Execute_OnPropertyChanged(Actor, PropertyData);
				ISKGMLEInterface::Execute_OnFloatPropertyChanged(Actor, PropertyData);
			}
		}
	}
}

void USKGMLEStatics::SetIntProperty(AActor* Actor, FSKGMLEIntProperty PropertyData, int32 Value)
{
	if (IsValid(Actor))
	{
		if (FProperty* VariableProperty = Actor->GetClass()->FindPropertyByName(PropertyData.PropertyName))
		{
			if (const FIntProperty* Property = CastField<FIntProperty>(VariableProperty))
			{
				PropertyData.PropertyType = ESKGMLEVariableType::Float;
				Property->SetPropertyValue_InContainer(Actor, Value);
				ISKGMLEInterface::Execute_OnPropertyChanged(Actor, PropertyData);
				ISKGMLEInterface::Execute_OnIntPropertyChanged(Actor, PropertyData);
			}
		}
	}
}

void USKGMLEStatics::SetTextProperty(AActor* Actor, FSKGMLETextProperty PropertyData, FText Value)
{
	if (IsValid(Actor))
	{
		if (FProperty* VariableProperty = Actor->GetClass()->FindPropertyByName(PropertyData.PropertyName))
		{
			if (const FTextProperty* Property = CastField<FTextProperty>(VariableProperty))
			{
				PropertyData.PropertyType = ESKGMLEVariableType::Text;
				Property->SetPropertyValue_InContainer(Actor, Value);
				ISKGMLEInterface::Execute_OnPropertyChanged(Actor, PropertyData);
				ISKGMLEInterface::Execute_OnTextPropertyChanged(Actor, PropertyData);
			}
		}
	}
}

void USKGMLEStatics::SetNameProperty(AActor* Actor, FSKGMLENameProperty PropertyData, FName Value)
{
	if (IsValid(Actor))
	{
		if (FProperty* VariableProperty = Actor->GetClass()->FindPropertyByName(PropertyData.PropertyName))
		{
			if (const FNameProperty* Property = CastField<FNameProperty>(VariableProperty))
			{
				PropertyData.PropertyType = ESKGMLEVariableType::Name;
				Property->SetPropertyValue_InContainer(Actor, Value);
				ISKGMLEInterface::Execute_OnPropertyChanged(Actor, PropertyData);
				ISKGMLEInterface::Execute_OnNamePropertyChanged(Actor, PropertyData);
			}
		}
	}
}

void USKGMLEStatics::SetStringProperty(AActor* Actor, FSKGMLEStringProperty PropertyData, FString Value)
{
	if (IsValid(Actor))
	{
		if (FProperty* VariableProperty = Actor->GetClass()->FindPropertyByName(PropertyData.PropertyName))
		{
			if (const FStrProperty* Property = CastField<FStrProperty>(VariableProperty))
			{
				PropertyData.PropertyType = ESKGMLEVariableType::String;
				Property->SetPropertyValue_InContainer(Actor, *Value);
				ISKGMLEInterface::Execute_OnPropertyChanged(Actor, PropertyData);
				ISKGMLEInterface::Execute_OnStringPropertyChanged(Actor, PropertyData);
			}
		}
	}
}

TArray<FSKGMLEFloatProperty> USKGMLEStatics::GetAllFloatProperties(AActor* Actor, FString Prefix)
{
	TArray<FSKGMLEFloatProperty> Properties;
	Properties.Reserve(10);
	if (Actor)
	{
		for (TFieldIterator<FDoubleProperty> Property(Actor->GetClass()); Property; ++Property)
		{
			if (Property)
			{
				if (!Prefix.IsEmpty() && !Property->GetName().Contains(Prefix))
				{
					break;
				}
				FSKGMLEFloatProperty PropertyData;
				PropertyData.CurrentValue = Property->GetPropertyValue(Property->ContainerPtrToValuePtr<void>(Actor));
				PropertyData.PropertyName = Property->GetFName();
				Properties.Add(PropertyData);
			}
		}
	}
	Properties.Shrink();
	return Properties;
}

TArray<FSKGMLEIntProperty> USKGMLEStatics::GetAllIntProperties(AActor* Actor, FString Prefix)
{
	TArray<FSKGMLEIntProperty> Properties;
	Properties.Reserve(10);
	if (Actor)
	{
		for (TFieldIterator<FIntProperty> Property(Actor->GetClass()); Property; ++Property)
		{
			if (Property)
			{
				if (!Prefix.IsEmpty() && !Property->GetName().Contains(Prefix))
				{
					break;
				}
				FSKGMLEIntProperty PropertyData;
				PropertyData.CurrentValue = Property->GetPropertyValue(Property->ContainerPtrToValuePtr<void>(Actor));
				PropertyData.PropertyName = Property->GetFName();
				Properties.Add(PropertyData);
			}
		}
	}
	Properties.Shrink();
	return Properties;
}

TArray<FSKGMLEBoolProperty> USKGMLEStatics::GetAllBoolProperties(AActor* Actor, FString Prefix)
{
	TArray<FSKGMLEBoolProperty> Properties;
	Properties.Reserve(10);
	if (Actor)
	{
		for (TFieldIterator<FBoolProperty> Property(Actor->GetClass()); Property; ++Property)
		{
			if (Property)
			{
				if (!Prefix.IsEmpty() && !Property->GetName().Contains(Prefix))
				{
					break;
				}
				FSKGMLEBoolProperty PropertyData;
				PropertyData.CurrentValue = Property->GetPropertyValue(Property->ContainerPtrToValuePtr<void>(Actor));
				PropertyData.PropertyName = Property->GetFName();
				Properties.Add(PropertyData);
			}
		}
	}
	Properties.Shrink();
	return Properties;
}

TArray<FSKGMLETextProperty> USKGMLEStatics::GetAllTextProperties(AActor* Actor, FString Prefix)
{
	TArray<FSKGMLETextProperty> Properties;
	Properties.Reserve(10);
	if (Actor)
	{
		for (TFieldIterator<FTextProperty> Property(Actor->GetClass()); Property; ++Property)
		{
			if (Property)
			{
				if (!Prefix.IsEmpty() && !Property->GetName().Contains(Prefix))
				{
					break;
				}
				FSKGMLETextProperty PropertyData;
				PropertyData.CurrentValue = Property->GetPropertyValue(Property->ContainerPtrToValuePtr<void>(Actor));
				PropertyData.PropertyName = Property->GetFName();
				Properties.Add(PropertyData);
			}
		}
	}
	Properties.Shrink();
	return Properties;
}

TArray<FSKGMLENameProperty> USKGMLEStatics::GetAllNameProperties(AActor* Actor, FString Prefix)
{
	TArray<FSKGMLENameProperty> Properties;
	Properties.Reserve(10);
	if (Actor)
	{
		for (TFieldIterator<FNameProperty> Property(Actor->GetClass()); Property; ++Property)
		{
			if (Property)
			{
				if (!Prefix.IsEmpty() && !Property->GetName().Contains(Prefix))
				{
					break;
				}
				FSKGMLENameProperty PropertyData;
				PropertyData.CurrentValue = Property->GetPropertyValue(Property->ContainerPtrToValuePtr<void>(Actor));
				PropertyData.PropertyName = Property->GetFName();
				Properties.Add(PropertyData);
			}
		}
	}
	Properties.Shrink();
	return Properties;
}

TArray<FSKGMLEStringProperty> USKGMLEStatics::GetAllStringProperties(AActor* Actor, FString Prefix)
{
	TArray<FSKGMLEStringProperty> Properties;
	Properties.Reserve(10);
	if (Actor)
	{
		for (TFieldIterator<FStrProperty> Property(Actor->GetClass()); Property; ++Property)
		{
			if (Property)
			{
				if (!Prefix.IsEmpty() && !Property->GetName().Contains(Prefix))
				{
					break;
				}
				FSKGMLEStringProperty PropertyData;
				PropertyData.CurrentValue = Property->GetPropertyValue(Property->ContainerPtrToValuePtr<void>(Actor));
				PropertyData.PropertyName = Property->GetFName();
				Properties.Add(PropertyData);
			}
		}
	}
	Properties.Shrink();
	return Properties;
}

FSKGMLEAllProperties USKGMLEStatics::GetAllProperties(AActor* Actor, FString Prefix)
{
	FSKGMLEAllProperties AllProperties;
	if (Actor)
	{
		AllProperties.BoolProperties = GetAllBoolProperties(Actor, Prefix);
		AllProperties.FloatProperties = GetAllFloatProperties(Actor, Prefix);
		AllProperties.IntProperties = GetAllIntProperties(Actor, Prefix);
		AllProperties.TextProperties = GetAllTextProperties(Actor, Prefix);
		AllProperties.NameProperties = GetAllNameProperties(Actor, Prefix);
		AllProperties.StringProperties = GetAllStringProperties(Actor, Prefix);
	}
	return AllProperties;
}

void USKGMLEStatics::CopyFloatProperties(AActor* ActorToCopyTo, const TArray<FSKGMLEFloatProperty>& PropertiesToCopyFrom)
{
	for (const FSKGMLEFloatProperty& ActorAProperty : PropertiesToCopyFrom)
	{
		SetFloatProperty(ActorToCopyTo, ActorAProperty, ActorAProperty.CurrentValue);
	}
}

void USKGMLEStatics::CopyIntProperties(AActor* ActorToCopyTo, const TArray<FSKGMLEIntProperty>& PropertiesToCopyFrom)
{
	for (const FSKGMLEIntProperty& ActorAProperty : PropertiesToCopyFrom)
	{
		SetIntProperty(ActorToCopyTo, ActorAProperty, ActorAProperty.CurrentValue);
	}
}

void USKGMLEStatics::CopyBoolProperties(AActor* ActorToCopyTo, const TArray<FSKGMLEBoolProperty>& PropertiesToCopyFrom)
{
	for (const FSKGMLEBoolProperty& ActorAProperty : PropertiesToCopyFrom)
	{
		SetBoolProperty(ActorToCopyTo, ActorAProperty, ActorAProperty.CurrentValue);
	}
}

void USKGMLEStatics::CopyTextProperties(AActor* ActorToCopyTo, const TArray<FSKGMLETextProperty>& PropertiesToCopyFrom)
{
	for (const FSKGMLETextProperty& ActorAProperty : PropertiesToCopyFrom)
	{
		SetTextProperty(ActorToCopyTo, ActorAProperty, ActorAProperty.CurrentValue);
	}
}

void USKGMLEStatics::CopyNameProperties(AActor* ActorToCopyTo, const TArray<FSKGMLENameProperty>& PropertiesToCopyFrom)
{
	for (const FSKGMLENameProperty& ActorAProperty : PropertiesToCopyFrom)
	{
		SetNameProperty(ActorToCopyTo, ActorAProperty, ActorAProperty.CurrentValue);
	}
}

void USKGMLEStatics::CopyStringProperties(AActor* ActorToCopyTo, const TArray<FSKGMLEStringProperty>& PropertiesToCopyFrom)
{
	for (const FSKGMLEStringProperty& ActorAProperty : PropertiesToCopyFrom)
	{
		SetStringProperty(ActorToCopyTo, ActorAProperty, ActorAProperty.CurrentValue);
	}
}

void USKGMLEStatics::CopyAllProperties(AActor* ActorToCopyTo, const TArray<FSKGMLEFloatProperty>& FloatPropertiesToCopyFrom,
	const TArray<FSKGMLEIntProperty>& IntPropertiesToCopyFrom, const TArray<FSKGMLEBoolProperty>& BoolPropertiesToCopyFrom,
	const TArray<FSKGMLETextProperty>& TextPropertiesToCopyFrom, const TArray<FSKGMLENameProperty>& NamePropertiesToCopyFrom,
	const TArray<FSKGMLEStringProperty>& StringPropertiesToCopyFrom)
{
	CopyFloatProperties(ActorToCopyTo, FloatPropertiesToCopyFrom);
	CopyIntProperties(ActorToCopyTo, IntPropertiesToCopyFrom);
	CopyBoolProperties(ActorToCopyTo, BoolPropertiesToCopyFrom);
	CopyTextProperties(ActorToCopyTo, TextPropertiesToCopyFrom);
	CopyNameProperties(ActorToCopyTo, NamePropertiesToCopyFrom);
	CopyStringProperties(ActorToCopyTo, StringPropertiesToCopyFrom);
}

bool USKGMLEStatics::GetMouseWorldTransform(const APlayerController* PlayerController, FTransform& OUTTransform,
                                            float MaxTraceDistance, TArray<AActor*> IgnoredActors)
{
	if (PlayerController && MaxTraceDistance > 0.0f)
	{
		FVector Origin, Direction;
		PlayerController->DeprojectMousePositionToWorld(Origin, Direction);
		const FVector End = Origin + Direction * MaxTraceDistance;
		
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoredActors);
		if (PlayerController->GetWorld()->LineTraceSingleByChannel(HitResult, Origin, End, ECC_Visibility, QueryParams))
		{
			OUTTransform = FTransform(FRotator::ZeroRotator, HitResult.Location, FVector::OneVector);
			return true;
		}
	}
	return false;
}

void USKGMLEStatics::HideSelection(TArray<AActor*> Actors, bool bHide)
{
	for (AActor* Actor : Actors)
	{
		if (IsValid(Actor))
		{
			TArray<UMeshComponent*> MeshComponents = GetMeshComponentsFromActor(Actor);
			for (UMeshComponent* MeshComponent : MeshComponents)
			{
				if (IsValid(MeshComponent))
				{
					MeshComponent->SetHiddenInGame(bHide);
				}
			}
		}
	}
}