// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KioskCharacter.h"
#include "KioskCharacterEntry.generated.h"

USTRUCT(BlueprintType)
struct DUCKTALKJAM_API FKioskCharacterEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AKioskCharacter> CharacterClass;

};