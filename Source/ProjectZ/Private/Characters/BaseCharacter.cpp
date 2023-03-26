// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/TargetComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "NiagaraComponent.h"

ABaseCharacter::ABaseCharacter() 
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	TargetComponent = CreateDefaultSubobject<UTargetComponent>(TEXT("TargetComponent"));
	TargetComponent->MinimumDistanceToEnable = 3000.f;
	TargetComponent->TargetableCollisionChannel = ECollisionChannel::ECC_Visibility;
	TargetComponent->bShouldControlRotation = true;

	DeathEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DeathEffect"));
	DeathEffect->SetupAttachment(GetRootComponent());

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	DeathEffect->Deactivate();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	FVector GetLocation = (Hitter) ? 
		Hitter->GetActorLocation() : 
		GetActorRotation().Vector() * -1.f;
	
	if (IsAlive())
		DirectionalHitReact(GetLocation);
	else
	{
		Die();
		ABaseCharacter* Player = Cast<ABaseCharacter>(Hitter);
		Player->TargetComponent->TargetLockOff();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	FVector HitLocation = DamageCauser->GetActorLocation();

	DirectionalHitReact(HitLocation);

	PlayHitSound(HitLocation);
	SpawnHitParticles(HitLocation);

	return DamageAmount;
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::PlayHitMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		AnimInstance->Montage_Play(HitMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitMontage);
	}
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
		CombatTarget = nullptr;
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
		
	for (int i = 0; i < DynamicDissolveMaterialInstances.Num(); ++i)
	{
		DynamicDissolveMaterialInstances[i] = UMaterialInstanceDynamic::Create(DissolveMaterialInstances[i], this);
		GetMesh()->SetMaterial(i, DynamicDissolveMaterialInstances[i]);
		DynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Dissolve"), 1.f);
		DynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Glow"), 50.f);
	}

	StartDissolve();
	DeathEffect->Activate();

}

void ABaseCharacter::DeathEnd()
{
	Destroy();

	if (EquippedWeapon)
		EquippedWeapon->Destroy();

}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	SetToHitVector(ImpactPoint);
	const double CosTheTa = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheTa);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
		Theta *= -1.f;

	FName Section("FromBack");
	if (-45.f <= Theta && Theta < 45.f)
		Section = FName("FromFront");
	else if (-135.f <= Theta && Theta < -45.f)
		Section = FName("FromLeft");
	else if (45.f <= Theta && Theta < 135.f)
		Section = FName("FromRight");

	PlayHitMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
		Attributes->ReceiveDamage(DamageAmount);
}

void ABaseCharacter::PlayMontageSection(const FName& SectionName)
{
	PlayMontageSection(AttackMontage, SectionName);
}

void ABaseCharacter::EnemyDodge()
{
	PlayRandomMontageSection(DodgeMontage, DodgeMontageSections);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName)
{
	if (SectionName.Num() <= 0)
		return -1;

	const int32	MaxSectionIndex = SectionName.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionName[Selection]);
	return Selection;
}

void ABaseCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	for (int i = 0; i < DynamicDissolveMaterialInstances.Num(); ++i)
	{
		if (DynamicDissolveMaterialInstances[i])
			DynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABaseCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABaseCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABaseCharacter::DoRagdoll()
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	DoRagdollImpulse();
}

void ABaseCharacter::DoRagdollImpulse()
{
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMesh()->AddImpulseToAllBodiesBelow(GetActorLocation() + ToHit * -5000.0f, NAME_None);
}

void ABaseCharacter::SetToHitVector(const FVector& ImpactPoint)
{
	const FVector ImpactLowerd(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	ToHit = (ImpactLowerd - GetActorLocation()).GetSafeNormal();


}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	
	if (Pose < EDeathPose::EDP_MAX)
		DeathPose = Pose;

	return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance)
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::AbleMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
