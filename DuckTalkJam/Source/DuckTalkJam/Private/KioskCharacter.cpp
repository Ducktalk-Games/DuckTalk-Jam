// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskCharacter.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKioskCharacter::AKioskCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Actor root
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Hat
	HatRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HatRoot"));
	HatRoot->SetupAttachment(RootComponent);

	HatMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HatMesh"));
	HatMesh->SetupAttachment(HatRoot);

	// Glasses
	GlassesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GlassesRoot"));
	GlassesRoot->SetupAttachment(RootComponent);

	GlassesMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassesMesh"));
	GlassesMesh->SetupAttachment(GlassesRoot);

	// Beard
	BeardRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BeardRoot"));
	BeardRoot->SetupAttachment(RootComponent);

	BeardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeardMesh"));
	BeardMesh->SetupAttachment(BeardRoot);

	// Patch
	PatchRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PatchRoot"));
	PatchRoot->SetupAttachment(RootComponent);

	PatchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PatchMesh"));
	PatchMesh->SetupAttachment(PatchRoot);

	// Tie
	TieRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TieRoot"));
	TieRoot->SetupAttachment(RootComponent);

	TieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TieMesh"));
	TieMesh->SetupAttachment(TieRoot);
}

// Called when the game starts or when spawned
void AKioskCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerReference = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

// Called every frame
void AKioskCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKioskCharacter::StartEncounter_Implementation()
{
	// Overridable.
}