// Fill out your copyright notice in the Description page of Project Settings.


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

void AKioskState::RemoveItem(FGameplayTagContainer ItemTag)
{
	if (Items.Contains(ItemTag)) Items.Remove(ItemTag);
}