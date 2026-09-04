// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText LicensePlate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};