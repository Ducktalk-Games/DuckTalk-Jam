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
	const AKioskState* KioskState = GetGameState<AKioskState>();
	return KioskState && KioskState->CurrentPhase == Phase;
}

void AKioskGameModeBase::OrchestrateEncounter()
{
	if (!IsKioskPhase(EKioskPhase::Playing)) return;
	if (PossibleEncounterCharacters.IsEmpty() || b_EncounterInProgress) return;

	const int32 RandomIndex = FMath::RandRange(0, PossibleEncounterCharacters.Num() - 1); // Get a random index from PossibleEncounterCharacters
	TSubclassOf<AKioskCharacter> EncounterClass = PossibleEncounterCharacters[RandomIndex]; // Get the AKioskCharacter (BP_EncounterCharacter) class at that index
	if (!EncounterClass) return;

	// Likely have a period of dead air inbetween encounters.
	// Could be accomplished by having the encounters start further away
	// or by adding a delay before the encounter is spawned.

	AKioskCharacter* Encounter = GetWorld()->SpawnActor<AKioskCharacter>(EncounterClass);
	if (!Encounter) return;

	CurrentEncounter = Encounter;
	b_EncounterInProgress = true;
	OnEncounterStarted.Broadcast(Encounter);
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