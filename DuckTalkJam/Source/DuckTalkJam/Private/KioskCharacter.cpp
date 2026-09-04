// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskCharacter.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKioskCharacter::AKioskCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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