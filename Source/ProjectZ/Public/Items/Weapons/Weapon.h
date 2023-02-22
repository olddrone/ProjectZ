// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class PROJECTZ_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void UnEquip();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	
	void PlayEquipSound();

	void ExecuteGetHit(FHitResult& BoxHit);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


private:
	void BoxTrace(FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor, FName type);

public:
	TArray<AActor*> IgnoreActors;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InterActionWidget;
};
