// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/Weapon.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon() : Damage(20.f), BoxTraceExtent(FVector(5.f)), bShowBoxDebug(false)
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Trace Start"));
	TraceStart->SetupAttachment(GetRootComponent());
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Trace End"));
	TraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	

}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetItemState(EItemState::EIS_Equipped);

	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::DisableSphereCollision()
{
	if (GetSphere())
	{
		GetSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
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