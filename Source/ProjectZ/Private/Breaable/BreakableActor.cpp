// Fill out your copyright notice in the Description page of Project Settings.

#include "Breaable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Chip.h"
#include "Components/BoxComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GetmetryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GetmetryCollection"));
	SetRootComponent(GetmetryCollection);
	GetmetryCollection->SetGenerateOverlapEvents(true);
	GetmetryCollection->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetmetryCollection->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(GetRootComponent());
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken)
		return;

	bBroken = true;
	UWorld* World = GetWorld();
	if (World && ChipClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 50.f;
		const int32 Selection = FMath::RandRange(0, ChipClasses.Num() - 1);
		World->SpawnActor<AChip>(ChipClasses[Selection], Location, GetActorRotation());
	}
}
