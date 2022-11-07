// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;

UCLASS()
class PROJECTZ_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	void CheckCombatTarget();
	void CheckPatrolTarget();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:
	void PatrolTimerFinished();

private:	
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EDeathPose DeathPose;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double CombatRadius;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double AttackRadius;

	UPROPERTY()
	AAIController* EnemyController;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float WaitMin;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	float WaitMax;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	EEnemyState EnemyState;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> WeaponClass;
};
