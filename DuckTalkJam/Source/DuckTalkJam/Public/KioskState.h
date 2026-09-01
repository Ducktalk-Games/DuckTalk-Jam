// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DayEncounterConfig.h"
#include "GameFramework/GameStateBase.h"
#include "KioskState.generated.h"

UENUM(BlueprintType)
enum class EKioskPhase : uint8
{
	Setup		UMETA(DisplayName = "Setup"),
	Playing		UMETA(DisplayName = "Playing"),
	EndOfDay	UMETA(DisplayName = "End Of Day")
};

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API AKioskState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 Day = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	float Coins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	bool b_ItemsInitialized = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	EKioskPhase CurrentPhase = EKioskPhase::Setup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	TArray<FName> Flags;

	/*
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Modifier> Modifiers;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_NoteItem> Notes;

	*/

#pragma region Flags

	UFUNCTION(BlueprintCallable)
	void AddFlag(FName Flag);

	UFUNCTION(BlueprintCallable)
	bool HasFlag(FName Flag);

	UFUNCTION(BlueprintCallable)
	void RemoveFlag(FName Flag);

#pragma endregion Flags

	UFUNCTION(BlueprintCallable)
	void PopulateItems();

	UFUNCTION(BlueprintCallable)
	void ListModifiers();

	UFUNCTION(BlueprintCallable)
	void LoadNotes();
};