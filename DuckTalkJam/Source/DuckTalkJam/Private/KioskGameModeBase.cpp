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
	SetKioskPhase(EKioskPhase::Setup);
}

void AKioskGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ProcessActiveEvents();
}

void AKioskGameModeBase::StartRound()
{
	SetKioskPhase(EKioskPhase::Playing);
	GetWorldTimerManager().SetTimer(
		EncounterTimerHandle,
		this,
		&AKioskGameModeBase::TryOrchestrateEncounter,
		SubsequentEncounterDelay,
		true,
		FirstEncounterDelay
	);
	OnStartRound.Broadcast();
}

void AKioskGameModeBase::EndRound()
{
	OnEndRound.Broadcast();
}

bool AKioskGameModeBase::IsGamePhase(EKioskPhase Phase) const
{
	return CurrentPhase == Phase;
}

void AKioskGameModeBase::SetKioskPhase(EKioskPhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;
	switch (CurrentPhase)
	{
		case EKioskPhase::None: break;
		case EKioskPhase::Setup: break;
		case EKioskPhase::Playing: StartRound(); break;
		case EKioskPhase::EndOfDay: EndRound(); break;
	}

	OnPhaseChanged.Broadcast(NewPhase);
}

void AKioskGameModeBase::TryOrchestrateEncounter()
{
	if (!IsGamePhase(EKioskPhase::Playing) || b_EncounterInProgress || !CurrentEncounter) return;

	bool bEncountersLeft = false;
	OrchestrateEncounter(bEncountersLeft);

	if (!bEncountersLeft)
	{
		GetWorldTimerManager().ClearTimer(EncounterTimerHandle);
		CurrentPhase = EKioskPhase::EndOfDay;
		EndRound();
	}
}

void AKioskGameModeBase::OrchestrateEncounter(bool& bEncountersLeft)
{
	bEncountersLeft = false;

	if (!IsGamePhase(EKioskPhase::Playing) || EncountersPerDay.IsEmpty()) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day);
	if (!DayConfig || !DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex)) return;

	TSubclassOf<AKioskCharacter> CharacterClass = DayConfig->CharacterOrder[CurrentEncounterIndex].CharacterClass;
	if (!CharacterClass) return;

	const bool bHasMoreEncountersToday = DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1);
	bool bHasMoreDays = false;
	for (const auto& Pair : EncountersPerDay)
	{
		if (Pair.Key > KioskState->Day)
		{
			bHasMoreDays = true;
			break;
		}
	}

	bEncountersLeft = bHasMoreEncountersToday || bHasMoreDays;

	CurrentEncounter = CharacterClass;
	b_EncounterInProgress = true;

	OnEncounterStarted.Broadcast(CharacterClass);
}

void AKioskGameModeBase::OrchestrateEvent()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
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
	if (!IsGamePhase(EKioskPhase::Playing)) return;

}

void AKioskGameModeBase::ProcessActiveEvents()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;

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
	if (!IsGamePhase(EKioskPhase::Playing)) return;

}

void AKioskGameModeBase::ProcessCharacter(AKioskCharacter* Character)
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!CurrentEncounter || !Character) return;

	if (DoesCharacterViolateRules(Character)) PenalizePlayer();
	else ++CorrectlyLetIn;

	EncounterCharactersLetIn.Add(CurrentEncounter);

	CurrentEncounter = nullptr;
	b_EncounterInProgress = false;

	++CurrentEncounterIndex;
}

bool AKioskGameModeBase::DoesCharacterViolateRules(AKioskCharacter* Character)
{
	if (!IsGamePhase(EKioskPhase::Playing)) return false;
	if (!Character) return false;

	for (UKioskRule* Rule : AppliedRules)
	{
		if (Rule && Rule->IsViolatedBy(Character)) return true;
	}

	return false;
}

void AKioskGameModeBase::PenalizePlayer()
{
	OnPenalizePlayer.Broadcast();
}