// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/CharacterAnimInstance.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		IsFalling = CharacterMovement->IsFalling();
		IsLock = Character->GetLockOn();
		CharacterState = Character->GetCharacterState();
		ActionState = Character->GetActionState();
		DeathPose = Character->GetDeathPose();
		
		ZVelocity = Character->GetVelocity().Z;
		YawOffset = Character->GetYawOffset();
		
	}
}
