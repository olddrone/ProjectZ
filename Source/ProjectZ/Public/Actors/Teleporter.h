// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teleporter.generated.h"

class USphereComponent;
class UCapsuleComponent;
class UNiagaraComponent;
class UWidgetComponent;
class UWidgetAnimation;

UCLASS()
class PROJECTZ_API ATeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	ATeleporter();

	UFUNCTION(BlueprintCallable)
	FString GetMapName() const { return MapName; }

	void OpenMap();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// PROPERTY 지정자 중 BlueprintNativeEvent를 활용하여 코드로 수정
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FString MapName;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TeleportMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AreaMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent* WidgetArea;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* TeleportArea;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TeleportEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InfomationWidget;

};
