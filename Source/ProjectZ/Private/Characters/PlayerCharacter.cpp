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
#include "HUD/TranferWidget.h"
#include "Items/Chip.h"
#include "Items/Money.h"
#include "Components/TrailComponent.h"
#include "ProjectZ/ProjectZ.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Actors/Teleporter.h"

#include "ProjectZ/DebugMacros.h"
#include "Components/TargetComponent.h"

APlayerCharacter::APlayerCharacter() : 
	CharacterState(ECharacterState::ECS_Unequipped),
	ActionState(EActionState::EAS_Unocuupied), 
	ComboAttackNum(1), bSaveAttack(false), 
	bComboAttack(false)
{
	PrimaryActorTick.bCanEverTick = true;

	SetCameraComponent();
	SetMeshCollision();

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	
	Attributes->SetWalkSpeed(WalkSpeed::Walk);
	
	Trail = CreateDefaultSubobject<UTrailComponent>(TEXT("TrailComponent"));

}

void APlayerCharacter::StopMovement()
{
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
}

void APlayerCharacter::DropWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		Weapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		Weapon->SetItemState(EItemState::EIS_Hovering);
		Weapon->UnEquip();
	}
}

bool APlayerCharacter::GetLockOn() const
{
	return TargetComponent->IsLocked();
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
	GetSurfaceType();


	if (Attributes && PlayerOverlay)
	{
		if (Attributes->GetSprint())
		{
			if (Sprintable())
			{
				GetAttributes()->UseTickStamina(DeltaTime);
				Sprint();
				Trail->SprintTrail();

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
	PlayerController = Cast<APlayerController>(GetController());
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
			if (EquippedWeapon == nullptr)
			{
				PlayerOverlay->ShowWeaponImage(ESlateVisibility::Hidden);
			}
			DisplayOverlay();
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
	TargetComponent->TargetActorWithAxisInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
	TargetComponent->TargetActorWithAxisInput(Value);
}

void APlayerCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			DropWeapon(EquippedWeapon);
			
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (GetCharacterMovement()->IsFalling())
			return;

		EquipWeapon();
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_OneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
	InitWeaponHud(EquippedWeapon->GetItemIcon());
	ShowWeaponHud(ESlateVisibility::Visible);
}

// 코드 중복, 수정 필요
void APlayerCharacter::Attack()
{
	const float MinCost = Attributes->GetMinCost();
	const bool IsSprinting = GetActionState() == EActionState::EAS_Sprint;

	if (!HasEnoughStamina(MinCost))
		return;

	if (IsSprinting)
		SprintEnd();
	
	Super::Attack();
	
	if (bSaveAttack)
	{
		bComboAttack = true;
	}
	
	if (CanAttack())
	{
		PlayAttackMontage();
		SetActionState(EActionState::EAS_Attacking);
		Trail->StartTrail(EActionState::EAS_Attacking);
		UseAttackStamina();
	}
	
}

void APlayerCharacter::UseAttackStamina()
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->UseStamina(Attributes->GetAttackCost());
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void APlayerCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unocuupied;
	ComboAttackNum = 1;

}

void APlayerCharacter::ComboAble()
{
	bSaveAttack = true;
}

void APlayerCharacter::NextCombo()
{
	if (bComboAttack)
	{
		PlayAttackMontage();
		SetActionState(EActionState::EAS_Attacking);
		Trail->StartTrail(EActionState::EAS_Attacking);
		bComboAttack = false;
		UseAttackStamina();
	}
}

void APlayerCharacter::ComboDisable()
{
	bSaveAttack = false;
}

bool APlayerCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unocuupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void APlayerCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina(Attributes->GetDodgeCost()))
		return;


	if (UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity) > 0.f)
		SetActorRotation(GetLastMovementInputVector().Rotation().Quaternion());

	if (GetLockOn())
		bUseControllerRotationYaw = false;

	PlayDodgeMontage();
	DisableMeshCollision();
	ActionState = EActionState::EAS_Dodge;
	Trail->StartTrail(EActionState::EAS_Dodge);

	if (Attributes && PlayerOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool APlayerCharacter::HasEnoughStamina(float Cost)
{
	return Attributes && Attributes->GetStamina() >Cost;
}

bool APlayerCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unocuupied &&
		ActionState != EActionState::EAS_Sprint;
}

void APlayerCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	if (GetLockOn())
		bUseControllerRotationYaw = true;

	if (Attributes->GetSprint())
		Sprint();
	else
		SetActionState(EActionState::EAS_Unocuupied);

}

void APlayerCharacter::Die()
{
	Super::Die();
	SetActionState(EActionState::EAS_Dead);
	DisableMeshCollision();
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
	ShowWeaponHud(ESlateVisibility::Hidden);
}

void APlayerCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_OneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
	ShowWeaponHud(ESlateVisibility::Visible);
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
	if (Attributes)
		Attributes->SetSprint(true);
}

void APlayerCharacter::SprintEnd()
{
	if (Attributes)
		Attributes->SetSprint(false);

	SetActionState(EActionState::EAS_Unocuupied);
	Attributes->SetWalkSpeed(WalkSpeed::Walk);

}

int32 APlayerCharacter::PlayAttackMontage()
{
	FString PlaySection = "Attack" + FString::FromInt(ComboAttackNum);
	
	(ComboAttackNum < 3)
		? ++ComboAttackNum
		: ComboAttackNum = 1;

	PlayMontageSection(FName(*PlaySection));

	return ComboAttackNum;
}

EPhysicalSurface APlayerCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const USkeletalMeshSocket* RootSocket = GetMesh()->GetSocketByName(FName("StepSocket"));
	const FTransform SocketTransform = RootSocket->GetSocketTransform(GetMesh());
	const FVector Start = SocketTransform.GetLocation();
	const FVector End = Start + FVector(0.f, 0.f, -400.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, 
		ECollisionChannel::ECC_Visibility, QueryParams);
	
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

}

void APlayerCharacter::SetCameraComponent()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 250.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

void APlayerCharacter::SetMeshCollision()
{
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,
		ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,
		ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void APlayerCharacter::Move(float Value, EAxis::Type axis)
{
	if (Attributes->GetMove()== false)
		return;
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

	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &APlayerCharacter::Inventory);
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &APlayerCharacter::LockOn);

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
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
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

bool APlayerCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		const FVector Start = FVector(CrosshairWorldPosition);
		const FVector End = FVector(Start + CrosshairWorldDirection* 50'000.f);
		
		
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, 
			ECollisionChannel::ECC_Visibility, QueryParams);
		
		if (OutHitResult.bBlockingHit)
		{
			return true;
		}
			
	}

	return false;
}

void APlayerCharacter::InitMapName(FString Name)
{
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			TransferWidget = PlayerHUD->GetTransferWidget();
			if (PlayerOverlay)
			{
				TransferWidget->SetMapName(Name);
			}
		}
	}
}

void APlayerCharacter::DisplayOverlay_Implementation()
{
}

void APlayerCharacter::Teleport()
{
	SetPlayerInputMode(false);
	OverlappingTeleporter->OpenMap();

}

void APlayerCharacter::MoveToCharacter()
{
	SetActorLocation(GetActorLocation() + (-GetActorForwardVector() * 200.f));
}

void APlayerCharacter::SetOverlappingTeleport(ATeleporter* Teleporter)
{
	OverlappingTeleporter = Teleporter;

}

void APlayerCharacter::DisplayWidget_Implementation()
{
	Attributes->SetMove(false);
	SetPlayerInputMode(true);
	GetCharacterMovement()->Velocity = FVector(0.f);
	TransferWidget->SetVisibility(ESlateVisibility::Visible);
}

void APlayerCharacter::RemoveWidget_Implementation()
{
	Attributes->SetMove(true);
	SetPlayerInputMode(false);
	
	TransferWidget->SetVisibility(ESlateVisibility::Hidden);
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
	return HasEnoughStamina(Attributes->GetDodgeCost()) &&
		UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity) > 0.f;
}


void APlayerCharacter::Sprint()
{
	SetActionState(EActionState::EAS_Sprint);
	Attributes->SetWalkSpeed(WalkSpeed::Run);
}

void APlayerCharacter::EquipWeapon()
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

void APlayerCharacter::Inventory()
{
	PRINT_TEXT("Pressed I Key");
}

void APlayerCharacter::LockOn()
{
	TargetComponent->TargetActor();
}

void APlayerCharacter::InitWeaponHud(UTexture2D* Image)
{
	if (PlayerOverlay)
		PlayerOverlay->SetMainWeapon(Image);
}

void APlayerCharacter::ShowWeaponHud(ESlateVisibility bIsShow)
{
	if (PlayerOverlay)
		PlayerOverlay->ShowWeaponImage(bIsShow);
}

void APlayerCharacter::SetPlayerInputMode(bool bInputMode)
{
	FInputModeDataBase* InputMode = (bInputMode) 
		? static_cast<FInputModeDataBase*>(new FInputModeUIOnly())
		: static_cast<FInputModeDataBase*>(new FInputModeGameOnly());
	
	EActionState Action = (bInputMode)
		? EActionState::EAS_UI : EActionState::EAS_Unocuupied;

	PlayerController->SetInputMode(*InputMode);
	PlayerController->bShowMouseCursor = bInputMode;
	SetActionState(Action);
}

float APlayerCharacter::GetYawOffset()
{
	FRotator AimRotation = GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
	return UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

}