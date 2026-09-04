// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskTagRule.h"
#include "GameplayTagContainer.h"
#include "KioskCharacter.h"

bool UKioskTagRule::IsViolatedBy_Implementation(FGameplayTagContainer Traits) const
{
	if (Traits.IsEmpty()) return false;
	return Traits.HasTag(ForbiddenTag);
}