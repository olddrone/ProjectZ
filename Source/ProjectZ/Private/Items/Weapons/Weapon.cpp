// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"

#include "ProjectZ/DebugMacros.h"

AWeapon::AWeapon() : Damage(20.f), BoxTraceExtent(FVector(5.f)), bShowBoxDebug(false)
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Trace Start"));
	TraceStart->SetupAttachment(GetRootComponent());
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Trace End"));
	TraceEnd->SetupAttachment(GetRootComponent());

	InterActionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InterActionWidget"));
	InterActionWidget->SetupAttachment(GetRootComponent());
	InterActionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InterActionWidget->SetDrawSize(FVector2D(500, 50));

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	GetSphere()->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	GetSphere()->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	
	InterActionWidget->SetVisibility(false);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetItemState(EItemState::EIS_Equipped);

	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	GetSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PlayEquipSound();
	ItemEffect->Deactivate();

	InterActionWidget->SetVisibility(false);
}

void AWeapon::UnEquip()
{
	SetOwner(nullptr);
	SetInstigator(nullptr);
	GetSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetActorRotation(FRotator(0.f, 0.f, 0.f));
	FVector Location = GetActorLocation();
	Location.Z = 150.f;
	SetActorLocation(Location);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetItemMesh()->AttachToComponent(InParent, TransformRules, InSocketName);
}


void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (ActorIsSameType(OtherActor, "Enemy"))
		return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
	if (ActorIsSameType(BoxHit.GetActor(), "Enemy"))
			return;

		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, 
			GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);

	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	InterActionWidget->SetVisibility(true);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	InterActionWidget->SetVisibility(false);
}


bool AWeapon::ActorIsSameType(AActor* OtherActor, FName type)
{
	return GetOwner()->ActorHasTag(type)
		&& OtherActor->ActorHasTag(type);
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent,
		TraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);

	IgnoreActors.AddUnique(BoxHit.GetActor());

}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}

}