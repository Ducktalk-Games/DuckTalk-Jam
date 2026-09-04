// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskTagRule.h"
#include "KioskCharacter.h"

bool UKioskTagRule::IsViolatedBy_Implementation(AKioskCharacter* Character) const
{
	if (!Character) return false;
	return Character->Traits.HasTag(ForbiddenTag);
}