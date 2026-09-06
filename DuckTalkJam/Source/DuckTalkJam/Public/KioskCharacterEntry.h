// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KioskCharacter.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CharacterSex.h"
#include "KioskCharacterEntry.generated.h"

USTRUCT(BlueprintType)
struct DUCKTALKJAM_API FKioskCharacterEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterSex Sex = ECharacterSex::Male;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AKioskCharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDataTable> CharacterConversationTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FGameplayTagContainer Traits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UTexture2D* CurrentCharacterTexture = nullptr;

};