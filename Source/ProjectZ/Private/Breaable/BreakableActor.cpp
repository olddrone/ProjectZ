// Fill out your copyright notice in the Description page of Project Settings.

#include "Breaable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GetmetryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GetmetryCollection"));
	SetRootComponent(GetmetryCollection);
	GetmetryCollection->SetGenerateOverlapEvents(true);
	GetmetryCollection->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
}
