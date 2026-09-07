// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskGameplayEvent.h"

void AKioskGameplayEvent::BeginPlay()
{
	Super::BeginPlay();
}

void AKioskGameplayEvent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AKioskGameplayEvent::StartEvent_Implementation(AKioskGameModeBase* GameMode)
{}

bool AKioskGameplayEvent::IsCompleted_Implementation(AKioskGameModeBase* GameMode) const
{
	return false;
}