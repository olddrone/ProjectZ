// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/PlayerTrail.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"


APlayerTrail::APlayerTrail() : bSpawned(false)
{
	PrimaryActorTick.bCanEverTick = true;
	
	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("POSEABLEMESH"));
	RootComponent = PoseableMesh;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PoseMesh(
		TEXT("SkeletalMesh'/Game/CPStreetBoy/Meshes/SK_CPStreetBoy.SK_CPStreetBoy'"));
	if (SK_PoseMesh.Succeeded())
	{
		PoseableMesh->SetSkeletalMesh(SK_PoseMesh.Object);
	}
	ConstructorHelpers::FObjectFinder<UMaterial> M_GhostTail(
		TEXT("Material'/Game/CPStreetBoy/Materials/M_Trail/M_TrailBody.M_TrailBody'"));
	if (M_GhostTail.Succeeded())
	{
		Material = M_GhostTail.Object;
	}
}


void APlayerTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSpawned == true)
	{
		fAlphaCount -= DeltaTime;

		for (int i = 0; i < Materials.Num(); i++)
		{
			Materials[i]->SetScalarParameterValue("Opacity", fAlphaCount / fQuarterAlpha);
		}
		if (fAlphaCount < 0)
		{
			Destroy();
		}
	}
}

void APlayerTrail::Init(USkeletalMeshComponent* Pawn)
{
	PoseableMesh->CopyPoseFromSkeletalComponent(Pawn);

	TArray<UMaterialInterface*> Mats = PoseableMesh->GetMaterials();

	for (int i = 0; i < Mats.Num(); i++)
	{
		Materials.Add(UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Material));
		PoseableMesh->SetMaterial(i, Materials[i]);
	}

	fQuarterAlpha = fAlphaCount = 0.5f;
	bSpawned = true;
}

