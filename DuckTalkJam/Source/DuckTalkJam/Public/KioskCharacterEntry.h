// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

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