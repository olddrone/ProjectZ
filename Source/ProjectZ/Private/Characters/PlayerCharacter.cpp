// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"
#include "Items/Chip.h"
#include "Items/Money.h"
#include "Actors/PlayerTrail.h"

#include "Kismet/KismetMathLibrary.h"
#include "ProjectZ/DebugMacros.h"

APlayerCharacter::APlayerCharacter() : bSprint(false)
{
	PrimaryActorTick.bCanEverTick = true;

	SetCameraComponent();

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	SetWalkSpeed(Walk);


	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));
	InitializePlayerOverlay();

}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && PlayerOverlay)
	{
		if (bSprint)
		{
			if (Sprintable())
			{
				Attributes->UseTickStamina(DeltaTime);
				Sprint();
				if (Check)
					StartTimer();
				
			}
			else
				SprintEnd();
			
		}	
		else
			Attributes->RegenStamina(DeltaTime);
		
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

}

void APlayerCharacter::InitializePlayerOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.f);
				PlayerOverlay->SetMoney(0);
				PlayerOverlay->SetChip(0);
			}
		}
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	Move(Value, EAxis::X);
}

void APlayerCharacter::MoveRight(float Value)
{
	Move(Value, EAxis::Y);
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{

		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_OneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void APlayerCharacter::Attack()
{
	if (GetActionState() == EActionState::EAS_Sprint)
	{
		SprintEnd();
	}

	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		SetActionState(EActionState::EAS_Attacking);
		StartTrail(EActionState::EAS_Attacking);
	}

}

void APlayerCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unocuupied;
}

bool APlayerCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unocuupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void APlayerCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina())
		return;

	SetActorRotation(GetLastMovementInputVector().Rotation().Quaternion());

	PlayDodgeMontage();
	DisableMeshCollision();
	ActionState = EActionState::EAS_Dodge;
	StartTrail(EActionState::EAS_Dodge);

	if (Attributes && PlayerOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool APlayerCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool APlayerCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unocuupied &&
		ActionState != EActionState::EAS_Sprint;
}

void APlayerCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	if (bSprint)
		Sprint();
	else
		SetActionState(EActionState::EAS_Unocuupied);

}

void APlayerCharacter::Die()
{
	DisableMeshCollision();
	Super::Die();
	SetActionState(EActionState::EAS_Dead);

}

bool APlayerCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unocuupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool APlayerCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unocuupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void APlayerCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_OneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);

	}
}

void APlayerCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void APlayerCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacter::FinishEquipping()
{
	SetActionState(EActionState::EAS_Unocuupied);
}

void APlayerCharacter::HitReactEnd()
{
	SetActionState(EActionState::EAS_Unocuupied);
}

void APlayerCharacter::SprintStart()
{
	bSprint = true;
	
}

void APlayerCharacter::SprintEnd()
{
	bSprint = false;
	SetActionState(EActionState::EAS_Unocuupied);
	SetWalkSpeed(Walk);
}

void APlayerCharacter::SetCameraComponent()
{
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->TargetArmLength = 300.f;
	CameraArm->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}

void APlayerCharacter::Move(float Value, EAxis::Type axis)
{
	if (IsOccupied())
		return;
	if (Controller && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(axis));
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APlayerCharacter::EKeyPressed);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::Dodge);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::SprintEnd);
}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}

}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	if (IsAlive())
	{
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	else
		Die();
	SetHUDHealth();
	return DamageAmount;
}

void APlayerCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void APlayerCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void APlayerCharacter::AddChips(AChip* Chip)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddChips(Chip->GetValue());
		PlayerOverlay->SetChip(Attributes->GetChips());
	}
}

void APlayerCharacter::AddMoney(AMoney* Money)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddMoney(Money->GetValue());
		PlayerOverlay->SetMoney(Attributes->GetMoney());
	}
}

void APlayerCharacter::SetHUDHealth()
{
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

bool APlayerCharacter::Sprintable()
{
	return HasEnoughStamina() &&
		UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity) > 0.f;
}

void APlayerCharacter::SetWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::Sprint()
{
	SetActionState(EActionState::EAS_Sprint);
	//StartTrail(EActionState::EAS_Sprint);
	SetWalkSpeed(Run);
}

void APlayerCharacter::StartTrail(EActionState Action)
{
	MakeTrail();

	if (ActionState == Action)
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName(TEXT("TrailTimerReset")), Action);
		GetWorldTimerManager().SetTimer(TrailTimerHandle, TimerDel, AutomaticTrailRate, false);
	}
}

void APlayerCharacter::MakeTrail()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	FRotator rotator = GetActorRotation();
	FVector  SpawnLocation = GetActorLocation();
	SpawnLocation.Z -= 90;
	rotator.Yaw -= 90;
	auto GTrail = Cast<APlayerTrail>(GetWorld()->SpawnActor<AActor>(
		ActorToSpawn, SpawnLocation, rotator, SpawnParams));
	if (GTrail)
	{
		GTrail->Init(GetMesh());
	}
}

void APlayerCharacter::StartTimer()
{
	Check = false;
	GetWorldTimerManager().SetTimer(Timer, this, &APlayerCharacter::TEST, Rate, true);
}

void APlayerCharacter::TEST()
{
	if (Check == false)
	{
		GetWorldTimerManager().ClearTimer(Timer);
		StartTimer();
		Check = true;
		MakeTrail();
	}
}

void APlayerCharacter::TrailTimerReset(EActionState Action)
{
	if (ActionState == Action)
	{
		StartTrail(Action);
	}
}