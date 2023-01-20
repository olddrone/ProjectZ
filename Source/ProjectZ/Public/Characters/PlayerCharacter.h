// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/PickupInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AChip;
class AMoney;
class UAnimMontage;
class UPlayerOverlay;

UCLASS()
class PROJECTZ_API APlayerCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddChips(AChip* Chip) override;
	virtual void AddMoney(AMoney* Money) override;


	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(EActionState Actions) { ActionState = Actions; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	
protected:
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EKeyPressed();
	
	void EquipWeapon(AWeapon* Weapon);

	virtual void Attack() override;
	virtual void AttackEnd() override;
	
	virtual void ComboAble() override;
	virtual void NextCombo()override;
	virtual void ComboDisable() override;

	virtual bool CanAttack() override;
	
	void Dodge();
	bool HasEnoughStamina();
	bool IsOccupied();
	virtual void DodgeEnd() override;

	virtual void Die() override;

	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();

	void PlayEquipMontage(const FName& SectionName);
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	void SprintStart();
	void SprintEnd();

	virtual int32 PlayAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

private:
	void SetCameraComponent();
	void Move(float Value, EAxis::Type axis);
	void InitializePlayerOverlay();
	void SetHUDHealth();
	
	FORCEINLINE bool IsUnoccupied() const 
	{ 
		return GetActionState() == EActionState::EAS_Unocuupied ||
		ActionState == EActionState::EAS_Sprint; 
	}
	
	bool Sprintable();

	void SetWalkSpeed(float WalkSpeed);

	void Sprint();


private:
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AItem* OverlappingItem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unocuupied;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UPlayerOverlay* PlayerOverlay;

protected:
	void StartTrail(EActionState Action);
	UFUNCTION()
	void TrailTimerReset(EActionState Action);

	void MakeTrail();

private:
	FTimerHandle TrailTimerHandle;
	float AutomaticTrailRate = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Walk = 450.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Run = 720.f;

	//UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	bool bSprint;

	FTimerHandle Timer;
	float Rate = 0.025f;
	void StartTimer();
	void TEST();
	bool Check = true;

	int32 ComboAttackNum = 1;

	bool bSaveAttack = false;
	bool bComboAtteck = false;
};
