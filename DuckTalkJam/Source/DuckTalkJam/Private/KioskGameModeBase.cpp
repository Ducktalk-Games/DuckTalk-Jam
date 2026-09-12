// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskGameModeBase.h"
#include "KioskCharacter.h"
#include "KioskState.h"
#include "CharacterSex.h"
#include "KioskGameplayEvent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogKiosk);

AKioskGameModeBase::AKioskGameModeBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKioskGameModeBase::BeginPlay()
{
	KioskState = GetGameInstance<UKioskState>();
	Super::BeginPlay();
	SetKioskPhase(EKioskPhase::Setup);
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
	if (!KioskState) return;

	KioskState->Coins = FMath::Max(KioskState->Coins + DayWage - (Mistakes * MistakePenalty), 0);
	++KioskState->Day;

	OnEndRound.Broadcast();

	if (!EncountersPerDay.Contains(KioskState->Day))
	{
		EndGame();
		return;
	}
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
			OrchestrateRules();
			StartRound();
			break;
		case EKioskPhase::EndOfDay:
			EndRound();
			break;
		case EKioskPhase::Shopping:
			break;
	}

	OnPhaseChanged.Broadcast(NewPhase);
}

void AKioskGameModeBase::OrchestrateEncounter(bool& bEncountersLeft)
{
	UE_LOG(LogKiosk, Log, TEXT("=== OrchestrateEncounter START ==="));
	UE_LOG(LogKiosk, Log,
		TEXT("Day: %d | CurrentEncounterIndex: %d | EncounterInProgress: %s"),
		KioskState->Day,
		CurrentEncounterIndex,
		b_EncounterInProgress ? TEXT("true") : TEXT("false")
	);

	bEncountersLeft = false;

	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!KioskState) return;
	if (b_EncounterInProgress || EncountersPerDay.IsEmpty()) return;


	UE_LOG(LogKiosk, Log,
		TEXT("Looking for encounter configuration for Day %d."),
		KioskState->Day
	);

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day);
	if (!DayConfig)
	{
		UE_LOG(LogKiosk, Warning,
			TEXT("OrchestrateEncounter aborted: No DayConfig found for Day %d."),
			KioskState->Day
		);

		UE_LOG(LogKiosk, Log, TEXT("Broadcasting OnNoEncounters."));
		OnNoEncounters.Broadcast();
		return;
	}

	if (!DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex))
	{
		UE_LOG(LogKiosk, Warning,
			TEXT("OrchestrateEncounter aborted: Invalid encounter index %d. CharacterOrder.Num(): %d"),
			CurrentEncounterIndex,
			DayConfig->CharacterOrder.Num()
		);

		UE_LOG(LogKiosk, Log, TEXT("Broadcasting OnNoEncounters."));
		OnNoEncounters.Broadcast();
		return;
	}

	const auto& EncounterData = DayConfig->CharacterOrder[CurrentEncounterIndex];

	TSubclassOf<AKioskCharacter> CharacterClass = EncounterData.CharacterClass;
	if (!CharacterClass) return;

	UDataTable* CharacterDialogueTable = EncounterData.CharacterConversationTable;
	if (!CharacterDialogueTable) return;

	const FGameplayTagContainer CharacterTraits = EncounterData.Traits;
	if (CharacterTraits.IsEmpty()) return;

	UTexture2D* CharacterTexture = EncounterData.CurrentCharacterTexture;
	if (!CharacterTexture) return;

	const ECharacterSex CharacterSex = EncounterData.Sex;

	AKioskCharacter* InWorldCharacter = Cast<AKioskCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), CharacterClass));
	if (!InWorldCharacter) return;

	CurrentEncounter = CharacterClass;
	CurrentEncounterCharacter = InWorldCharacter;
	CurrentCharacterEntry = EncounterData;

	b_EncounterInProgress = true;
	b_EncounterResolved = false;
	b_DialogueFinished = false;

	UE_LOG(LogKiosk, Log,
		TEXT("Encounter state initialized. InProgress: %s | Resolved: %s | DialogueFinished: %s"),
		b_EncounterInProgress ? TEXT("true") : TEXT("false"),
		b_EncounterResolved ? TEXT("true") : TEXT("false"),
		b_DialogueFinished ? TEXT("true") : TEXT("false")
	);

	bEncountersLeft = DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1);

	UE_LOG(LogKiosk, Log,
		TEXT("Encounter started. Index: %d | Character: %s | EncountersLeft: %s"),
		CurrentEncounterIndex,
		*GetNameSafe(InWorldCharacter),
		bEncountersLeft ? TEXT("true") : TEXT("false")
	);

	UE_LOG(LogKiosk, Log, TEXT("Broadcasting OnEncounterStarted."));
	OnEncounterStarted.Broadcast(InWorldCharacter);

	UE_LOG(LogKiosk, Log, TEXT("=== OrchestrateEncounter END ==="));
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

void AKioskGameModeBase::OrchestrateEvent()
{
	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (!KioskState) return;
	if (PossibleEvents.IsEmpty() || b_EventHappening) return;

	const int32 RandomIndex = FMath::RandRange(0, PossibleEvents.Num() - 1);
	TSubclassOf<AKioskGameplayEvent> EventClass = PossibleEvents[RandomIndex];

	if (!EventClass) return;

	AKioskGameplayEvent* Event = GetWorld()->SpawnActor<AKioskGameplayEvent>(
		EventClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator);
	if (!Event) return;

	Event->OnCompleted.AddUObject(
		this,
		&AKioskGameModeBase::OnGameplayEventCompleted
	);

	b_EventHappening = true;
	ActiveEvents.Add(Event);

	Event->StartEvent(this);
}

