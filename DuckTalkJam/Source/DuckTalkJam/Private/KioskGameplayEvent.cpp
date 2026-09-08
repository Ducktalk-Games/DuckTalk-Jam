// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskGameplayEvent.h"

AKioskGameplayEvent::AKioskGameplayEvent()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKioskGameplayEvent::BeginPlay()
{
	Super::BeginPlay();
}

void AKioskGameplayEvent::StartEvent_Implementation(AKioskGameModeBase* GameMode)
{}

void AKioskGameplayEvent::CompleteEvent()
{
	OnCompleted.Broadcast(this);
}