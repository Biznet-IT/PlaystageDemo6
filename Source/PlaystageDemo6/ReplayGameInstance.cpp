// Biznet It 2024


#include "ReplayGameInstance.h"
//#include "Misc/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UReplayGameInstance::PopulateReplayList()
{
    // Implement your logic to populate the replay list
}

void UReplayGameInstance::GetReplayInfo(TArray<FS_ReplayInfo>& OutReplayInfos)
{
    OutReplayInfos.Empty();

    IFileManager& FileManager = IFileManager::Get();
    FString ReplayDirectory = FPaths::ProjectSavedDir() / TEXT("Demos");
    TArray<FString> ReplayFiles;
    FileManager.FindFiles(ReplayFiles, *ReplayDirectory, TEXT("*.replay"));

    for (const FString& ReplayFile : ReplayFiles)
    {
        FS_ReplayInfo ReplayInfo;
        ReplayInfo.Name = FPaths::GetBaseFilename(ReplayFile);
        ReplayInfo.FriendlyName = ReplayInfo.Name;  // You can set a different friendly name if you have it
        ReplayInfo.Timestamp = FDateTime::Now();  // Set the actual timestamp if you have it
        ReplayInfo.LengthInMS = 0.0f;  // Set the actual length if you have it
        OutReplayInfos.Add(ReplayInfo);
    }
}