// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/CharacterAnimInstance.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BaseCharacter= Cast<ABaseCharacter>(TryGetPawnOwner());
	if (BaseCharacter)
	{
		CharacterMovmement = BaseCharacter->GetCharacterMovement();
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovmement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovmement->Velocity);
		IsFalling = CharacterMovmement->IsFalling();
	}
}
