// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskAllowedTagRule.h"
#include "GameplayTagContainer.h"
#include "KioskCharacter.h"

bool UKioskAllowedTagRule::IsViolatedBy_Implementation(FGameplayTagContainer Traits) const
{
	if (Traits.IsEmpty()) return false;

	const bool bViolated = Traits.HasTag(AllowedTags);
	return bViolated;
}