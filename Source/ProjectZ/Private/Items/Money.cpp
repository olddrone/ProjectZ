// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Money.h"
#include "Interfaces/PickupInterface.h"


void AMoney::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddMoney(this);

		SpawnPickupSound();
		Destroy();
	}
}