void AKioskGameModeBase::OrchestrateDayExclusiveEvents()
{
	if (!IsGamePhase(EKioskPhase::Setup)) return;
	if (!KioskState) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day);
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
		UE_LOG(LogKiosk, Warning, TEXT("Starting day-exclusive event '%s' for Day %d."), *GetNameSafe(Event), KioskState->Day);
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
	UE_LOG(LogKiosk, Warning, TEXT("Cleared day-exclusive events."));
}

void AKioskGameModeBase::OnGameplayEventCompleted(AKioskGameplayEvent* Event)
{
	if (!IsValid(Event)) return;

	ActiveEvents.Remove(Event);
	HappenedEvents.Add(Event);
	b_EventHappening = false;
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
			PenalizePlayer(Character, CurrentCharacterEntry.Traits, KioskState->Day); // should have been kept out
			break;
		
		case ERuleEvaluation::RequiredToEnter:
			RewardPlayer(Character, CurrentCharacterEntry.Traits, KioskState->Day); // correctly let in
			break;

		case ERuleEvaluation::NoApplicableRule:
			// n/a
			break;
	}


	EncounterCharactersLetIn.Add(CurrentEncounter);

	CurrentEncounter = nullptr;
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
			RewardPlayer(Character, CurrentCharacterEntry.Traits, KioskState->Day); // correctly kept out
			break;

		case ERuleEvaluation::RequiredToEnter:
			PenalizePlayer(Character, CurrentCharacterEntry.Traits, KioskState->Day); // should have been let in
			break;

		case ERuleEvaluation::NoApplicableRule:
			// n/a
			break;
	}

	CurrentEncounter = nullptr;
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
	CurrentEncounterCharacter = nullptr;
}

void AKioskGameModeBase::TryAdvanceEncounter()
{
	if (!b_EncounterResolved || !b_DialogueFinished || b_IsRepremanded) return;

	b_EncounterResolved = false;
	b_DialogueFinished = false;

	GetWorldTimerManager().SetTimer(
		TimerBetweenEncounters,
		this,
		&AKioskGameModeBase::AdvanceEncounter,
		15.0f,
		false
	);
}

void AKioskGameModeBase::AdvanceEncounter()
{
	bool bEncountersLeft = false;
	OrchestrateEncounter(bEncountersLeft);
}

ERuleEvaluation AKioskGameModeBase::EvaluateCharacterRules() const
{
	bool bRequiredToEnter = false;

	UE_LOG(LogKiosk, Verbose, TEXT("Evaluating %d applied kiosk rules."), AppliedRules.Num());

	for (UKioskRule* Rule : AppliedRules)
	{
		if (!Rule)
		{
			UE_LOG(LogKiosk, Warning, TEXT("Encountered null rule in AppliedRules."));
			continue;
		}

		const bool bMatchesRule = Rule->IsViolatedBy(CurrentCharacterEntry.Traits);

		UE_LOG(
			LogKiosk,
			Error,
			TEXT("Rule '%s' | Type: %d | Violated: %s"),
			*GetNameSafe(Rule),
			static_cast<int32>(Rule->RuleType),
			bMatchesRule ? TEXT("true") : TEXT("false")
		);

		if (!bMatchesRule)
		{
			continue;
		}

		switch (Rule->RuleType)
		{
		case EKioskRuleType::Forbiden:
		{
			UE_LOG(
				LogKiosk,
				Log,
				TEXT("Character evaluation result: Forbidden. Triggered by rule '%s'."),
				*GetNameSafe(Rule)
			);

			return ERuleEvaluation::Forbidden;
		}

		case EKioskRuleType::RequiredEntry:
		{
			UE_LOG(
				LogKiosk,
				Log,
				TEXT("Rule '%s' requires entry."),
				*GetNameSafe(Rule)
			);

			bRequiredToEnter = true;
			break;
		}

		default:
		{
			UE_LOG(
				LogKiosk,
				Warning,
				TEXT("Rule '%s' has an unhandled RuleType: %d."),
				*GetNameSafe(Rule),
				static_cast<int32>(Rule->RuleType)
			);

			break;
		}
		}
	}

	if (bRequiredToEnter)
	{
		UE_LOG(LogKiosk, Log, TEXT("Character evaluation result: RequiredToEnter."));
		return ERuleEvaluation::RequiredToEnter;
	}

	UE_LOG(LogKiosk, Log, TEXT("Character evaluation result: NoApplicableRule."));
	return ERuleEvaluation::NoApplicableRule;
}

void AKioskGameModeBase::PenalizePlayer(AKioskCharacter* Character, FGameplayTagContainer Traits, int GameDay)
{
	++Mistakes;
	OnPenalizePlayer.Broadcast(Character, Traits, KioskState->Day);
}

void AKioskGameModeBase::RewardPlayer(AKioskCharacter* Character, FGameplayTagContainer Traits, int GameDay)
{
	++CorrectlyProcessed;
	OnRewardPlayer.Broadcast(Character, Traits, KioskState->Day);
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

	if (const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(KioskState->Day))
	{
		if (DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1))
		{
			return true;
		}
	}

	for (const auto& Pair : EncountersPerDay)
	{
		if (Pair.Key > KioskState->Day && !Pair.Value.CharacterOrder.IsEmpty())
		{
			return true;
		}
	}

	return false;
}