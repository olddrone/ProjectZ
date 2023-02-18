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
#include "Actors/PlayerTrail.h"
#include "ProjectZ/ProjectZ.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectZ/DebugMacros.h"
#include "GameFramework/PlayerController.h"
#include "Actors/Teleporter.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"

APlayerCharacter::APlayerCharacter() : bSprint(false), 
	CharacterState(ECharacterState::ECS_Unequipped),
	ActionState(EActionState::EAS_Unocuupied), 
	bTrail(true), ComboAttackNum(1), bSaveAttack(false),
	bComboAtteck (false), bMove(true)
{
	PrimaryActorTick.bCanEverTick = true;

	SetCameraComponent();
	SetMeshCollision();

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	
	SetWalkSpeed(WalkSpeed::Walk);


	DeathEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DeathEffect"));
	DeathEffect->SetupAttachment(GetRootComponent());

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));

}

void APlayerCharacter::StopMovement()
{
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));
	InitializePlayerOverlay();
	DeathEffect->Deactivate();

}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetSurfaceType();
	

	if (Attributes && PlayerOverlay)
	{
		if (bSprint)
		{
			if (Sprintable())
			{
				Attributes->UseTickStamina(DeltaTime);
				Sprint();
				if (bTrail)
					SprintTrail();
				
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
		bComboAtteck = true;
	}
	
	if (CanAttack())
	{
		PlayAttackMontage();
		SetActionState(EActionState::EAS_Attacking);
		StartTrail(EActionState::EAS_Attacking);
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
	if (bComboAtteck)
	{
		PlayAttackMontage();
		SetActionState(EActionState::EAS_Attacking);
		StartTrail(EActionState::EAS_Attacking);
		bComboAtteck = false;
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
	if (bSprint)
		Sprint();
	else
		SetActionState(EActionState::EAS_Unocuupied);

}

void APlayerCharacter::Die()
{
	Super::Die();
	SetActionState(EActionState::EAS_Dead);
	DisableMeshCollision();

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
	bSprint = true;
	
}

void APlayerCharacter::SprintEnd()
{
	bSprint = false;
	SetActionState(EActionState::EAS_Unocuupied);

	SetWalkSpeed(WalkSpeed::Walk);

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
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, 
		ECollisionChannel::ECC_Visibility, QueryParams);
	
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

}

void APlayerCharacter::SetCameraComponent()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
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
	if (!bMove)
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
		const FVector Start = FVector(CrosshairWorldPosition);
		const FVector End = FVector(Start + CrosshairWorldDirection* 50'000.f);
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, 
			ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
			return true;
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
	bMove = false;
	SetPlayerInputMode(true);
	GetCharacterMovement()->Velocity = FVector(0.f);
	TransferWidget->SetVisibility(ESlateVisibility::Visible);
}

void APlayerCharacter::RemoveWidget_Implementation()
{
	bMove = true;
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

void APlayerCharacter::SetWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::Sprint()
{
	SetActionState(EActionState::EAS_Sprint);
	SetWalkSpeed(WalkSpeed::Run);
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
	PRINT_TEXT("Pressed Middle Mouse Key");
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

void APlayerCharacter::StartTrail(EActionState Action)
{
	MakeTrail();

	if (ActionState == Action)
	{
		static FTimerHandle TrailTimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName(TEXT("TrailTimerReset")), Action);
		GetWorldTimerManager().SetTimer(TrailTimerHandle, TimerDel, TimerRate::AutomaticTrailRate, false);
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

void APlayerCharacter::SprintTrail()
{
	static FTimerHandle SprintTimerHandle;
	bTrail = false;
	GetWorldTimerManager().SetTimer(SprintTimerHandle, [this]()
		{
			if (bTrail == false)
			{
				GetWorldTimerManager().ClearTimer(SprintTimerHandle);
				SprintTrail();
				bTrail = true;
				MakeTrail();
			}
		}, TimerRate::SprintRate, true);
}

void APlayerCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	for (int i = 0; i < DynamicDissolveMaterialInstances.Num(); ++i)
	{
		if (DynamicDissolveMaterialInstances[i])
			DynamicDissolveMaterialInstances[i]->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void APlayerCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &APlayerCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
	
}

void APlayerCharacter::TrailTimerReset(EActionState Action)
{
	if (ActionState == Action)
	{
		StartTrail(Action);
	}
}