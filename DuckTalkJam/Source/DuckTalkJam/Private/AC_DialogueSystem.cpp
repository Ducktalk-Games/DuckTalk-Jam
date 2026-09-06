// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "AC_DialogueSystem.h"
#include "F_DialogueRow.h"
#include "Engine/DataTable.h"
#include "KioskState.h"


// Sets default values for this component's properties
UAC_DialogueSystem::UAC_DialogueSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UAC_DialogueSystem::BeginPlay()
{
	Super::BeginPlay();
	KioskState = GetWorld()->GetGameInstance<UKioskState>();
}

// Called every frame
void UAC_DialogueSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAC_DialogueSystem::StartDialogue()
{
	if (!DialogueTable)
	{
		UE_LOG(LogTemp, Error, TEXT("No dialogue table found."));
		return;
	}

	FF_DialogueRow* EntryRow = ResolveDialogueRow(FName("DIAG_INTRO_001"));

	if (!EntryRow)
	{
		UE_LOG(LogTemp, Log, TEXT("No valid dialogue entry row found."));
		EndDialogue();
		return;
	}

	CurrentNode = EntryRow;
	b_IsInDialogue = true;

	OnDialogueStarted.Broadcast();
}

void UAC_DialogueSystem::AdvanceDialogue(FName RowName)
{
	if (!DialogueTable) return;

	if (RowName.IsNone())
	{
		EndDialogue();
		return;
	}

	FF_DialogueRow* RowToAdvance = ResolveDialogueRow(RowName);
	if (!RowToAdvance)
	{
		UE_LOG(LogTemp, Log, TEXT("No valid dialogue row found. Ending dialogue."));
		EndDialogue();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s: %s"), *RowToAdvance->Speaker.ToString(), *RowToAdvance->DialogueText.ToString());
	CurrentNode = RowToAdvance;
}

void UAC_DialogueSystem::GetCurrentNode(FF_DialogueRow& OutNode)
{
	OutNode = CurrentNode ? *CurrentNode : FF_DialogueRow();
}

FF_DialogueRow* UAC_DialogueSystem::ResolveDialogueRow(FName RowName)
{
	if (!DialogueTable || RowName.IsNone()) return nullptr;

	TSet<FName> VisitedRows;

	while (!RowName.IsNone())
	{
		if (VisitedRows.Contains(RowName))
		{
			UE_LOG(LogTemp, Error, TEXT("Circular dialogue NextRow chain detected at '%s'"), *RowName.ToString());
			return nullptr;
		}

		VisitedRows.Add(RowName);

		FF_DialogueRow* Row = DialogueTable->FindRow<FF_DialogueRow>(RowName, TEXT("Resolve dialogue row lookup"));
		if (!Row)
		{
			UE_LOG(LogTemp, Error, TEXT("Row '%s' not found in dialogue table."), *RowName.ToString());
			return nullptr;
		}

		if (!Row->RequiredFlag.IsValid()) return Row;
		if (KioskState && KioskState->HasFlag(Row->RequiredFlag)) return Row;

		UE_LOG(LogTemp, Log, TEXT("Skipping dialogue row '%s': missing required flag '%s'."), *RowName.ToString(), *Row->RequiredFlag.ToString());
		RowName = Row->NextRow;
	}

	return nullptr;
}

void UAC_DialogueSystem::EndDialogue()
{
	// Necessary for phone dialogues to work separately
	OnDialogueEnded.Broadcast(CurrentNode && CurrentNode->b_IsPhoneCallDialogue);
	
	CurrentNode = nullptr;
	DialogueTable = nullptr;
	b_IsInDialogue = false;

	UE_LOG(LogTemp, Log, TEXT("Dialogue ended."));
}

TArray<FF_DialogueChoice> UAC_DialogueSystem::GetChoices()
{
	if (!CurrentNode) return {};
	return CurrentNode->Choices;
}

void UAC_DialogueSystem::SelectChoice(
	const FF_DialogueChoice& Choice,
	FName& OutNextRow)
{
	OutNextRow = NAME_None;

	if (!DialogueTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot select choice: no dialogue table has been found."));
		EndDialogue();
		return;
	}

	if (Choice.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Selected dialogue choice is empty."));
		EndDialogue();
		return;
	}

	if (Choice.Flag.IsValid()) KioskState->AddFlag(Choice.Flag);

	const bool bShouldEndDialogue = Choice.NextRow.IsNone();
	if (bShouldEndDialogue)
	{
		UE_LOG(LogTemp, Log, TEXT("Choice has no next row. Ending dialogue."));
		EndDialogue();
	}
	else OutNextRow = Choice.NextRow;

	OnChoiceWithFunction.Broadcast(Choice.ChoiceFunction);
}

bool UAC_DialogueSystem::FindChoiceWithFlagInTable(
	FGameplayTagContainer FlagToFind,
	FF_DialogueChoice& OutChoice,
	FName& OutRowName) const
{
	OutChoice = FF_DialogueChoice();
	OutRowName = NAME_None;

	if (!DialogueTable || !FlagToFind.IsValid()) return false;

	const TMap<FName, uint8*>& RowMap = DialogueTable->GetRowMap();
	for (const TPair<FName, uint8*>& RowPair : RowMap)
	{
		const FName RowName = RowPair.Key;
		const FF_DialogueRow* DialogueRow = reinterpret_cast<const FF_DialogueRow*>(RowPair.Value);
		if (!DialogueRow) continue;

		for (const FF_DialogueChoice& Choice : DialogueRow->Choices)
		{
			if (Choice.Flag.IsValid() && FlagToFind.HasTag(Choice.Flag))
			{
				OutChoice = Choice;
				OutRowName = RowName;
				return true;
			}
		}
	}

	return false;
}