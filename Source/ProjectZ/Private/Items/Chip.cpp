// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Chip.h"
#include "Interfaces/PickupInterface.h"


void AChip::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddChips(this);

		SpawnPickupSystem();
		SpawnPickupSound();

		Destroy();
	}
		
}
