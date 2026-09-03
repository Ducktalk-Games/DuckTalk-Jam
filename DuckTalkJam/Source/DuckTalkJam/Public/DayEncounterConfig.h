// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KioskCharacterEntry.h"
#include "DayEncounterConfig.generated.h"

USTRUCT(BlueprintType)
struct DUCKTALKJAM_API FDayEncounterConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKioskCharacterEntry> CharacterOrder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDataTable> BossDayConversationTable;
};