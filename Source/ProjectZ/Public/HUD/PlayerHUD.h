// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class APlayerCharacter;
class UPlayerOverlay;
class UUserWidget;
class UTranferWidget;
/**
 * 
 */
UCLASS()
class PROJECTZ_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void SetOverlay(float Percent);

	void SetMapName(FString Name);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveWidget();

	void SetPlayerInputMode(bool bInputMode);

	FORCEINLINE void SetController(APlayerController* Cont) { Controller = Cont; }
	FORCEINLINE UPlayerOverlay* GetPlayerOverlay() { return PlayerOverlay; }
	FORCEINLINE UTranferWidget* GetTransferWidget() { return TransferWidget; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TObjectPtr<APlayerController> Controller;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TObjectPtr<UPlayerOverlay> PlayerOverlay;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UTranferWidget> TransferWidgetClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTranferWidget> TransferWidget;
};

