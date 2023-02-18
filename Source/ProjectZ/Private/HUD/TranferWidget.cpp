// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/TranferWidget.h"
#include "Components/TextBlock.h"

void UTranferWidget::SetMapName(FString Name)
{
	if (MapName)
	{
		const FText ToText = FText::FromString(Name);
		MapName->SetText(ToText);
	}
}

void UTranferWidget::YesButtonPressed()
{
}

void UTranferWidget::NoButtonPressed()
{
}
