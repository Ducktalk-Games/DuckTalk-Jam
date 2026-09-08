// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskGameModeBase.h"
#include "KioskCharacter.h"
#include "KioskState.h"
#include "CharacterSex.h"
#include "KioskGameplayEvent.h"
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
	KioskState->Coins = FMath::Max(KioskState->Coins + DayWage - (Mistakes * MistakePenalty), 0);
	++Day;

	OnEndRound.Broadcast();

	if (!EncountersPerDay.Contains(Day))
	{
		EndGame();
		return;
	}

	PrepareForNextRound();
	SetKioskPhase(EKioskPhase::Setup);
}

void AKioskGameModeBase::EndGame()
{
	OnEndGame.Broadcast();
	SetKioskPhase(EKioskPhase::Credits);
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
			ClearDayExclusiveEvents();
			break;
		case EKioskPhase::Setup:
			ClearDayExclusiveEvents();
			OrchestrateDayExclusiveEvents();
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
	UE_LOG(LogTemp, Log, TEXT("=== OrchestrateEncounter START ==="));
	UE_LOG(LogTemp, Log,
		TEXT("Day: %d | CurrentEncounterIndex: %d | EncounterInProgress: %s"),
		Day,
		CurrentEncounterIndex,
		b_EncounterInProgress ? TEXT("true") : TEXT("false")
	);

	bEncountersLeft = false;

	if (!IsGamePhase(EKioskPhase::Playing))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OrchestrateEncounter aborted: Game phase is not Playing.")
		);
		return;
	}

	if (b_EncounterInProgress)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OrchestrateEncounter aborted: An encounter is already in progress.")
		);
		return;
	}

	if (EncountersPerDay.IsEmpty())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OrchestrateEncounter aborted: EncountersPerDay is empty.")
		);
		return;
	}

	if (!KioskState)
	{
		UE_LOG(LogTemp, Error,
			TEXT("OrchestrateEncounter aborted: KioskState is null.")
		);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("Looking for encounter configuration for Day %d."),
		Day
	);

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(Day);

	if (!DayConfig)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OrchestrateEncounter aborted: No DayConfig found for Day %d."),
			Day
		);

		UE_LOG(LogTemp, Log, TEXT("Broadcasting OnNoEncounters."));
		OnNoEncounters.Broadcast();
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("DayConfig found. CharacterOrder.Num(): %d"),
		DayConfig->CharacterOrder.Num()
	);

	if (!DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OrchestrateEncounter aborted: Invalid encounter index %d. CharacterOrder.Num(): %d"),
			CurrentEncounterIndex,
			DayConfig->CharacterOrder.Num()
		);

		UE_LOG(LogTemp, Log, TEXT("Broadcasting OnNoEncounters."));
		OnNoEncounters.Broadcast();
		return;
	}

	const auto& EncounterData = DayConfig->CharacterOrder[CurrentEncounterIndex];

	UE_LOG(LogTemp, Log,
		TEXT("Encounter data found for index %d."),
		CurrentEncounterIndex
	);

	TSubclassOf<AKioskCharacter> CharacterClass = EncounterData.CharacterClass;

	if (!CharacterClass)
	{
		UE_LOG(LogTemp, Error,
			TEXT("OrchestrateEncounter aborted: CharacterClass is null at encounter index %d."),
			CurrentEncounterIndex
		);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("CharacterClass: %s"),
		*GetNameSafe(CharacterClass.Get())
	);

	UDataTable* CharacterDialogueTable = EncounterData.CharacterConversationTable;

	if (!CharacterDialogueTable)
	{
		UE_LOG(LogTemp, Error,
			TEXT("OrchestrateEncounter aborted: CharacterConversationTable is null for character %s."),
			*GetNameSafe(CharacterClass.Get())
		);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("CharacterConversationTable: %s"),
		*GetNameSafe(CharacterDialogueTable)
	);

	const FGameplayTagContainer CharacterTraits = EncounterData.Traits;

	if (CharacterTraits.IsEmpty())
	{
		UE_LOG(LogTemp, Error,
			TEXT("OrchestrateEncounter aborted: CharacterTraits is empty for character %s."),
			*GetNameSafe(CharacterClass.Get())
		);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("CharacterTraits: %s"),
		*CharacterTraits.ToStringSimple()
	);

	UTexture2D* CharacterTexture = EncounterData.CurrentCharacterTexture;

	if (!CharacterTexture)
	{
		UE_LOG(LogTemp, Error,
			TEXT("OrchestrateEncounter aborted: CurrentCharacterTexture is null for character %s."),
			*GetNameSafe(CharacterClass.Get())
		);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("CharacterTexture: %s"),
		*GetNameSafe(CharacterTexture)
	);

	const ECharacterSex CharacterSex = EncounterData.Sex;

	UE_LOG(LogTemp, Log,
		TEXT("Searching world for actor of class: %s"),
		*GetNameSafe(CharacterClass.Get())
	);

	AKioskCharacter* InWorldCharacter = Cast<AKioskCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), CharacterClass)
	);

	if (!InWorldCharacter)
	{
		UE_LOG(LogTemp, Error,
			TEXT("OrchestrateEncounter aborted: Could not find an in-world AKioskCharacter of class %s."),
			*GetNameSafe(CharacterClass.Get())
		);
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("Found in-world character: %s"),
		*GetNameSafe(InWorldCharacter)
	);

	CurrentEncounter = CharacterClass;
	CurrentEncounterCharacter = InWorldCharacter;
	CurrentCharacterEntry = EncounterData;

	b_EncounterInProgress = true;
	b_EncounterResolved = false;
	b_DialogueFinished = false;

	UE_LOG(LogTemp, Log,
		TEXT("Encounter state initialized. InProgress: %s | Resolved: %s | DialogueFinished: %s"),
		b_EncounterInProgress ? TEXT("true") : TEXT("false"),
		b_EncounterResolved ? TEXT("true") : TEXT("false"),
		b_DialogueFinished ? TEXT("true") : TEXT("false")
	);

	bEncountersLeft = DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1);

	UE_LOG(LogTemp, Log,
		TEXT("Encounter started. Index: %d | Character: %s | EncountersLeft: %s"),
		CurrentEncounterIndex,
		*GetNameSafe(InWorldCharacter),
		bEncountersLeft ? TEXT("true") : TEXT("false")
	);

	UE_LOG(LogTemp, Log, TEXT("Broadcasting OnEncounterStarted."));
	OnEncounterStarted.Broadcast(InWorldCharacter);

	UE_LOG(LogTemp, Log, TEXT("=== OrchestrateEncounter END ==="));
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

