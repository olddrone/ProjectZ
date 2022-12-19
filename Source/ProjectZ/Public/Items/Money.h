// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Money.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API AMoney : public AItem
{
	GENERATED_BODY()

public:
	FORCEINLINE int32 GetValue() const { return Value; }

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	int32 Value;

};
