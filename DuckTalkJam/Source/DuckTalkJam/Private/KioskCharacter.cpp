// Fill out your copyright notice in the Description page of Project Settings.


#include "KioskCharacter.h"

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
	
}

// Called every frame
void AKioskCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKioskCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}