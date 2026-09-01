// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_DialogueSystem.h"
#include "F_DialogueRow.h"
#include "Engine/DataTable.h"


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

	FF_DialogueRow* EntryRow = DialogueTable->FindRow<FF_DialogueRow>(FName("DIAG_INTRO_001"), TEXT("Start dialogue startup node lookup"));
	if (!EntryRow)
	{
		UE_LOG(LogTemp, Error, TEXT("Database has no entry row."));
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

	FF_DialogueRow* RowToAdvance = DialogueTable->FindRow<FF_DialogueRow>(RowName, TEXT("Dialogue advance lookup"));
	if (!RowToAdvance)
	{
		UE_LOG(LogTemp, Error, TEXT("Row not found. Ending dialogue."));
		EndDialogue();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s: %s"),
		*RowToAdvance->Speaker.ToString(),
		*RowToAdvance->DialogueText.ToString());
	CurrentNode = RowToAdvance;
}

void UAC_DialogueSystem::GetCurrentNode(FF_DialogueRow& OutNode)
{
	OutNode = CurrentNode ? *CurrentNode : FF_DialogueRow();
}

void UAC_DialogueSystem::EndDialogue()
{
	b_IsInDialogue = false;
	CurrentNode = nullptr;
	DialogueTable = nullptr;

	UE_LOG(LogTemp, Log, TEXT("Dialogue ended."));
	OnDialogueEnded.Broadcast();
}

TArray<FF_DialogueChoice> UAC_DialogueSystem::GetChoices()
{
	if (!CurrentNode) return {};
	return CurrentNode->Choices;
}

void UAC_DialogueSystem::SelectChoice(const FF_DialogueChoice& Choice, FName& OutNextRow)
{
	OutNextRow = NAME_None;

	if (!DialogueTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot select choice: no dialogue table has been found."));
		EndDialogue();
		return;
	}

	// Completely invalid / blank choice
	if (Choice.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Selected dialogue choice is empty."));
		EndDialogue();
		return;
	}

	// Valid choice, but deliberately has nowhere to go
	if (Choice.NextRow.IsNone())
	{
		UE_LOG(LogTemp, Log,
			TEXT("Choice has no next row. Ending dialogue."));
		EndDialogue();
		return;
	}

	OutNextRow = Choice.NextRow;
}