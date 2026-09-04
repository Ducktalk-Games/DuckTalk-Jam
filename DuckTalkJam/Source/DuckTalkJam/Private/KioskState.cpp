// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskState.h"
#include "DayEncounterConfig.h"

void AKioskState::AddFlag(FGameplayTag Flag)
{
	if (!Flags.Contains(Flag)) Flags.Add(Flag);
}

bool AKioskState::HasFlag(FGameplayTag Flag)
{
	return Flags.Contains(Flag);
}

void AKioskState::RemoveFlag(FGameplayTag Flag)
{
	if (Flags.Contains(Flag)) Flags.Remove(Flag);
}

void AKioskState::AddItem(FGameplayTagContainer ItemTag)
{
	if (!Items.Contains(ItemTag)) Items.Add(ItemTag);
}

bool AKioskState::HasItem(FGameplayTagContainer ItemTag)
{
	if (Items.Contains(ItemTag)) return true;
	return false;
}

void AKioskState::RemoveItem(FGameplayTagContainer ItemTag)
{
	if (Items.Contains(ItemTag)) Items.Remove(ItemTag);
}