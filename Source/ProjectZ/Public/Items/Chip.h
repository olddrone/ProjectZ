// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Chip.generated.h"


/**
 * 
 */
UCLASS()
class PROJECTZ_API AChip : public AItem
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE int32 GetValue() const { return Value; }
	FORCEINLINE void SetValue(int32 NumberOfChips) { Value = NumberOfChips; }

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Properties")
	int32 Value;

	double DesiredZ;

	UPROPERTY(EditAnywhere)
	float DriftRate;
};
