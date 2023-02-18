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
		APlayerController* Controller = World->GetFirstPlayerController();
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
