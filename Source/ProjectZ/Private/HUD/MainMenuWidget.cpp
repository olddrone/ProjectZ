// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/MainMenuWidget.h"
#include "Save/FirstSaveGame.h"
#include "Kismet/GameplayStatics.h"

FName UMainMenuWidget::GetMapname()
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(
		LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	return LoadGameInstance->MapName;
}
