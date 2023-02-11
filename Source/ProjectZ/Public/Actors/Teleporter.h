// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Teleporter.generated.h"

class USphereComponent;
class UCapsuleComponent;
class UNiagaraComponent;
class UWidgetComponent;

UCLASS()
class PROJECTZ_API ATeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	ATeleporter();

	UFUNCTION(BlueprintCallable)
	FString GetMapName() const { return MapName; }

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	// 추후 접촉 시 UI로 맵 이동 확인 
	/*UFUNCTION()
	virtual void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

	UFUNCTION()
	virtual void OnAreaOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FString MapName;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TeleporterMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USphereComponent* WidgetArea;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* TeleporterCapsule;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TeleporterEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* Widget;
};
