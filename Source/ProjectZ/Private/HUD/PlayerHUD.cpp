// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"
#include "HUD/TranferWidget.h"

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
 
	UWorld* World = GetWorld();
	if (World)
	{
		Controller = World->GetFirstPlayerController();
		if (Controller && PlayerOverlayClass)
		{
			PlayerOverlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			PlayerOverlay->AddToViewport();
		}

		if (Controller && TransferWidgetClass)
		{
			TransferWidget = CreateWidget<UTranferWidget>(Controller, TransferWidgetClass);
			TransferWidget->AddToViewport();
			TransferWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APlayerHUD::SetOverlay(float Percent)
{
	PlayerOverlay->SetHealthBarPercent(Percent);
	PlayerOverlay->SetStaminaBarPercent(1.f);
	PlayerOverlay->SetMoney(0);
	PlayerOverlay->SetChip(0);
}

void APlayerHUD::SetOverlay(float HealthPercent, float StaminaPercent, uint32 Money, uint32 Chip)
{
	PlayerOverlay->SetHealthBarPercent(HealthPercent);
	PlayerOverlay->SetStaminaBarPercent(StaminaPercent);
	PlayerOverlay->SetMoney(Money);
	PlayerOverlay->SetChip(Chip);
}

void APlayerHUD::SetMapName(FString Name)
{
	TransferWidget->SetMapName(Name);
}

void APlayerHUD::DisplayWidget_Implementation()
{
}

void APlayerHUD::RemoveWidget_Implementation()
{
}
