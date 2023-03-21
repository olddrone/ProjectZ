// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AttributeComponent.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UAttributeComponent::UAttributeComponent() : Health(100.f), MaxHealth(100.f),
Money(0), Chips(0), Stamina(100.f), MaxStamina(100.f), 
DodgeCost(14.f), StaminaRegenRate(8.f), SprintCost(10.f), AttackCost(8.f),
bSprint(false), bMove(true)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RegenStamina(DeltaTime);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::UseTickStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina - SprintCost * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::AddMoney(int32 AmountOfMoney)
{
	Money += AmountOfMoney;
}

void UAttributeComponent::AddChips(int32 NumberOfChips)
{
	Chips += NumberOfChips;
}

void UAttributeComponent::SetWalkSpeed(float WalkSpeed)
{
	if (PlayerCharacter && PlayerCharacter->GetCharacterMovement())
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

