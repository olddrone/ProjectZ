// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	void UseTickStamina(float DeltaTime);

	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();

	void AddMoney(int32 AmountOfMoney);
	void AddChips(int32 NumberOfChips);
	
	void SetWalkSpeed(float WalkSpeed);

	void SetHealth(float health) { Health = health; }
	void SetStamina(float stamina) { Stamina = stamina; }
	void SetMoney(uint32 money) { Money = money; }
	void SetChips(uint32 chip) { Chips = chip; }

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE int32 GetMoney() const { return Money; }
	FORCEINLINE int32 GetChips() const { return Chips; }

	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetMinCost() const { return DodgeCost; }
	FORCEINLINE float GetAttackCost() const { return AttackCost; }

	FORCEINLINE bool GetSprint() const { return bSprint; }
	FORCEINLINE bool GetMove() const { return bMove; }
	FORCEINLINE void SetSprint(bool Sprint) { bSprint = Sprint; }
	FORCEINLINE void SetMove(bool Move) { bMove = Move; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Money;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Chips;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float SprintCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float AttackCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate;

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	bool bSprint;

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	bool bMove;
};
