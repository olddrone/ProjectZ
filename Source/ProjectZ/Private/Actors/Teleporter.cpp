// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Teleporter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ProjectZ/DebugMacros.h"

ATeleporter::ATeleporter() : MapName(TEXT("Defaults"))
{
 	PrimaryActorTick.bCanEverTick = true;

	TeleportMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportMesh"));
	TeleportMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TeleportMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(TeleportMesh);

	AreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaMesh"));
	AreaMesh->SetupAttachment(GetRootComponent());

	WidgetArea = CreateDefaultSubobject<USphereComponent>(TEXT("WidgetArea"));
	WidgetArea->SetupAttachment(GetRootComponent());

	TeleportArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TeleportArea"));
	TeleportArea->SetupAttachment(GetRootComponent());

	TeleportEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportEffect"));
	TeleportEffect->SetupAttachment(GetRootComponent());

	InfomationWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfomationWidget"));
	InfomationWidget->SetupAttachment(GetRootComponent());
	InfomationWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InfomationWidget->SetDrawSize(FVector2D(300, 300));

}

void ATeleporter::BeginPlay()
{
	Super::BeginPlay();

	TeleportArea->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnCapsuleOverlap);
	TeleportArea->OnComponentEndOverlap.AddDynamic(this, &ATeleporter::OnCapsuleEndOverlap);
	
	InfomationWidget->SetVisibility(false);
}

void ATeleporter::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		Character->SetActorLocation(GetActorLocation());
		Character->StopMovement(); 
		Character->InitMapName(MapName);
		Character->SetOverlappingTeleport(this);
		Character->DisplayWidget();

	}
}

void ATeleporter::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{

		Character->InitMapName(FString("Defaults"));
		Character->SetOverlappingTeleport(nullptr);
	}
}

void ATeleporter::OpenMap()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(MapName));
}

