// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AChip;
class AMoney;
class UAnimMontage;
class UPlayerOverlay;
class UTranferWidget;
class ATeleporter;
class APlayerController;
class UTrailComponent;
class UCombatComponent;

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

	bool TraceUnderCrosshairs(FHitResult& OutHitResult);

	void InitMapName(FString Name);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayOverlay();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisplayWidget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveWidget();

	UFUNCTION(BlueprintCallable)
	void Teleport();

	UFUNCTION(BlueprintCallable)
	void MoveToCharacter();

	void SetOverlappingTeleport(ATeleporter* Teleporter);


	FORCEINLINE UTranferWidget* GetTransferWidget() const { return TransferWidget; }
	FORCEINLINE APlayerController* GetPlayerController() const { return PlayerController; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE void SetActionState(EActionState State) { ActionState = State; }

protected:
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EKeyPressed();
	
	virtual void Die() override;

	void PlayEquipMontage(const FName& SectionName);
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	virtual int32 PlayAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTrailComponent> Trail;

	//UPROPERTY(VisibleAnywhere)
	//TObjectPtr<UCombatComponent> Combat;

private:
	void SetCameraComponent();
	void SetMeshCollision();

	void Move(float Value, EAxis::Type axis);
	void InitializePlayerOverlay();
	void SetHUDHealth();
	
	void Inventory();
	

	void InitWeaponHud(UTexture2D* Image);
	void ShowWeaponHud(ESlateVisibility bIsShow);

	void SetPlayerInputMode(bool bInputMode);


private:
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent>	SpringArm;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AItem> OverlappingItem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerOverlay> PlayerOverlay;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTranferWidget> TransferWidget;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ATeleporter> OverlappingTeleporter;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<APlayerController> PlayerController;
	


	
	// Combat============================================================

public:

	void StopMovement();
	void DropWeapon(AWeapon* Weapon);

	FORCEINLINE bool IsUnoccupied() const
	{
		return ActionState == EActionState::EAS_Unocuupied || ActionState == EActionState::EAS_Sprint;
	}

	void Dodge();
	bool HasEnoughStamina(float Cost);
	bool IsOccupied();
	virtual void DodgeEnd() override;

	void EquipWeapon(AWeapon* Weapon);
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();

	virtual void Attack() override;
	void UseAttackStamina();
	virtual void AttackEnd() override;

	virtual void ComboAble() override;
	virtual void NextCombo()override;
	virtual void ComboDisable() override;

	virtual bool CanAttack() override;

	

private:
	
	void SprintStart();
	void SprintEnd();
	bool Sprintable();
	void SetWalkSpeed(float WalkSpeed);
	void Sprint();
	void EquipWeapon();
	void LockOn();

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState;

	int32					ComboAttackNum;
	bool					bSaveAttack;
	bool					bComboAtteck;
	bool					bSprint;
	bool					bMove;
};
