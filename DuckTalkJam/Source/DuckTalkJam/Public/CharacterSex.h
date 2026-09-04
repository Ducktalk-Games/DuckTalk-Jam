// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSex.generated.h"

/**
 *  Sex? Woman, man… sometimes goat.
 */
UENUM(BlueprintType)
enum class FCharacterSex : uint8
{
	Male,
	Female
};