// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KioskGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API AKioskGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<A_Character> PossibleCharacters;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<A_Character> CharactersLetIn;

	UFUNCTION(BlueprintCallable)
	void OrchestrateRandomCharacter(A_Character& Character);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Events> PossibleEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Events> HappenedEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b_EventHappening = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Rule> PossibleRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Rule> AppliedRules;
	
	*/

	UFUNCTION(BlueprintCallable)
	void OrchestrateEncounter();

	UFUNCTION(BlueprintCallable)
	void OrchestrateEvent();
};