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
	UE_LOG(LogTemp, Log, TEXT("=== OrchestrateEncounter START ==="));
	
	bEncountersLeft = false;

	if (!IsGamePhase(EKioskPhase::Playing)) return;
	if (b_EncounterInProgress || EncountersPerDay.IsEmpty()) return;
	if (!KioskState) return;

	const FDayEncounterConfig* DayConfig = EncountersPerDay.Find(Day);

	if (!DayConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrchestrateEncounter aborted: No DayConfig found for Day %d."), Day);
		OnNoEncounters.Broadcast();
		return;
	}

	if (!DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("OrchestrateEncounter aborted: Invalid encounter index %d. CharacterOrder.Num(): %d"),
			CurrentEncounterIndex, DayConfig->CharacterOrder.Num()
		);

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

	bEncountersLeft = DayConfig->CharacterOrder.IsValidIndex(CurrentEncounterIndex + 1);
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

	const int32 RandomIndex = FMath::RandRange(0, PossibleEvents.Num() - 1); // Get a random index from PossibleEvents
	TSubclassOf<AKioskGameplayEvent> EventClass = PossibleEvents[RandomIndex]; // get the event class at that index

	AKioskGameplayEvent* Event = NewObject<AKioskGameplayEvent>(this, EventClass); // Create an instance of that event class
	if (!Event) return;

	Event->StartEvent(this);
	b_EventHappening = true;
	ActiveEvents.Add(Event);
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
	if (!b_EncounterResolved || !b_DialogueFinished)return;

	b_EncounterResolved = false;
	b_DialogueFinished = false;

	GetWorldTimerManager().SetTimer(
		TimerBetweenEncounters,
		this,
		&AKioskGameModeBase::AdvanceEncounter,
		30.0f,
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