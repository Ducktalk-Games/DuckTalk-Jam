// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskGameModeBase.h"
#include "KioskCharacter.h"
#include "KioskState.h"
#include "CharacterSex.h"
#include "Kismet/GameplayStatics.h"

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
	PayDocks.Empty();
	OnStartRound.Broadcast();

	bool bEncountersLeft = false;
	OrchestrateEncounter(bEncountersLeft);
}

void AKioskGameModeBase::EndRound()
{
	OnEndRound.Broadcast();
	KioskState->Day++;
	PrepareForNextRound();
	SetKioskPhase(EKioskPhase::Setup);
}

void AKioskGameModeBase::PrepareForNextRound()
{
	CurrentEncounterIndex = 0;
	CurrentEncounter = nullptr;
	CurrentCharacterDialogueTable = nullptr;
	CurrentCharacterTraits = FGameplayTagContainer();
	CurrentCharacterTexture = nullptr;
	CurrentCharacterSex = FCharacterSex::Female;
	b_EncounterInProgress = false;
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
		case EKioskPhase::None:
			break;

		case EKioskPhase::Setup:
			PrepareForNextRound();
			break;

		case EKioskPhase::Playing:
			StartRound();
			break;

		case EKioskPhase::EndOfDay:
			// Do NOT clear encounter data here.
			break;

		case EKioskPhase::Shopping:
			break;
	}

	OnPhaseChanged.Broadcast(NewPhase);
}

void AKioskGameModeBase::OrchestrateEncounter(bool& bEncountersLeft)
{
	bEncountersLeft = false;

	if (!IsGamePhase(EKioskPhase::Playing) || b_EncounterInProgress || EncountersPerDay.IsEmpty()) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day);
	if (!DayConfig || !DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex))
	{
		GetWorldTimerManager().ClearTimer(EncounterTimerHandle);
		SetKioskPhase(EKioskPhase::EndOfDay);
		return;
	}

	TSubclassOf<AKioskCharacter> CharacterClass = DayConfig->CharacterOrder[CurrentEncounterIndex].CharacterClass;
	if (!CharacterClass) return;

	TObjectPtr<UDataTable> CharacterDialogueTable = DayConfig->CharacterOrder[CurrentEncounterIndex].CharacterConversationTable;
	if (!CharacterDialogueTable) return;

	FGameplayTagContainer CharacterTraits = DayConfig->CharacterOrder[CurrentEncounterIndex].Traits;
	if (CharacterTraits.IsEmpty()) return;

	UTexture2D* CharacterTexture = DayConfig->CharacterOrder[CurrentEncounterIndex].CurrentCharacterTexture;
	if (!CharacterTexture) return;

	FCharacterSex CharacterSex = DayConfig->CharacterOrder[CurrentEncounterIndex].Sex;

	const bool bHasMoreEncountersToday = DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1);
	bool bHasMoreDays = false;
	for (const auto& Pair : EncountersPerDay)
	{
		if (Pair.Key > KioskState->Day) { bHasMoreDays = true; break; }
	}

	bEncountersLeft = bHasMoreEncountersToday || bHasMoreDays;

	AKioskCharacter* InWorldCharacter = Cast<AKioskCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), CharacterClass));
	if (!InWorldCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("No in-world actor found for %s"), *CharacterClass->GetName());
		return;
	}

	CurrentEncounter = CharacterClass;
	CurrentEncounterCharacter = InWorldCharacter;
	CurrentCharacterDialogueTable = CharacterDialogueTable;
	CurrentCharacterTraits = CharacterTraits;
	CurrentCharacterTexture = CharacterTexture;
	CurrentCharacterSex = CharacterSex;
	b_EncounterInProgress = true;

	OnEncounterStarted.Broadcast(InWorldCharacter);
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
	if (!KioskState) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day);
	if (!DayConfig) return;

	AppliedRules.Empty();
	for (TSubclassOf<UKioskRule> RuleClass : DayConfig->Rules)
	{
		if (!RuleClass) continue;
		UKioskRule* Rule = NewObject<UKioskRule>(this, RuleClass);
		if (Rule) AppliedRules.Add(Rule);
	}
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

void AKioskGameModeBase::ProcessCharacter(AKioskCharacter* Character)
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!CurrentEncounter || !Character) return;

	Character->GrantedEntry();

	if (DoesCharacterViolateRules()) PenalizePlayer(Character, CurrentCharacterTraits); // correctly kept out
	else RewardPlayer(Character, CurrentCharacterTraits);                               // should have been let in

	EncounterCharactersLetIn.Add(CurrentEncounter);

	CurrentEncounter = nullptr;
	CurrentCharacterDialogueTable = nullptr;
	CurrentCharacterTraits = FGameplayTagContainer();
	CurrentCharacterTexture = nullptr;
	CurrentCharacterSex = FCharacterSex::Female;
	b_EncounterInProgress = false;

	++CurrentEncounterIndex;

	bool bEncountersLeft = false;
	OrchestrateEncounter(bEncountersLeft);
}

void AKioskGameModeBase::TurnAwayCharacter(AKioskCharacter* Character)
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!CurrentEncounter || !Character) return;

	Character->RejectedEntry();

	if (DoesCharacterViolateRules()) RewardPlayer(Character, CurrentCharacterTraits); // correctly kept out
	else PenalizePlayer(Character, CurrentCharacterTraits);                            // should have been let in

	CurrentEncounter = nullptr;
	b_EncounterInProgress = false;
	++CurrentEncounterIndex;
}

bool AKioskGameModeBase::DoesCharacterViolateRules()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return false;

	for (UKioskRule* Rule : AppliedRules)
	{
		if (Rule && Rule->IsViolatedBy(CurrentCharacterTraits)) return true;
	}

	return false;
}

void AKioskGameModeBase::PenalizePlayer(AKioskCharacter* Character, FGameplayTagContainer Traits)
{
	OnPenalizePlayer.Broadcast(Character, Traits);
}

void AKioskGameModeBase::RewardPlayer(AKioskCharacter* Character, FGameplayTagContainer Traits)
{
	OnRewardPlayer.Broadcast(Character, Traits);
}