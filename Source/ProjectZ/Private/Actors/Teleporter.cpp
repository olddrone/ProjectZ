// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Teleporter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

ATeleporter::ATeleporter() : MapName(TEXT("Defaults"))
{
 	PrimaryActorTick.bCanEverTick = true;

	TeleporterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleporterMesh"));
	TeleporterMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TeleporterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(TeleporterMesh);

	TeleporterCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TeleporterCapsule"));
	TeleporterCapsule->SetupAttachment(GetRootComponent());

	TeleporterEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleporterEffect"));
	TeleporterEffect->SetupAttachment(GetRootComponent());

	WidgetArea = CreateDefaultSubobject<USphereComponent>(TEXT("WidgetArea"));
	WidgetArea->SetupAttachment(GetRootComponent());

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(GetRootComponent());
	Widget->SetWidgetSpace(EWidgetSpace::Screen);
 	Widget->SetDrawSize(FVector2D(200, 50));

}

void ATeleporter::BeginPlay()
{
	Super::BeginPlay();

	TeleporterCapsule->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnCapsuleOverlap);
	
	WidgetArea->OnComponentBeginOverlap.AddDynamic(this, &ATeleporter::OnAreaOverlap);
	WidgetArea->OnComponentEndOverlap.AddDynamic(this, &ATeleporter::OnAreaEndOverlap);

	Widget->SetVisibility(false);
}

void ATeleporter::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor);
	if (Character)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(MapName));
	}
}

void ATeleporter::OnAreaOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Widget->SetVisibility(true);
}

void ATeleporter::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Widget->SetVisibility(false);
}

