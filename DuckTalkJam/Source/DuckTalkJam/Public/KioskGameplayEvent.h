// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KioskGameplayEvent.generated.h"

class AKioskGameModeBase;
class AKioskGameplayEvent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnKioskGameplayEventCompleted, AKioskGameplayEvent*);

UCLASS()
class DUCKTALKJAM_API AKioskGameplayEvent : public AActor
{
	GENERATED_BODY()

public:
	AKioskGameplayEvent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartEvent(AKioskGameModeBase* GameMode);

	virtual void StartEvent_Implementation(AKioskGameModeBase* GameMode);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Event")
	void CompleteEvent();

	FOnKioskGameplayEventCompleted OnCompleted;

protected:
	virtual void BeginPlay() override;
};