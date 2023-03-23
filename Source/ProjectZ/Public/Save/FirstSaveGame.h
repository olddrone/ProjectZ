// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStat
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	uint32 Money;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	uint32 Chip;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	bool HasWeapon;
};

/**
 * 
 */
UCLASS()
class PROJECTZ_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Basic")
	FName MapName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FCharacterStat Stat;
};
