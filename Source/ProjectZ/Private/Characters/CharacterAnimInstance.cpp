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
		CharacterMovmement = Character->GetCharacterMovement();
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovmement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovmement->Velocity);
		IsFalling = CharacterMovmement->IsFalling();
		CharacterState = Character->GetCharacterState();
		ActionState = Character->GetActionState();
		DeathPose = Character->GetDeathPose();
	}
}
