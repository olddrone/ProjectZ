// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerOverlay;

/**
 * 
 */
UCLASS()
class PROJECTZ_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() { return PlayerOverlay; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	UPlayerOverlay* PlayerOverlay;
};
