// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/TranferWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UTranferWidget::SetMapName(FString Name)
{
	if (MapName)
	{
		Map = FName(*Name);
		FText ToText = FText::FromString(Name);
		MapName->SetText(ToText);
	}
}

void UTranferWidget::YesButtonPressed()
{
	UGameplayStatics::OpenLevel(GetWorld(), Map);
}
