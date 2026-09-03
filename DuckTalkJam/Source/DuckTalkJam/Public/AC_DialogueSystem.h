// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "F_DialogueRow.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AC_DialogueSystem.generated.h"

class AKioskState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUCKTALKJAM_API UAC_DialogueSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_DialogueSystem();

	UPROPERTY()
	TObjectPtr<AKioskState> KioskState;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Dialogue",
		meta = (ToolTip = "The Data Table containing all dialogue rows for this character.")
	)
	TObjectPtr<UDataTable> DialogueTable;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueStarted OnDialogueStarted;
	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Starts the dialogue from the previously set Dialogue Table. Sets b_IsInDialogue to true.")
	)
	void StartDialogue();

	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Advances the current dialogue.\nRowName: The Data Table row to load next.")
	)
	void AdvanceDialogue(FName RowName);

	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Gets the current dialogue node.")
	)
	void GetCurrentNode(FF_DialogueRow& OutNode);
	FF_DialogueRow* CurrentNode;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueEnded OnDialogueEnded;
	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Ends the current dialogue. Sets b_IsInDialogue to false.")
	)
	void EndDialogue();

	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Returns all choices for the node.")
	)
	TArray<FF_DialogueChoice> GetChoices();

	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Selects a choice to progress the dialogue.")
	)
	void SelectChoice(const FF_DialogueChoice& Choice, FName& OutNextRow);

	UFUNCTION(
		BlueprintCallable,
		Category = "Dialogue",
		meta = (ToolTip = "Tries to find a choice with a flag in the dialogue table.")
	)
	bool FindChoiceWithFlagInTable(FGameplayTagContainer FlagToFind, FF_DialogueChoice& OutChoice, FName& OutRowName) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool b_IsInDialogue = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};