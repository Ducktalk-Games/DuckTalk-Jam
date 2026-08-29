// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "KioskState.generated.h"

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API AKioskState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Night = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Coins = 0;

	/*
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerItem> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Modifier> Modifiers;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_NoteItem> Notes;

	*/

	UFUNCTION(BlueprintCallable)
	void PopulateItems();

	UFUNCTION(BlueprintCallable)
	void ListModifiers();

	UFUNCTION(BlueprintCallable)
	void LoadNotes();
};