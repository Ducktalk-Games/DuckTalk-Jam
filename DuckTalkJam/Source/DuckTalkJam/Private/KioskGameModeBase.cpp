// Fill out your copyright notice in the Description page of Project Settings.


#include "KioskGameModeBase.h"

AKioskGameModeBase::AKioskGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKioskGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ProcessActiveEvents();
}

void AKioskGameModeBase::OrchestrateEncounter()
{

}

void AKioskGameModeBase::OrchestrateEvent()
{
	if (PossibleEvents.IsEmpty() || b_EventHappening) return;

	const int32 RandomIndex = FMath::RandRange(0, PossibleEvents.Num() - 1); // Get a random index from PossibleEvents
	TSubclassOf<UKioskGameplayEvent> EventClass = PossibleEvents[RandomIndex]; // get the event class at that index

	UKioskGameplayEvent* Event = NewObject<UKioskGameplayEvent>(this, EventClass); // Create an instance of that event class
	if (!Event) return;

	Event->StartEvent(this);
	b_EventHappening = true;
	ActiveEvents.Add(Event);
}

void AKioskGameModeBase::ProcessActiveEvents()
{
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

}

void AKioskGameModeBase::ProcessCharacter(AKioskCharacter* Character)
{
	if (DoesCharacterViolateRules(Character))
	{
		PenalizePlayer();
	}
}

bool AKioskGameModeBase::DoesCharacterViolateRules(AKioskCharacter* Character)
{
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