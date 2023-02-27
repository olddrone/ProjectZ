// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TrailComponent.h"
#include "Actors/PlayerTrail.h"
#include "Characters/PlayerCharacter.h"

UTrailComponent::UTrailComponent() : bTrail(true)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UTrailComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}


void UTrailComponent::StartTrail(EActionState Action)
{
	MakeTrail();
	
	if (PlayerCharacter->GetActionState() == Action)
	{
		static FTimerHandle TrailTimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName(TEXT("TrailTimerReset")), Action);
		GetWorld()->GetTimerManager().SetTimer(TrailTimerHandle, TimerDel, TimerRate::AutomaticTrailRate, false);
	}
}

void UTrailComponent::TrailTimerReset(EActionState Action)
{
	if (PlayerCharacter->GetActionState() == Action)
	{
		StartTrail(Action);
	}
}

void UTrailComponent::MakeTrail()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	FRotator rotator = PlayerCharacter->GetActorRotation();
	FVector  SpawnLocation = PlayerCharacter->GetActorLocation();
	SpawnLocation.Z -= 90;
	rotator.Yaw -= 90;
	auto GTrail = Cast<APlayerTrail>(GetWorld()->SpawnActor<AActor>(
		ActorToSpawn, SpawnLocation, rotator, SpawnParams));
	if (GTrail)
	{
		GTrail->Init(PlayerCharacter->GetMesh());
	}
}

void UTrailComponent::SprintTrail()
{
	if (!bTrail)
		return;

	static FTimerHandle SprintTimerHandle;
	bTrail = false;
	
	GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, [this]()
		{
			if (bTrail == false)
			{
				GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
				SprintTrail();
				bTrail = true;
				MakeTrail();
			}
		}, TimerRate::SprintRate, true);
}

