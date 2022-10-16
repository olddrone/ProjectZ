// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectZ/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	PlayHitMontage();

	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowerd(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowerd - GetActorLocation()).GetSafeNormal();
	const double CosTheTa = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheTa);
	Theta = FMath::RadiansToDegrees(Theta);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green,
			FString::Printf(TEXT("Theta : %f"), Theta));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),
		GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),
		GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
}

void AEnemy::PlayHitMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		Test("Hit");
		AnimInstance->Montage_Play(HitMontage);
		//AnimInstance->Montage_JumpToSection("Hit", HitMontage);

	}
}
