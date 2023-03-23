// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameMode/ProjectZGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"

void AProjectZGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	FString LoadData = UGameplayStatics::ParseOption(Options, "LoadData");
	FString Location = UGameplayStatics::ParseOption(Options, "SetPosition");

	bLoadData = LoadData.ToBool();
	bLocation = Location.ToBool();

}
	
