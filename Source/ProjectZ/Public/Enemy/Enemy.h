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
class AChip;
class UUserWidget;

UCLASS()
class PROJECTZ_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitDamage(int32 Damage, FVector HitLocation);

	
	FORCEINLINE	EEnemyState GetEnemyState() const { return EnemyState; }

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	void SpawnChip();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	
	void Dodge();
	bool CanDodge();
	virtual void DodgeEnd() override;

	UFUNCTION(BlueprintCallable)
	void StoreHitDamage(UUserWidget* HitDamage, FVector Location);

	UFUNCTION()
	void DestroyHitDamage(UUserWidget* HitDamage);

	void UpdateHitDamages();

private:
	void InitializeEnemy();

	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	bool IsOutsideCombatRadius();
	void ChaseTarget();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	void ClearPatrolTimer();

	void StartAttackTimer();
	void ClearAttackTimer();

	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	void SpawnDefaultWeapon();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:	
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double CombatRadius;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double AttackRadius;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double AcceptanceRadius;

	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	double PatrolRadius;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMin;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = "true"))
	float PatrolWaitMax;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float PatrollingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ChasingSpeed;

	FTimerHandle AttackTimer;

	FTimerHandle DodgeCooldown;
	float DodgeMax;
	bool bDodge;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackMin;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackMax;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AChip> ChipClass;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitDamages;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float HitDamageDestroyTime;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EEnemyGrade EnemyGrade;
};
