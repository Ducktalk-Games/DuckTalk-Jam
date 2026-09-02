// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KioskCharacter.h"
#include "KioskCarCharacter.generated.h"

UCLASS()
class DUCKTALKJAM_API AKioskCarCharacter : public AKioskCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKioskCarCharacter();

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Car Occupants",
		meta = (ToolTip = "List of KioskCharacters that will appear in the car.")
	)
	TArray<TSubclassOf<AKioskCharacter>> Occupants;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};