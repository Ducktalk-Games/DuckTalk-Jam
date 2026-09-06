// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskState.h"
#include "DayEncounterConfig.h"

void UKioskState::AddFlag(FGameplayTag Flag)
{
	if (!Flags.Contains(Flag)) Flags.Add(Flag);
}

bool UKioskState::HasFlag(FGameplayTag Flag)
{
	return Flags.Contains(Flag);
}

void UKioskState::RemoveFlag(FGameplayTag Flag)
{
	if (Flags.Contains(Flag)) Flags.Remove(Flag);
}

void UKioskState::AddItem(FGameplayTag ItemTag)
{
	if (!Items.Contains(ItemTag)) Items.Add(ItemTag);
}

bool UKioskState::HasItem(FGameplayTag ItemTag)
{
	if (Items.Contains(ItemTag)) return true;
	return false;
}

void UKioskState::RemoveItem(FGameplayTag ItemTag)
{
	if (Items.Contains(ItemTag)) Items.Remove(ItemTag);
}