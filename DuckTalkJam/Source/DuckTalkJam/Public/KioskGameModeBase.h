// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KioskGameplayEvent.h"
#include "KioskRule.h"
#include "KioskState.h"
#include "CharacterSex.h"
#include "KioskGameModeBase.generated.h"

class AKioskCharacter;
class AKioskState;

UENUM(BlueprintType)
enum class EKioskPhase : uint8
{
	None 		UMETA(DisplayName = "None"),
	Setup		UMETA(DisplayName = "Setup"),
	StartOfDay	UMETA(DisplayName = "Start Of Day"),
	Playing		UMETA(DisplayName = "Playing"),
	EndOfDay	UMETA(DisplayName = "End Of Day"),
	Shopping	UMETA(DisplayName = "Shopping")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoEncounters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EKioskPhase, Phase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncounterStarted, AKioskCharacter*, CharacterActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPenalizePlayer, AKioskCharacter*, Character, FGameplayTagContainer, Traits);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRewardPlayer, AKioskCharacter*, Character, FGameplayTagContainer, Traits);

UCLASS()
class DUCKTALKJAM_API AKioskGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKioskGameModeBase();

	UPROPERTY()
	UKioskState* KioskState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 Day = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	EKioskPhase CurrentPhase = EKioskPhase::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FirstEncounterDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SubsequentEncounterDelay = 15.0f;

	UFUNCTION(BlueprintPure)
	bool IsGamePhase(EKioskPhase Phase) const;

	UFUNCTION(BlueprintCallable)
	void SetKioskPhase(EKioskPhase NewPhase);

	FTimerHandle EncounterTimerHandle;
	UFUNCTION(BlueprintCallable)
	void OrchestrateEncounter(bool& bEncountersLeft);

	UFUNCTION(BlueprintCallable)
	void OrchestrateEvent();

	UFUNCTION(BlueprintCallable)
	void OrchestrateRules();

#pragma region GameplayEvents

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnStartRound OnStartRound;

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnEndRound OnEndRound;

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnPenalizePlayer OnPenalizePlayer;

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnRewardPlayer OnRewardPlayer;

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnEncounterStarted OnEncounterStarted;

	UPROPERTY(BlueprintAssignable, Category = "Kiosk|Events")
	FOnNoEncounters OnNoEncounters;

	UFUNCTION(BlueprintCallable)
	void StartRound();

	UFUNCTION(BlueprintCallable)
	void EndRound();

	void PrepareForNextRound();

#pragma endregion GameplayEvents

#pragma region Characters

#pragma region Day Progression

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	TMap<int32, FDayEncounterConfig> EncountersPerDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DayWage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MistakePenalty = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> PayDocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CorrectlyProcessed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mistakes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b_EncounterInProgress = false;

	UFUNCTION(BlueprintPure)
	bool HasEncountersLeft() const;

#pragma endregion Day Progression

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<AKioskCharacter> CurrentEncounter;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AKioskCharacter> CurrentEncounterCharacter;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentEncounterIndex = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<AKioskCharacter>> EncounterCharactersLetIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKioskCharacterEntry CurrentCharacterEntry;

#pragma endregion Characters

#pragma region Rules

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UKioskRule>> PossibleRules;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKioskRule>> AppliedRules;

	UFUNCTION(BlueprintCallable)
	void ProcessCharacter(AKioskCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void TurnAwayCharacter(AKioskCharacter* Character);

	UFUNCTION(BlueprintCallable)
	bool DoesCharacterViolateRules();

	UFUNCTION(BlueprintCallable)
	void PenalizePlayer(AKioskCharacter* Character, FGameplayTagContainer Traits);

	UFUNCTION(BlueprintCallable)
	void RewardPlayer(AKioskCharacter* Character, FGameplayTagContainer Traits);

	UFUNCTION(BlueprintCallable)
	void AddPayDock(FName DockName, float Amount);

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

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};