void AKioskGameModeBase::OrchestrateEvent()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (PossibleEvents.IsEmpty() || b_EventHappening) return;

	const int32 RandomIndex = FMath::RandRange(0, PossibleEvents.Num() - 1);
	TSubclassOf<AKioskGameplayEvent> EventClass = PossibleEvents[RandomIndex];

	if (!EventClass) return;

	AKioskGameplayEvent* Event = GetWorld()->SpawnActor<AKioskGameplayEvent>(
		EventClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator);
	if (!Event) return;

	b_EventHappening = true;
	ActiveEvents.Add(Event);

	Event->StartEvent(this);
}

void AKioskGameModeBase::OrchestrateDayExclusiveEvents()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!KioskState) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(Day);
	if (!DayConfig) return;

	for (TSubclassOf<AKioskGameplayEvent> EventClass : DayConfig->DayExclusiveEvents)
	{
		if (!EventClass) continue;

		AKioskGameplayEvent* Event = GetWorld()->SpawnActor<AKioskGameplayEvent>(
			EventClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator);

		if (!Event) continue;

		ActiveDayExclusiveEvents.Add(Event);
		UE_LOG(LogTemp, Warning, TEXT("Starting day-exclusive event '%s' for Day %d."), *GetNameSafe(Event), Day);
		Event->StartEvent(this);
	}
}

void AKioskGameModeBase::ClearDayExclusiveEvents()
{
	if (ActiveDayExclusiveEvents.IsEmpty()) return;

	for (AKioskGameplayEvent* Event : ActiveDayExclusiveEvents)
	{
		if (!IsValid(Event)) continue;
		Event->Destroy();
	}

	ActiveDayExclusiveEvents.Empty();
	UE_LOG(LogTemp, Warning, TEXT("Cleared day-exclusive events."));
}

void AKioskGameModeBase::OnGameplayEventCompleted(AKioskGameplayEvent* Event)
{
	if (!IsValid(Event)) return;

	ActiveEvents.Remove(Event);
	HappenedEvents.Add(Event);
	Event->Destroy();
}

void AKioskGameModeBase::ProcessCharacter(AKioskCharacter* Character)
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!CurrentCharacterEntry.CharacterClass || !Character) return;

	Character->GrantedEntry();

	switch (EvaluateCharacterRules())
	{
		case ERuleEvaluation::Forbidden:
			PenalizePlayer(Character, CurrentCharacterEntry.Traits); // should have been kept out
			break;
		
		case ERuleEvaluation::RequiredToEnter:
			RewardPlayer(Character, CurrentCharacterEntry.Traits); // correctly let in
			break;

		case ERuleEvaluation::NoApplicableRule:
			// n/a
			break;
	}


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

	switch (EvaluateCharacterRules())
	{
		case ERuleEvaluation::Forbidden:
			RewardPlayer(Character, CurrentCharacterEntry.Traits); // correctly kept out
			break;

		case ERuleEvaluation::RequiredToEnter:
			PenalizePlayer(Character, CurrentCharacterEntry.Traits); // should have been let in
			break;

		case ERuleEvaluation::NoApplicableRule:
			// n/a
			break;
	}

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
	CurrentEncounterCharacter;
}

void AKioskGameModeBase::TryAdvanceEncounter()
{
	if (!b_EncounterResolved || !b_DialogueFinished) return;

	b_EncounterResolved = false;
	b_DialogueFinished = false;

	GetWorldTimerManager().SetTimer(
		TimerBetweenEncounters,
		this,
		&AKioskGameModeBase::AdvanceEncounter,
		10.0f,
		false
	);
}

void AKioskGameModeBase::AdvanceEncounter()
{
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

ERuleEvaluation AKioskGameModeBase::EvaluateCharacterRules() const
{
	bool bRequiredToEnter = false;

	for (UKioskRule* Rule : AppliedRules)
	{
		if (!Rule) continue;

		const bool bMatchesRule = Rule->IsViolatedBy(CurrentCharacterEntry.Traits);

		if (!bMatchesRule) continue;

		switch (Rule->RuleType)
		{
			case EKioskRuleType::Forbiden: return ERuleEvaluation::Forbidden;
			case EKioskRuleType::RequiredEntry: bRequiredToEnter = true; break;
		}
	}

	if (bRequiredToEnter) return ERuleEvaluation::RequiredToEnter;

	return ERuleEvaluation::NoApplicableRule;
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