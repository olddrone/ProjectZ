// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerTrail.generated.h"

class UPoseableMeshComponent;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class USkeletalMeshComponent;
class UMaterial;

UCLASS()
class PROJECTZ_API APlayerTrail : public AActor
{
	GENERATED_BODY()

public:
	APlayerTrail();
	virtual void Tick(float DeltaTime) override;

	void Init(USkeletalMeshComponent* Pawn);

private:
	UPoseableMeshComponent* PoseableMesh;
	UMaterial* Material;
	TArray<UMaterialInstanceDynamic*> Materials;

	float	fAlphaCount;
	float	fQuarterAlpha;
	bool	bSpawned;

};
