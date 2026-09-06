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

	KioskState = GetGameInstance<UKioskState>();
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
	KioskState->Coins = FMath::Max(
		KioskState->Coins + DayWage - (Mistakes * MistakePenalty),
		0
	);
	++Day;

	OnEndRound.Broadcast();

	PrepareForNextRound();
	SetKioskPhase(EKioskPhase::Setup);
}

void AKioskGameModeBase::PrepareForNextRound()
{
	CurrentEncounterIndex = 0;
	CurrentEncounter = nullptr;
	CurrentCharacterEntry = FKioskCharacterEntry();
	b_EncounterInProgress = false;
	b_EncounterResolved = false;
	b_DialogueFinished = false;
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
			break;
		case EKioskPhase::Shopping:
			break;
	}

	OnPhaseChanged.Broadcast(NewPhase);
}

void AKioskGameModeBase::OrchestrateEncounter(bool& bEncountersLeft)
{
	bEncountersLeft = false;

	if (!IsGamePhase(EKioskPhase::Playing) ||
		b_EncounterInProgress ||
		EncountersPerDay.IsEmpty() ||
		!KioskState)
	{
		return;
	}

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(Day);
	if (!DayConfig || !DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex))
	{
		OnNoEncounters.Broadcast();
		return;
	}

	const auto& EncounterData = DayConfig->CharacterOrder[CurrentEncounterIndex];
	TSubclassOf<AKioskCharacter> CharacterClass = EncounterData.CharacterClass;
	if (!CharacterClass) return;

	UDataTable* CharacterDialogueTable = EncounterData.CharacterConversationTable;
	if (!CharacterDialogueTable) return;

	FGameplayTagContainer CharacterTraits = EncounterData.Traits;
	if (CharacterTraits.IsEmpty()) return;

	UTexture2D* CharacterTexture = EncounterData.CurrentCharacterTexture;
	if (!CharacterTexture) return;

	FCharacterSex CharacterSex = EncounterData.Sex;
	AKioskCharacter* InWorldCharacter = Cast<AKioskCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), CharacterClass));

	if (!InWorldCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("No in-world actor found for %s"), *CharacterClass->GetName());
		return;
	}

	CurrentEncounter = CharacterClass;
	CurrentEncounterCharacter = InWorldCharacter;
	CurrentCharacterEntry = EncounterData;
	b_EncounterInProgress = true;

	b_EncounterResolved = false;
	b_DialogueFinished = false;

	bEncountersLeft = DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1);
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

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(Day);
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
	if (!CurrentCharacterEntry.CharacterClass || !Character) return;

	Character->GrantedEntry();

	if (DoesCharacterViolateRules())
		PenalizePlayer(Character, CurrentCharacterEntry.Traits); // should have been kept out
	else
		RewardPlayer(Character, CurrentCharacterEntry.Traits); // correctly let in

	EncounterCharactersLetIn.Add(CurrentEncounter);

	CurrentEncounter = nullptr;
	CurrentCharacterEntry = FKioskCharacterEntry();
	b_EncounterInProgress = false;

	++CurrentEncounterIndex;
}

void AKioskGameModeBase::TurnAwayCharacter(AKioskCharacter* Character)
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!CurrentCharacterEntry.CharacterClass || !Character) return;

	Character->RejectedEntry();

	if (DoesCharacterViolateRules())
		RewardPlayer(Character, CurrentCharacterEntry.Traits); // correctly kept out
	else
		PenalizePlayer(Character, CurrentCharacterEntry.Traits); // should have been let in

	CurrentEncounter = nullptr;
	CurrentCharacterEntry = FKioskCharacterEntry();
	b_EncounterInProgress = false;

	++CurrentEncounterIndex;
}

void AKioskGameModeBase::HandleDialogueEnded(bool bWasPhoneDialogue)
{
	if (bWasPhoneDialogue) return;
	
	b_DialogueFinished = true;
	TryAdvanceEncounter();
}

void AKioskGameModeBase::HandleEncounterExitFinished()
{
	b_EncounterResolved = true;
	TryAdvanceEncounter();
}

void AKioskGameModeBase::TryAdvanceEncounter()
{
	if (!b_EncounterResolved || !b_DialogueFinished)return;

	b_EncounterResolved = false;
	b_DialogueFinished = false;

	bool bEncountersLeft = false;
	OrchestrateEncounter(bEncountersLeft);
}

bool AKioskGameModeBase::DoesCharacterViolateRules()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return false;

	for (UKioskRule* Rule : AppliedRules)
	{
		if (Rule && Rule->IsViolatedBy(CurrentCharacterEntry.Traits)) return true;
	}

	return false;
}

void AKioskGameModeBase::PenalizePlayer(AKioskCharacter* Character, FGameplayTagContainer Traits)
{
	++Mistakes;
	OnPenalizePlayer.Broadcast(Character, Traits);
}

void AKioskGameModeBase::RewardPlayer(AKioskCharacter* Character, FGameplayTagContainer Traits)
{
	++CorrectlyProcessed;
	OnRewardPlayer.Broadcast(Character, Traits);
}

void AKioskGameModeBase::AddPayDock(FName DockName, float Amount)
{
	if (!PayDocks.Contains(DockName))
	{
		PayDocks.Add(DockName, Amount);
	}
}

bool AKioskGameModeBase::HasEncountersLeft() const
{
	if (!KioskState || EncountersPerDay.IsEmpty())
	{
		return false;
	}

	if (const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(Day))
	{
		if (DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1))
		{
			return true;
		}
	}

	for (const auto& Pair : EncountersPerDay)
	{
		if (Pair.Key > Day && !Pair.Value.CharacterOrder.IsEmpty())
		{
			return true;
		}
	}

	return false;
}