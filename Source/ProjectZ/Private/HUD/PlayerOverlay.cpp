// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetMainWeapon(UTexture2D* Image)
{
	if (MainWeapon)
	{
		MainWeapon->SetBrushFromTexture(Image);
	}
}

void UPlayerOverlay::SetMoney(int32 Gold)
{
	if (MoneyText)
	{
		const FString MoneyString = FString::Printf(TEXT("%d"), Gold);
		const FText ToText = FText::FromString(MoneyString);
		MoneyText->SetText(ToText);
	}
}

void UPlayerOverlay::SetChip(int32 Chip)
{
	if (ChipText)
	{
		const FString ChipString = FString::Printf(TEXT("%d"), Chip);
		const FText ToText = FText::FromString(ChipString);
		ChipText->SetText(ToText);
	
	}
}

void UPlayerOverlay::ShowWeaponImage(ESlateVisibility bIsShow) const
{
	if (MainWeapon)
	{
		MainWeapon->SetVisibility(bIsShow);
	}
}
