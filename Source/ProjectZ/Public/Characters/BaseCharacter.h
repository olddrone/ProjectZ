// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class PROJECTZ_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	
	virtual void Attack();
	virtual void Die();
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack() { return false; }
	bool IsAlive();
	void DisableMeshCollision();

	UFUNCTION(BlueprintCallable)
	void AbleMeshCollision();

	void PlayHitMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void StopAttackMontage();

	virtual void PlayDodgeMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd() { }
	
	UFUNCTION(BlueprintCallable)
	virtual void ComboAble() { }

	UFUNCTION(BlueprintCallable)
	virtual void NextCombo() { }

	UFUNCTION(BlueprintCallable)
	virtual void ComboDisable() { }

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd() { }

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	void DoRagdoll();
	void DoRagdollImpulse();
	void SetToHitVector(const FVector& ImpactPoint);

	void PlayMontageSection(const FName& SectionName);
private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AActor* CombatTarget;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;


private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	double WarpTargetDistance;

	FVector ToHit = FVector(0, 0, 0);

};
