// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TranferWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class PROJECTZ_API UTranferWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMapName(FString Name);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FName GetMap() const { return Map; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MapName;

	UPROPERTY()
	FName Map;
};
