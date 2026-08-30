// Fill out your copyright notice in the Description page of Project Settings.


#include "KioskTagRule.h"
#include "KioskCharacter.h"

bool UKioskTagRule::IsViolatedBy_Implementation(AKioskCharacter* Character) const
{
	if (!Character) return false;
	return Character->Traits.HasTag(ForbiddenTag);
}