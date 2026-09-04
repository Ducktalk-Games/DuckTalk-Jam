// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KioskGameplayEvent.h"
#include "KioskRule.h"
#include "KioskState.h"
#include "KioskGameModeBase.generated.h"

class AKioskCharacter;
class AKioskState;

UENUM(BlueprintType)
enum class EKioskPhase : uint8
{
	None 		UMETA(DisplayName = "None"),
	Setup		UMETA(DisplayName = "Setup"),
	Playing		UMETA(DisplayName = "Playing"),
	EndOfDay	UMETA(DisplayName = "End Of Day"),
	Shopping	UMETA(DisplayName = "Shopping")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EKioskPhase, Phase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncounterStarted, AKioskCharacter*, CharacterActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPenalizePlayer, AKioskCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRewardPlayer, AKioskCharacter*, Character);

UCLASS()
class DUCKTALKJAM_API AKioskGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKioskGameModeBase();

	UPROPERTY()
	TObjectPtr<AKioskState> KioskState;

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
	void TryOrchestrateEncounter();

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

	UFUNCTION(BlueprintCallable)
	void StartRound();

	UFUNCTION(BlueprintCallable)
	void EndRound();

#pragma endregion GameplayEvents

#pragma region Characters

#pragma region Day Progression

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounters")
	TMap<int32, FDayEncounterConfig> EncountersPerDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoinPenalty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CoinReward;

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
	int32 CorrectlyLetIn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mistakes = 0;

	UFUNCTION(BlueprintCallable)
	void OrchestrateRandomCharacter(AKioskCharacter* Character);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool b_EncounterInProgress = false;

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
	bool DoesCharacterViolateRules(AKioskCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void PenalizePlayer(AKioskCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void RewardPlayer(AKioskCharacter* Character);

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