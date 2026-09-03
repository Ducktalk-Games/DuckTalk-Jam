// Fill out your copyright notice in the Description page of Project Settings.


#include "KioskAdmissionRule.h"

bool UKioskAdmissionRule::IsViolatedBy_Implementation(AKioskCharacter* Character) const
{
	if (!Character) return false;

	const TSubclassOf<AKioskCharacter> CharacterClass = Character->GetClass();
	if (ExpectedCharacters.Contains(CharacterClass)) return false;

	return false;
}