// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "KioskCharacter.generated.h"

UCLASS()
class DUCKTALKJAM_API AKioskCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKioskCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FGameplayTagContainer Traits;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Character",
		meta = (ToolTip = "The Data Table containing all dialogue rows for this character.")
	)
	TObjectPtr<UDataTable> ConversationTable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};