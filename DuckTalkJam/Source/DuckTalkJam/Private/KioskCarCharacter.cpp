// Fill out your copyright notice in the Description page of Project Settings.


#include "KioskCarCharacter.h"
#include "Engine/DataTable.h"

// Sets default values
AKioskCarCharacter::AKioskCarCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKioskCarCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AKioskCarCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKioskCarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}