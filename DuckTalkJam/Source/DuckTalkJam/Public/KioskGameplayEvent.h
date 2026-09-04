// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "KioskGameplayEvent.generated.h"

class AKioskGameModeBase;

UCLASS(Abstract, Blueprintable)
class DUCKTALKJAM_API UKioskGameplayEvent : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintCallable,
		meta = (ToolTip = "Called when the event begins. Use this for presenting the event to the player, such as UI messages, phone calls, prompts, sounds, or other setup that tells the player what is happening.")
	)
	void StartEvent(AKioskGameModeBase* GameMode);

	UFUNCTION(
		BlueprintNativeEvent,
		BlueprintCallable,
		meta = (ToolTip = "Checks whether the gameplay condition for this event has been completed. Use this for actual game-state checks, such as whether the radio is off, an item was removed, or another required world condition is true.")
	)
	bool IsCompleted(AKioskGameModeBase* GameMode) const;

	virtual void StartEvent_Implementation(AKioskGameModeBase* GameMode);
	virtual bool IsCompleted_Implementation(AKioskGameModeBase* GameMode) const;
};