// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Chip.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"

AEnemy::AEnemy() : CombatRadius(1000.f), AttackRadius(200.f), AcceptanceRadius(50.f),
PatrolRadius(200.f), PatrolWaitMin(2.f), PatrolWaitMax(5.f), PatrollingSpeed(200.f),
ChasingSpeed(300.f), AttackMin(0.5f), AttackMax(1.f), HitDamageDestroyTime(1.f),
EnemyState(EEnemyState::EES_Patrolling), DodgeMax(3.f), bDodge(true), EnemyGrade(EEnemyGrade::EEG_Normal)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(60.f);

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) 
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState == EEnemyState::EES_Dead)
		return;

	(EnemyState == EEnemyState::EES_Patrolling) 
		? CheckPatrolTarget() : CheckCombatTarget();

	UpdateHitDamages();
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (EnemyState != EEnemyState::EES_Engaged)
			StartPatrolling();
	}

	else if (IsOutsideAttackRadius() && EnemyState != EEnemyState::EES_Chasing)
	{
		ClearAttackTimer();
		if (EnemyState != EEnemyState::EES_Engaged)
			ChaseTarget();
	}

	else if (CanAttack())
		StartAttackTimer();

	else if (CanDodge())
		Dodge();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (EnemyState != EEnemyState::EES_Dead)
		ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();

	if (IsInsideAttackRadius())
	{
		if (EnemyState != EEnemyState::EES_Dead)
			StartAttackTimer();
	}
}


void AEnemy::Die()
{
	Super::Die();
	EnemyState = EEnemyState::EES_Dead;

	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnChip();

	DoRagdoll();

}

void AEnemy::SpawnChip()
{
	UWorld* World = GetWorld();
	if (World && ChipClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		AChip* SpawnedChip = World->SpawnActor<AChip>(ChipClass, SpawnLocation, GetActorRotation());
		if (SpawnedChip)
		{
			SpawnedChip->SetValue(Attributes->GetChips());
			SpawnedChip->SetOwner(this);
		}
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	
	ShowHitDamage(DamageAmount, GetActorLocation()+FVector(0.f,0.f,100.f));

	if (IsInsideAttackRadius())
		EnemyState = EEnemyState::EES_Attacking;
	
	else if (IsOutsideAttackRadius())
		ChaseTarget();


	return DamageAmount;
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr)
		return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)
		return;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
		HealthBarWidget->SetVisibility(false);
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
		HealthBarWidget->SetVisibility(true);
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	MoveToTarget(PatrolTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
			ValidTargets.AddUnique(Target);
		
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr)
		return;

	FVector Direction = CombatTarget->GetActorLocation() - GetActorLocation();
	Direction.Z = 0;
	FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(Rotation);
	
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return  IsInsideAttackRadius() &&
		(EnemyState == EEnemyState::EES_NoState ||
		EnemyState == EEnemyState::EES_Patrolling ||
		EnemyState == EEnemyState::EES_Chasing);

}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
}


void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Dodge &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
	
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::Dodge()
{
	bDodge = false;
	DisableMeshCollision();
	EnemyState = EEnemyState::EES_Dodge;
	EnemyDodge();


	GetWorldTimerManager().SetTimer(DodgeCooldown, [this]() 
		{ bDodge = true; }, DodgeMax, false);
}

bool AEnemy::CanDodge()
{
	APlayerCharacter* Target = Cast<APlayerCharacter>(CombatTarget);

	return EnemyGrade == EEnemyGrade::EEG_Boss &&
		IsInsideAttackRadius() && 
		EnemyState != EEnemyState::EES_Dodge &&
		Target->GetActionState() == EActionState::EAS_Attacking && 
		EnemyState != EEnemyState::EES_Engaged &&
		EnemyState != EEnemyState::EES_Dead && bDodge;

}

void AEnemy::DodgeEnd()
{
	Super::DodgeEnd();

	EnemyState = EEnemyState::EES_NoState;
}

void AEnemy::StoreHitDamage(UUserWidget* HitDamage, FVector Location)
{
	HitDamages.Add(HitDamage, Location);

	FTimerHandle HitDamageTimer;
	FTimerDelegate HitDamageDelegate;
	HitDamageDelegate.BindUFunction(this, FName("DestroyHitDamage"), HitDamage);
	GetWorld()->GetTimerManager().SetTimer(
		HitDamageTimer, HitDamageDelegate, 
		HitDamageDestroyTime, false);
}

void AEnemy::DestroyHitDamage(UUserWidget* HitDamage)
{
	HitDamages.Remove(HitDamage);
	HitDamage->RemoveFromParent();
}

void AEnemy::UpdateHitDamages()
{
	for (auto& HitPair : HitDamages)
	{
		UUserWidget* HitDamage = HitPair.Key;
		const FVector Location = HitPair.Value;
		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);

		HitDamage->SetPositionInViewport(ScreenPosition);
	}
}
