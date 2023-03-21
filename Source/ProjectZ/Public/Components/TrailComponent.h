// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/CharacterTypes.h"
#include "TrailComponent.generated.h"

class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UTrailComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTrailComponent();

	void StartTrail(EActionState Action);

	UFUNCTION()
	void TrailTimerReset(EActionState Action);

	void MakeTrail();
	void SprintTrail();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;

	bool bTrail;
};
