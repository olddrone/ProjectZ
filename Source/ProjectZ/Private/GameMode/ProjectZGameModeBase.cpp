// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameMode/ProjectZGameModeBase.h"
#include "Kismet/GameplayStatics.h"


void AProjectZGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	FString LoadData = UGameplayStatics::ParseOption(Options, "LoadData");
	FString Location = UGameplayStatics::ParseOption(Options, "SetPosition");

	bLoadData = LoadData.ToBool();
	bLocation = Location.ToBool();

}

void AProjectZGameModeBase::AsyncLevelLoad(const FString& LevelDir, const FString& LevelName, const FString& Options)
{
	
	LoadPackageAsync(LevelDir + LevelName,
		FLoadPackageAsyncDelegate::CreateLambda([=](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
			{
				if (Result == EAsyncLoadingResult::Succeeded)
					AsyncLevelLoadFinished(LevelName, Options);
			}
	), 0, PKG_ContainsMap);
}


void AProjectZGameModeBase::AsyncLevelLoadFinished(const FString LevelName, const FString Options)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), true, Options);
}

