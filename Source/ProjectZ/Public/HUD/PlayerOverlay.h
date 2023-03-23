// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UOverlay;
/**
 * 
 */
UCLASS()
class PROJECTZ_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetMainWeapon(UTexture2D* Image);
	void SetMoney(int32 Gold);
	void SetChip(int32 Chip);
	
	void ShowHelp(ESlateVisibility bIsShow) const;
	void ShowWeaponImage(ESlateVisibility bIsShow) const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MainWeapon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MoneyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChipText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Help;
};
