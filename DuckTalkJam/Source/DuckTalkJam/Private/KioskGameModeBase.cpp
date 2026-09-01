// Fill out your copyright notice in the Description page of Project Settings.


#include "KioskGameModeBase.h"
#include "KioskCharacter.h"
#include "KioskState.h"

AKioskGameModeBase::AKioskGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKioskGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	KioskState = GetGameState<AKioskState>();

	if (!KioskState)
	{
		UE_LOG(LogTemp, Error, TEXT("KioskState is null!"));
		return;
	}

	StartRound();
}

void AKioskGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ProcessActiveEvents();
}

void AKioskGameModeBase::StartRound()
{
	OnStartRound.Broadcast();
}

void AKioskGameModeBase::EndRound()
{
	OnEndRound.Broadcast();
}

bool AKioskGameModeBase::IsKioskPhase(EKioskPhase Phase) const
{
	if (!KioskState) return false;
	return KioskState && KioskState->CurrentPhase == Phase;
}

void AKioskGameModeBase::OrchestrateEncounter()
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;
	if (EncountersPerDay.IsEmpty() || b_EncounterInProgress) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day);
	if (!DayConfig && !DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex)) return;
	
	TSubclassOf<AKioskCharacter> CharacterClass = DayConfig->CharacterOrder[CurrentEncounterIndex].CharacterClass;
	if (!CharacterClass) return;

	CurrentEncounter = CharacterClass;
	b_EncounterInProgress = true;
	OnEncounterStarted.Broadcast(CharacterClass);
}

void AKioskGameModeBase::OrchestrateEvent()
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;
	if (PossibleEvents.IsEmpty() || b_EventHappening) return;

	const int32 RandomIndex = FMath::RandRange(0, PossibleEvents.Num() - 1); // Get a random index from PossibleEvents
	TSubclassOf<UKioskGameplayEvent> EventClass = PossibleEvents[RandomIndex]; // get the event class at that index

	UKioskGameplayEvent* Event = NewObject<UKioskGameplayEvent>(this, EventClass); // Create an instance of that event class
	if (!Event) return;

	Event->StartEvent(this);
	b_EventHappening = true;
	ActiveEvents.Add(Event);
}

void AKioskGameModeBase::OrchestrateRules()
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;

}

void AKioskGameModeBase::ProcessActiveEvents()
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;

	for (int32 i = ActiveEvents.Num() - 1; i >= 0; --i)
	{
		UKioskGameplayEvent* Event = ActiveEvents[i];

		if (!Event)
		{
			ActiveEvents.RemoveAt(i);
			continue;
		}

		if (Event->IsCompleted(this))
		{
			HappenedEvents.Add(Event);
			ActiveEvents.RemoveAt(i);
		}
	}

	b_EventHappening = !ActiveEvents.IsEmpty();
}

void AKioskGameModeBase::OrchestrateRandomCharacter(AKioskCharacter* Character)
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;

}

void AKioskGameModeBase::ProcessCharacter(AKioskCharacter* Character)
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;
	if (!CurrentEncounter || !Character) return;

	if (DoesCharacterViolateRules(Character))
	{
		PenalizePlayer();
	}

	EncounterCharactersLetIn.Add(CurrentEncounter);

	CurrentEncounter = nullptr;
	b_EncounterInProgress = false;
}

bool AKioskGameModeBase::DoesCharacterViolateRules(AKioskCharacter* Character)
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return false;
	if (!Character) return false;

	for (UKioskRule* Rule : AppliedRules)
	{
		if (Rule && Rule->IsViolatedBy(Character)) return true;
	}

	return false;
}

void AKioskGameModeBase::PenalizePlayer()
{

}