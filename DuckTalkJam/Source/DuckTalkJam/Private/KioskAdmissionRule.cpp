// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskAdmissionRule.h"

bool UKioskAdmissionRule::IsViolatedBy_Implementation(AKioskCharacter* Character) const
{
	if (!Character) return false;

	const TSubclassOf<AKioskCharacter> CharacterClass = Character->GetClass();
	if (ExpectedCharacters.Contains(CharacterClass)) return false;

	return false;
}