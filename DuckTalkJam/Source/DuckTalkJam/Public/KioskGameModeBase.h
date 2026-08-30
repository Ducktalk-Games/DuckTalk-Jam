// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KioskGameplayEvent.h"
#include "KioskRule.h"
#include "KioskGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API AKioskGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKioskGameModeBase();

	UFUNCTION(BlueprintCallable)
	void OrchestrateEncounter();

	UFUNCTION(BlueprintCallable)
	void OrchestrateEvent();

#pragma region Characters

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AKioskCharacter>> PossibleCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AKioskCharacter>> CharactersLetIn;

	UFUNCTION(BlueprintCallable)
	void OrchestrateRandomCharacter(AKioskCharacter* Character);

#pragma endregion Characters

#pragma region Rules

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UKioskRule>> PossibleRules;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKioskRule>> AppliedRules;

	UFUNCTION(BlueprintCallable)
	void ProcessCharacter(AKioskCharacter* Character);

	UFUNCTION(BlueprintCallable)
	bool DoesCharacterViolateRules(AKioskCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void PenalizePlayer();

#pragma endregion Rules

#pragma region Events

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UKioskGameplayEvent>> PossibleEvents;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKioskGameplayEvent>> ActiveEvents;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKioskGameplayEvent>> HappenedEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b_EventHappening = false;

	void ProcessActiveEvents();

#pragma endregion Events

protected:
	virtual void Tick(float DeltaSeconds) override;
};